/*
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 - 2021 Trustwave Holdings, Inc. (http://www.trustwave.com/)
 *
 * You may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * If any of the files related to licensing are missing or if you have any
 * other questions related to licensing please contact Trustwave Holdings, Inc.
 * directly using the email address security@modsecurity.org.
 *
 */

#include <string.h>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "modsecurity/rules_set.h"
#include "modsecurity/modsecurity.h"

using modsecurity::Transaction;

namespace {

unsigned char response_body[] = "" \
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\r" \
    "<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " \
    "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" " \
    "xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\n\r" \
    "  <soap:Body>\n\r" \
    "  <EnlightenResponse xmlns=\"http://clearforest.com/\">\n\r" \
    "  <EnlightenResult>string</EnlightenResult>\n\r" \
    "  </EnlightenResponse>\n\r" \
    "  </soap:Body>\n\r" \
    "</soap:Envelope>\n\r";

char ip[] = "200.249.12.31";

const char* const help_message = R"(Usage: benchmark [num_iterations] [options]

Options:
  --request-file <path>  Load one raw HTTP request message from a file.
  --request-dir <path>   Load raw HTTP request messages from all regular files in a directory.
  --rules-file <path>    Load a different rules file (default: basic_rules.conf).
  -h, -?, --help         Show this help.

Raw request files use a simple HTTP-message format:
  POST /submit HTTP/1.1
  Host: example.test
  Content-Type: application/x-www-form-urlencoded

  a=1&b=2

The default behavior is unchanged: without --request-file or --request-dir,
the benchmark uses the historical built-in GET request. Supplying request files
allows reproducible experiments with small GETs, URL-encoded bodies, JSON,
large bodies, or CRS target-expansion payloads. The timing summary separates
ModSecurity phases, but it does not yet split operator/regex/transformation time;
that requires additional instrumentation inside rule/operator execution.)";

struct BenchmarkRequest {
    std::string name;
    std::string method;
    std::string uri;
    std::string http_version;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;
};

struct BenchmarkOptions {
    unsigned long long num_requests = 1000000;
    std::string rules_file = "basic_rules.conf";
    std::string request_file;
    std::string request_dir;
};

struct TimingTotals {
    uint64_t total_us = 0;
    uint64_t connection_us = 0;
    uint64_t uri_us = 0;
    uint64_t request_headers_us = 0;
    uint64_t request_body_us = 0;
    uint64_t response_headers_us = 0;
    uint64_t response_body_us = 0;
    uint64_t logging_us = 0;
};

uint64_t elapsed_us(std::chrono::steady_clock::time_point start) {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start).count();
}

std::string trim_cr(std::string s) {
    if (!s.empty() && s.back() == '\r') {
        s.pop_back();
    }
    return s;
}

BenchmarkRequest default_request() {
    BenchmarkRequest req;
    req.name = "builtin-default-get";
    req.method = "GET";
    req.uri = "/test.pl?param1=test&para2=test2";
    req.http_version = "1.1";
    req.headers = {
        {"Host", "net.tutsplus.com"},
        {"User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; "
            "rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)"},
        {"Accept", "text/html,application/xhtml+xml,application/xml;"
            "q=0.9,*/*;q=0.8"},
        {"Accept-Language", "en-us,en;q=0.5"},
        {"Accept-Encoding", "gzip,deflate"},
        {"Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*/*;q=0.7"},
        {"Keep-Alive", "300"},
        {"Connection", "keep-alive"},
        {"Cookie", "PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120"},
        {"Pragma", "no-cache"},
        {"Cache-Control", "no-cache"},
    };
    return req;
}

bool parse_request_message(const std::string& data, const std::string& name,
    BenchmarkRequest *req, std::string *error) {
    std::istringstream input(data);
    std::string line;

    if (!std::getline(input, line)) {
        *error = "empty request file: " + name;
        return false;
    }

    line = trim_cr(line);
    std::istringstream request_line(line);
    std::string http_token;
    if (!(request_line >> req->method >> req->uri >> http_token)) {
        *error = "invalid request line in " + name + ": " + line;
        return false;
    }

    const std::string http_prefix = "HTTP/";
    if (http_token.compare(0, http_prefix.size(), http_prefix) == 0) {
        req->http_version = http_token.substr(http_prefix.size());
    } else {
        req->http_version = http_token;
    }
    req->name = name;

    while (std::getline(input, line)) {
        line = trim_cr(line);
        if (line.empty()) {
            break;
        }

        const auto colon = line.find(':');
        if (colon == std::string::npos) {
            *error = "invalid header in " + name + ": " + line;
            return false;
        }

        std::string value = line.substr(colon + 1);
        if (!value.empty() && value.front() == ' ') {
            value.erase(0, 1);
        }
        req->headers.emplace_back(line.substr(0, colon), value);
    }

    std::ostringstream body;
    body << input.rdbuf();
    req->body = body.str();

    return true;
}

