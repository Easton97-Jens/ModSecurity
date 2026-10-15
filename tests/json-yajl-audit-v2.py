#!/usr/bin/env python3
"""Exercise the real Apache/ModSecurity v2 JSON audit writer, not a JSON parser.

Requires a built YAJL-enabled mod_security2.so, Apache 2.4 and its module
directory. All payloads are synthetic and remain byte-exact. The output
directory must be new and outside the source tree. Example:

  python3 tests/json-yajl-audit-v2.py --httpd /path/to/apache2 \
    --modules /path/to/apache2/modules --module apache2/.libs/mod_security2.so \
    --output /tmp/modsecurity-v2-audit-run

Exit 0: all assertions pass; 1: assertion failure; 2: execution blocked.
--negative-control deliberately changes one expected body and must exit 1.
"""

import argparse
import collections
import hashlib
import http.client
import json
import os
from pathlib import Path
import socket
import subprocess
import sys
import time


# These are fixed expectations, never regenerated from the writer's output.
# HTTP bodies are deliberately not JSON: reading and writing JSON are separate
# operations, and this suite exercises only the writing side.
CASES = [
    ("W2-A01-quotes-backslash", b'quote="double"; backslash=\\; slash=/', 200),
    ("W2-A02-controls", b"tab=\t; LF=\n; CR=\r; BS=\b; FF=\f; SOH=\x01", 200),
    ("W2-A03-unicode", 'two=\u00e4; three=\u20ac; four=\U0001f600'.encode("utf-8"), 200),
    ("W2-A04-types-empty", b"", 200),
    ("W2-A05-interrupted", b"synthetic denied request", 403),
]
REQUEST_HEADER = b'quote="; backslash=\\; tab=\t; utf8=\xc3\xa4\xe2\x82\xac\xf0\x9f\x98\x80'


def apache_config(opts, out, port):
    modules = Path(opts.modules).resolve()
    lines = [
        f'ServerRoot "{out}"', f'PidFile "{out}/httpd.pid"',
        f'Listen 127.0.0.1:{port}', 'ServerName localhost',
        f'ErrorLog "{out}/error.log"', 'LogLevel warn',
        f'User #{os.getuid()}', f'Group #{os.getgid()}',
    ]
    for name in ("mpm_event", "authn_core", "authz_core", "unixd", "unique_id"):
        module = modules / f"mod_{name}.so"
        if module.exists():
            lines.append(f'LoadModule {name}_module "{module}"')
    lines.extend([
        f'LoadModule security2_module "{Path(opts.module).resolve()}"',
        f'DocumentRoot "{out}/htdocs"', f'<Directory "{out}/htdocs">',
        'Require all granted', 'ForceType text/plain', '</Directory>',
        'SecRuleEngine On', 'SecRequestBodyAccess On',
        'SecResponseBodyAccess On', 'SecResponseBodyMimeType text/plain text/html',
        f'SecTmpDir "{out}"', f'SecDataDir "{out}"', f'SecUploadDir "{out}"',
        f'SecDebugLog "{out}/debug.log"', 'SecDebugLogLevel 9',
        'SecAuditEngine On', 'SecAuditLogType Serial', 'SecAuditLogFormat JSON',
        'SecAuditLogParts ABCEFHZ', f'SecAuditLog "{out}/audit.jsonl"',
        'SecRule REQUEST_URI "@streq /W2-A05-interrupted" '
        '"id:972005,phase:2,t:none,deny,status:403,log,msg:\'audit writer denial\'"',
    ])
    return "\n".join(lines) + "\n"


def load_records(path):
    # Reject non-RFC JSON constants; do not repair malformed UTF-8 or escapes.
    def reject_constant(value):
        raise ValueError("non-JSON numeric constant: " + value)

    return [json.loads(line, parse_constant=reject_constant)
            for line in path.read_bytes().decode("utf-8", "strict").splitlines()
            if line.strip()]


