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

#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <vector>

#include "modsecurity/rules_set.h"
#include "modsecurity/modsecurity.h"

using modsecurity::Transaction;

namespace {

constexpr const char *kRequestUri = "/test.pl?param1=test&para2=test2";
constexpr const char *kClientIp = "198.51.100.10";  // RFC 5737 documentation range
constexpr const char *kServerIp = "198.51.100.20";  // RFC 5737 documentation range
constexpr const char *kDefaultRulesFile = "basic_rules.conf";
constexpr const char *kDefaultScenario = "legacy-full";

}  // namespace

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

const char* const help_message =
    "Usage: benchmark [num_iterations] "
    "[--scenario legacy-full|request-only] "
    "[--rules-file PATH] "
    "[-h|-?|--help]";

struct BenchmarkConfig {
    unsigned long long numRequests = 1000000;
    std::string scenarioName = kDefaultScenario;
    std::string rulesFile = kDefaultRulesFile;
};

struct BenchmarkCounters {
    unsigned long long interventions = 0;
};

struct BenchmarkRunSummary {
    BenchmarkConfig config;
    BenchmarkCounters counters;
    std::chrono::nanoseconds elapsed = std::chrono::nanoseconds::zero();
};

class ScenarioProvider {
 public:
    virtual ~ScenarioProvider() = default;
    virtual const char *name() const = 0;
    virtual bool execute(Transaction *transaction,
        modsecurity::ModSecurityIntervention *intervention,
        BenchmarkCounters *counters) const = 0;
};

class LegacyFullScenario : public ScenarioProvider {
 public:
    const char *name() const override {
        return "legacy-full";
    }

    bool execute(Transaction *transaction,
        modsecurity::ModSecurityIntervention *intervention,
        BenchmarkCounters *counters) const override {
        transaction->processConnection(kClientIp, 12345, kServerIp, 80);
        if (transaction->intervention(intervention)) {
            std::cout << "There is an intervention" << std::endl;
            counters->interventions++;
            return false;
        }

        transaction->processURI(kRequestUri, "GET", "1.1");
        if (transaction->intervention(intervention)) {
            std::cout << "There is an intervention" << std::endl;
            counters->interventions++;
            return false;
        }

        transaction->addRequestHeader("Host", "net.tutsplus.com");
        transaction->addRequestHeader("User-Agent",
            "Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.5) "
            "Gecko/20091102 Firefox/3.5.5 (.NET CLR 3.5.30729)");
        transaction->addRequestHeader("Accept",
            "text/html,application/xhtml+xml,application/xml;"
            "q=0.9,*/*;q=0.8");
        transaction->addRequestHeader("Accept-Language", "en-us,en;q=0.5");
        transaction->addRequestHeader("Accept-Encoding", "gzip,deflate");
        transaction->addRequestHeader("Accept-Charset",
            "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
        transaction->addRequestHeader("Keep-Alive", "300");
        transaction->addRequestHeader("Connection", "keep-alive");
        transaction->addRequestHeader("Cookie",
            "PHPSESSID=r2t5uvjq435r4q7ib3vtdjq120");
        transaction->addRequestHeader("Pragma", "no-cache");
        transaction->addRequestHeader("Cache-Control", "no-cache");
        transaction->processRequestHeaders();
        if (transaction->intervention(intervention)) {
            std::cout << "There is an intervention" << std::endl;
            counters->interventions++;
            return false;
        }

        transaction->processRequestBody();
        if (transaction->intervention(intervention)) {
            std::cout << "There is an intervention" << std::endl;
            counters->interventions++;
            return false;
        }

        transaction->addResponseHeader("HTTP/1.1", "200 OK");
        transaction->addResponseHeader("Content-Type", "text/xml; charset=utf-8");
        transaction->addResponseHeader("Content-Length", "200");
        transaction->processResponseHeaders(200, "HTTP 1.2");
        if (transaction->intervention(intervention)) {
            std::cout << "There is an intervention" << std::endl;
            counters->interventions++;
            return false;
        }

        transaction->appendResponseBody(response_body, strlen((const char*)response_body));
        transaction->processResponseBody();
        if (transaction->intervention(intervention)) {
            std::cout << "There is an intervention" << std::endl;
            counters->interventions++;
            return false;
        }

        return true;
    }
};

class RequestOnlyScenario : public ScenarioProvider {
 public:
    const char *name() const override {
        return "request-only";
    }