bool load_request_file(const std::string& path, BenchmarkRequest *req,
    std::string *error) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        *error = "failed to open request file: " + path;
        return false;
    }

    std::ostringstream data;
    data << file.rdbuf();
    return parse_request_message(data.str(), path, req, error);
}

bool load_request_directory(const std::string& path,
    std::vector<BenchmarkRequest> *requests, std::string *error) {
    std::vector<std::filesystem::path> files;
    std::error_code ec;

    for (const auto& entry : std::filesystem::directory_iterator(path, ec)) {
        if (ec) {
            *error = "failed to read request directory: " + path + ": "
                + ec.message();
            return false;
        }
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }

    std::sort(files.begin(), files.end());

    for (const auto& file : files) {
        BenchmarkRequest req;
        if (!load_request_file(file.string(), &req, error)) {
            return false;
        }
        requests->push_back(std::move(req));
    }

    if (requests->empty()) {
        *error = "request directory contains no regular files: " + path;
        return false;
    }

    return true;
}

bool parse_options(int argc, const char *argv[], BenchmarkOptions *options) {
    bool num_requests_set = false;

    for (int i = 1; i < argc; i++) {
        const std::string arg(argv[i]);

        if (arg == "-h" || arg == "-?" || arg == "--help") {
            std::cout << help_message << std::endl;
            return false;
        }

        if (arg == "--request-file" || arg == "--request-dir"
                || arg == "--rules-file") {
            if (i + 1 >= argc) {
                std::cerr << "Missing value for " << arg << std::endl;
                std::cerr << help_message << std::endl;
                exit(-1);
            }

            const std::string value(argv[++i]);
            if (arg == "--request-file") {
                options->request_file = value;
            } else if (arg == "--request-dir") {
                options->request_dir = value;
            } else {
                options->rules_file = value;
            }
            continue;
        }

        if (!num_requests_set) {
            errno = 0;
            char *endptr = nullptr;
            unsigned long long upper = strtoull(arg.c_str(), &endptr, 10);
            if (!errno && endptr != arg.c_str() && *endptr == '\0' && upper) {
                options->num_requests = upper;
                num_requests_set = true;
                continue;
            }
        }

        std::cerr << "Unknown argument: " << arg << std::endl;
        std::cerr << help_message << std::endl;
        exit(-1);
    }

    if (!options->request_file.empty() && !options->request_dir.empty()) {
        std::cerr << "Use only one of --request-file or --request-dir" << std::endl;
        exit(-1);
    }

    return true;
}

void print_timing_summary(const TimingTotals& timings,
    unsigned long long num_requests) {
    const auto avg = [num_requests](uint64_t total) -> double {
        if (num_requests == 0) {
            return 0;
        }
        return static_cast<double>(total) / num_requests;
    };

    std::cout << "Timing summary (microseconds/request):" << std::endl;
    std::cout << "  total: " << avg(timings.total_us) << std::endl;
    std::cout << "  connection: " << avg(timings.connection_us) << std::endl;
    std::cout << "  uri: " << avg(timings.uri_us) << std::endl;
    std::cout << "  request_headers: " << avg(timings.request_headers_us) << std::endl;
    std::cout << "  request_body: " << avg(timings.request_body_us) << std::endl;
    std::cout << "  response_headers: " << avg(timings.response_headers_us) << std::endl;
    std::cout << "  response_body: " << avg(timings.response_body_us) << std::endl;
    std::cout << "  logging: " << avg(timings.logging_us) << std::endl;
    std::cout << "Note: operator/regex/transformation timing requires additional "
        "instrumentation inside rule evaluation." << std::endl;
}

}  // namespace