def check_record(record, case, negative_control=False):
    case_id, body, status = case
    failures = []

    def check(condition, description):
        if not condition:
            failures.append(description)

    check(type(record) is dict, "record must be an object")
    if type(record) is not dict:
        return failures
    for key in ("transaction", "request", "response", "audit_data"):
        check(type(record.get(key)) is dict, key + " must be an object")
    if any(type(record.get(key)) is not dict
           for key in ("transaction", "request", "response", "audit_data")):
        return failures
    tx, request, response, audit = (record[key] for key in
                                   ("transaction", "request", "response", "audit_data"))
    # Variable metadata is checked by type/presence, not normalized away.
    for key in ("time", "transaction_id", "remote_address", "local_address"):
        check(type(tx.get(key)) is str and bool(tx[key]), "transaction." + key)
    for key in ("remote_port", "local_port"):
        check(type(tx.get(key)) is int, "transaction." + key + " must be integer")
    check(tx.get("remote_address") == "127.0.0.1", "remote_address mismatch")
    check(tx.get("local_address") == "127.0.0.1", "local_address mismatch")
    check(request.get("request_line") == f"POST /{case_id} HTTP/1.1", "request_line mismatch")
    headers = request.get("headers")
    check(type(headers) is dict, "request.headers must be an object")
    if type(headers) is dict:
        headers = {key.lower(): value for key, value in headers.items()}
        check(headers.get("x-audit-synthetic") == REQUEST_HEADER.decode("utf-8"),
              "request header escaping/Unicode mismatch")
        check(headers.get("content-type") == "text/plain", "request Content-Type mismatch")
        check(headers.get("content-length") == str(len(body)), "byte Content-Length mismatch")
    # v2 stores the logged request body as an array of retrieved body chunks.
    # This bounded request fits in one chunk. With Content-Length: 0 the
    # request was read, but retrieval produces no chunk, so body is [].
    want_body = [body.decode("utf-8")] if body else []
    if negative_control:
        want_body = ["controlled wrong expectation"]
    check(request.get("body") == want_body, "request.body exact decoded array mismatch")
    check(type(response.get("status")) is int and response["status"] == status,
          "response.status integer mismatch")
    check(type(response.get("headers")) is dict, "response.headers must be an object")
    check(type(response.get("protocol")) is str, "response.protocol must be a string")
    if status == 200:
        # The static response uses the same synthetic bytes, with no newline.
        check(response.get("body") == body.decode("utf-8"), "response.body exact decoded string mismatch")
        check(audit.get("response_body_dechunked") is True,
              "response_body_dechunked must be JSON true")
        check("action" not in audit, "successful response must omit action")
    else:
        action = audit.get("action")
        check(type(action) is dict, "audit_data.action must be an object")
        if type(action) is dict:
            check(action.get("intercepted") is True, "intercepted must be JSON true")
            check(type(action.get("phase")) is int and action["phase"] == 2,
                  "interception phase must be integer 2")
        messages = audit.get("messages")
        check(type(messages) is list and bool(messages), "denial messages must be a nonempty array")
        if type(messages) is list:
            check(all(type(message) is str for message in messages), "messages must contain strings")
            check(any("audit writer denial" in message for message in messages), "missing denial message")
    return failures


