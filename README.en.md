# ModSecurity: JSON tests and complete documentation

This English-only branch in [Easton97-Jens/ModSecurity](https://github.com/Easton97-Jens/ModSecurity/tree/test/json-yajl-english-only-20261015) contains the complete JSON behavior tests and English documentation. It includes the previous baseline, additional tests inspired by the jsonbench PRs and both ModSecurity comments, and the complete historical reports in English. Build the pinned official ModSecurity engine in a separate checkout using the commands below.

Clone this branch into a new directory:

```sh
git clone --single-branch --no-tags --branch test/json-yajl-english-only-20261015 https://github.com/Easton97-Jens/ModSecurity.git modsecurity-json-tests-en
cd modsecurity-json-tests-en
```

Start with the [new results and reproduction guide](doc/json-pr-extension/README.en.md), the [coverage matrix](doc/json-pr-extension/matrix.en.md), or [How ModSecurity processes a JSON request](doc/en/json-yajl-code-analysis.md). The complete historical reports and original project README are included in English.

Tested production base: [`8271c9d3bc0abcbd1189f5414c196c336c959047`](https://github.com/owasp-modsecurity/ModSecurity/commit/8271c9d3bc0abcbd1189f5414c196c336c959047). The engine and YAJL remain unchanged. This distribution contains tests and documentation; building requires a separate complete ModSecurity checkout.

Fresh characterization runs give **399 PASS**. Separate requirements give **6 PASS / 20 FAIL**, retaining unmet assertions as ordinary failures. Some are explicitly undecided policy proposals. No XFAIL hides them. The [machine-readable results](doc/json-pr-extension/results-summary.json) explain scope and repeated executions.

```sh
python3 test/json-baseline/tools/prepare_source.py --destination ../modsecurity-original --build
python3 test/json-baseline/tools/build.py --source ../modsecurity-original --output ../json-probes
python3 test/json-baseline/tools/run_all.py --probes ../json-probes --output ../json-results --profile characterization
python3 test/json-baseline/tools/run_all.py --probes ../json-probes --output ../json-requirements --profile requirements
```

The last command returns exit 1 on the examined original. See the linked guide for dependencies, build details, status classes and unchanged historical reports. The CI template runs the profiles separately; its requirements job accordingly remains red.

Test ideas were implemented independently; GPL adapter code from jsonbench was not copied. Original documentation and license notices are retained. License: [Apache-2.0](LICENSE).