    bool execute(Transaction *transaction,
        modsecurity::ModSecurityIntervention *intervention,
        BenchmarkCounters *counters) const override {
        transaction->processConnection(kClientIp, 12345, kServerIp, 80);
        if (transaction->intervention(intervention)) {
            std::cout << "There is an intervention" << std::endl;
            counters->interventions++;
            return false;
        }

        transaction->processURI(kRequestUri, "GET", "1.1");
        if (transaction->intervention(intervention)) {
            std::cout << "There is an intervention" << std::endl;
            counters->interventions++;
            return false;
        }

        transaction->addRequestHeader("Host", "net.tutsplus.com");
        transaction->addRequestHeader("User-Agent", "ModSecurity-benchmark/request-only");
        transaction->addRequestHeader("Accept", "*/*");
        transaction->processRequestHeaders();
        if (transaction->intervention(intervention)) {
            std::cout << "There is an intervention" << std::endl;
            counters->interventions++;
            return false;
        }

        transaction->processRequestBody();
        if (transaction->intervention(intervention)) {
            std::cout << "There is an intervention" << std::endl;
            counters->interventions++;
            return false;
        }

        return true;
    }
};

bool parseBenchmarkConfig(int argc, const char *argv[], BenchmarkConfig *config) {
    bool positionalIterationsConsumed = false;
    for (int i = 1; i < argc; i++) {
        const std::string argument(argv[i]);
        if (argument == "-h" || argument == "-?" || argument == "--help") {
            std::cout << help_message << std::endl;
            return false;
        }

        if (argument == "--scenario") {
            if (i + 1 >= argc) {
                std::cerr << "Missing value for --scenario\n" << help_message << std::endl;
                return false;
            }
            config->scenarioName = argv[++i];
            continue;
        }

        if (argument == "--rules-file") {
            if (i + 1 >= argc) {
                std::cerr << "Missing value for --rules-file\n" << help_message << std::endl;
                return false;
            }
            config->rulesFile = argv[++i];
            continue;
        }

        if (!positionalIterationsConsumed) {
            errno = 0;
            unsigned long long upper = strtoull(argv[i], 0, 10);
            if (!errno && upper) {
                config->numRequests = upper;
                positionalIterationsConsumed = true;
                continue;
            }
            if (errno) {
                perror("Invalid number of iterations");
            } else {
                std::cerr << "Failed to convert '" << argv[i]
                          << "' to integer value" << std::endl;
            }
            return false;
        }

        std::cerr << "Unknown argument: " << argument << std::endl
                  << help_message << std::endl;
        return false;
    }
    return true;
}

std::unique_ptr<ScenarioProvider> createScenarioProvider(
    const std::string& scenarioName) {
    if (scenarioName == "legacy-full") {
        return std::unique_ptr<ScenarioProvider>(new LegacyFullScenario());
    }

    if (scenarioName == "request-only") {
        return std::unique_ptr<ScenarioProvider>(new RequestOnlyScenario());
    }

    return nullptr;
}

void printReport(const BenchmarkRunSummary& summary) {
    const long double elapsed_seconds =
        static_cast<long double>(summary.elapsed.count()) / 1000000000.0L;
    const long double avg_tx_ns =
        static_cast<long double>(summary.elapsed.count())
        / static_cast<long double>(summary.config.numRequests);
    const long double tx_per_sec =
        static_cast<long double>(summary.config.numRequests) / elapsed_seconds;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Summary:\n";
    std::cout << "  scenario: " << summary.config.scenarioName << "\n";
    std::cout << "  rules_file: " << summary.config.rulesFile << "\n";
    std::cout << "  elapsed_seconds: " << elapsed_seconds << "\n";
    std::cout << "  avg_transaction_ns: " << avg_tx_ns << "\n";
    std::cout << "  throughput_tx_per_sec: " << tx_per_sec << "\n";
    std::cout << "  interventions: " << summary.counters.interventions << "\n";
}

int main(int argc, const char *argv[]) {
    BenchmarkConfig config;
    if (!parseBenchmarkConfig(argc, argv, &config)) {
        return 0;
    }
    std::unique_ptr<ScenarioProvider> scenario =
        createScenarioProvider(config.scenarioName);
    if (!scenario) {
        std::cerr << "Unknown scenario '" << config.scenarioName << "'." << std::endl;
        std::cerr << "Available scenarios: legacy-full, request-only" << std::endl;
        return -1;
    }

    std::cout << "Doing " << config.numRequests << " transactions...\n";
    std::cout << "Scenario: " << scenario->name() << "\n";
    std::cout << "Rules file: " << config.rulesFile << "\n";
    modsecurity::ModSecurity *modsec = new modsecurity::ModSecurity();
    modsecurity::RulesSet *rules = new modsecurity::RulesSet();
    modsecurity::ModSecurityIntervention it;
    modsecurity::intervention::clean(&it);
    modsec->setConnectorInformation("ModSecurity-benchmark v0.0.1-alpha"
            " (ModSecurity benchmark utility)");

    if (rules->loadFromUri(config.rulesFile.c_str()) < 0) {
        std::cout << "Problems loading the rules..." << std::endl;
        std::cout << rules->m_parserError.str() << std::endl;
        delete rules;
        delete modsec;
        return -1;
    }

    // Start timing after one-time setup to measure only transaction processing.
    const auto benchmark_start = std::chrono::steady_clock::now();
    BenchmarkCounters counters;

    for (unsigned long long i = 0; i < config.numRequests; i++) {
        Transaction *modsecTransaction = new Transaction(modsec, rules, NULL);
        scenario->execute(modsecTransaction, &it, &counters);
        modsecTransaction->processLogging();
        delete modsecTransaction;
        modsecurity::intervention::free(&it);
        modsecurity::intervention::clean(&it);
    }

    BenchmarkRunSummary summary;
    summary.config = config;
    summary.counters = counters;
    summary.elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - benchmark_start);

    delete rules;
    delete modsec;
    printReport(summary);
}