def run(opts, out):
    out.mkdir(parents=True, exist_ok=False)
    (out / "htdocs").mkdir()
    for case_id, body, _ in CASES:
        (out / "htdocs" / case_id).write_bytes(body)
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        port = sock.getsockname()[1]
    config = out / "httpd.conf"
    config.write_text(apache_config(opts, out, port))
    statuses = []
    with (out / "startup.log").open("wb") as log:
        server = subprocess.Popen([opts.httpd, "-X", "-f", str(config)], stdout=log, stderr=log)
    try:
        for _ in range(100):
            if server.poll() is not None:
                raise RuntimeError("Apache startup failed; inspect startup.log")
            try:
                with socket.create_connection(("127.0.0.1", port), timeout=0.1):
                    break
            except OSError:
                time.sleep(0.05)
        else:
            raise RuntimeError("Apache startup timeout")
        for case_id, body, _ in CASES:
            conn = http.client.HTTPConnection("127.0.0.1", port, timeout=8)
            conn.putrequest("POST", "/" + case_id)
            conn.putheader("Content-Type", "text/plain")
            conn.putheader("Content-Length", str(len(body)))
            conn.putheader("X-Audit-Synthetic", REQUEST_HEADER)
            conn.putheader("Connection", "close")
            conn.endheaders(body)
            response = conn.getresponse()
            statuses.append(response.status)
            response.read()
            conn.close()
        for _ in range(100):
            path = out / "audit.jsonl"
            if path.exists() and len(path.read_bytes().splitlines()) >= len(CASES):
                break
            time.sleep(0.02)
        else:
            raise RuntimeError("audit writer did not emit all records")
        if server.poll() is not None:
            raise RuntimeError(f"Apache exited unexpectedly with code {server.returncode}")
    finally:
        server.terminate()
        try:
            server.wait(timeout=5)
        except subprocess.TimeoutExpired:
            server.kill()
            server.wait()
    try:
        records = load_records(out / "audit.jsonl")
    except (UnicodeError, ValueError) as exc:
        return [dict(id="W2-A00-writer-json-syntax", status="FAIL", failures=[str(exc)])]
    if len(records) != len(CASES):
        return [dict(id="W2-A00-record-count", status="FAIL",
                     failures=[f"expected {len(CASES)} records, got {len(records)}"])]
    if any(type(record) is not dict or type(record.get("request")) is not dict
           for record in records):
        return [dict(id="W2-A00-writer-schema", status="FAIL",
                     failures=["each record and its request must be objects"])]
    # Identify transactions by the stable request line rather than log order.
    by_line = collections.defaultdict(list)
    for record in records:
        by_line[record.get("request", {}).get("request_line")].append(record)
    results = []
    for index, case in enumerate(CASES):
        case_id, body, status = case
        matches = by_line[f"POST /{case_id} HTTP/1.1"]
        failures = []
        if len(matches) != 1:
            failures.append(f"expected one matching record, got {len(matches)}")
        else:
            failures = check_record(matches[0], case, opts.negative_control and index == 0)
        if statuses[index] != status:
            failures.append(f"HTTP expected {status}, got {statuses[index]}")
        results.append(dict(id=case_id, status="FAIL" if failures else "PASS", failures=failures,
                            body_length=len(body), body_hex=body.hex(),
                            body_sha256=hashlib.sha256(body).hexdigest()))
    return results


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--httpd", required=True)
    parser.add_argument("--modules", required=True)
    parser.add_argument("--module", required=True)
    parser.add_argument("--output", required=True)
    parser.add_argument("--negative-control", action="store_true")
    opts = parser.parse_args()
    out = Path(opts.output).resolve()
    source = Path(__file__).resolve().parent.parent
    if out == source or source in out.parents:
        parser.error("--output must be outside the source tree")
    if out.exists():
        parser.error("--output must not already exist; existing results are preserved")
    try:
        results = run(opts, out)
    except (OSError, RuntimeError, http.client.HTTPException) as exc:
        results = [dict(id="W2-A00-execution", status="BLOCKED", failures=[str(exc)])]
    counts = {status: sum(result["status"] == status for result in results)
              for status in ("PASS", "FAIL", "SKIP", "BLOCKED", "XFAIL")}
    report = dict(suite="v2-json-audit-writer", counts=counts,
                  negative_control=opts.negative_control, cases=results)
    if out.exists():
        (out / "results.json").write_text(json.dumps(report, indent=2) + "\n")
    for result in results:
        print(result["status"], result["id"], "; ".join(result["failures"]), sep=" | ")
    print(json.dumps(counts))
    return 2 if counts["BLOCKED"] else 1 if counts["FAIL"] else 0


if __name__ == "__main__":
    sys.exit(main())