int main(int argc, const char *argv[]) {
    BenchmarkOptions options;
    if (!parse_options(argc, argv, &options)) {
        return 0;
    }

    std::vector<BenchmarkRequest> requests;
    std::string error;
    if (!options.request_file.empty()) {
        BenchmarkRequest req;
        if (!load_request_file(options.request_file, &req, &error)) {
            std::cerr << error << std::endl;
            return -1;
        }
        requests.push_back(std::move(req));
    } else if (!options.request_dir.empty()) {
        if (!load_request_directory(options.request_dir, &requests, &error)) {
            std::cerr << error << std::endl;
            return -1;
        }
    } else {
        requests.push_back(default_request());
    }

    std::cout << "Doing " << options.num_requests << " transactions with "
        << requests.size() << " request input(s)...\n";

    auto modsec = std::make_unique<modsecurity::ModSecurity>();
    modsec->setConnectorInformation("ModSecurity-benchmark v0.0.1-alpha" \
            " (ModSecurity benchmark utility)");

    auto rules = std::make_unique<modsecurity::RulesSet>();
    if (rules->loadFromUri(options.rules_file.c_str()) < 0) {
        std::cout << "Problems loading the rules..." << std::endl;
        std::cout << rules->m_parserError.str() << std::endl;
        return -1;
    }

    modsecurity::ModSecurityIntervention it;
    modsecurity::intervention::clean(&it);
    TimingTotals timings;

    for (unsigned long long i = 0; i < options.num_requests; i++) {
        const BenchmarkRequest& req = requests[i % requests.size()];
        auto total_start = std::chrono::steady_clock::now();
        Transaction *modsecTransaction = new Transaction(modsec.get(),
            rules.get(), NULL);

        auto phase_start = std::chrono::steady_clock::now();
        modsecTransaction->processConnection(ip, 12345, "127.0.0.1", 80);
        timings.connection_us += elapsed_us(phase_start);

        if (modsecTransaction->intervention(&it)) {
            std::cout << "There is an intervention" << std::endl;
            goto next_request;
        }

        phase_start = std::chrono::steady_clock::now();
        modsecTransaction->processURI(req.uri.c_str(), req.method.c_str(),
            req.http_version.c_str());
        timings.uri_us += elapsed_us(phase_start);

        if (modsecTransaction->intervention(&it)) {
            std::cout << "There is an intervention" << std::endl;
            goto next_request;
        }

        for (const auto& header : req.headers) {
            modsecTransaction->addRequestHeader(header.first, header.second);
        }

        phase_start = std::chrono::steady_clock::now();
        modsecTransaction->processRequestHeaders();
        timings.request_headers_us += elapsed_us(phase_start);

        if (modsecTransaction->intervention(&it)) {
            std::cout << "There is an intervention" << std::endl;
            goto next_request;
        }

        phase_start = std::chrono::steady_clock::now();
        if (!req.body.empty()) {
            modsecTransaction->appendRequestBody(
                reinterpret_cast<const unsigned char*>(req.body.data()),
                req.body.size());
        }
        modsecTransaction->processRequestBody();
        timings.request_body_us += elapsed_us(phase_start);

        if (modsecTransaction->intervention(&it)) {
            std::cout << "There is an intervention" << std::endl;
            goto next_request;
        }

        modsecTransaction->addResponseHeader("HTTP/1.1", "200 OK");
        modsecTransaction->addResponseHeader("Content-Type",
            "text/xml; charset=utf-8");
        modsecTransaction->addResponseHeader("Content-Length", "200");

        phase_start = std::chrono::steady_clock::now();
        modsecTransaction->processResponseHeaders(200, "HTTP 1.2");
        timings.response_headers_us += elapsed_us(phase_start);

        if (modsecTransaction->intervention(&it)) {
            std::cout << "There is an intervention" << std::endl;
            goto next_request;
        }

        modsecTransaction->appendResponseBody(response_body,
            strlen((const char*)response_body));

        phase_start = std::chrono::steady_clock::now();
        modsecTransaction->processResponseBody();
        timings.response_body_us += elapsed_us(phase_start);

        if (modsecTransaction->intervention(&it)) {
            std::cout << "There is an intervention" << std::endl;
            goto next_request;
        }

next_request:
        phase_start = std::chrono::steady_clock::now();
        modsecTransaction->processLogging();
        timings.logging_us += elapsed_us(phase_start);
        delete modsecTransaction;
        modsecurity::intervention::free(&it);
        modsecurity::intervention::clean(&it);
        timings.total_us += elapsed_us(total_start);
    }

    print_timing_summary(timings, options.num_requests);
}
