# ModSecurity: JSON-Tests und vollständige Dokumentation

[English](README.en.md) · [Deutsche Fassung](README.de.md)

Dieses eigenständige Test-Repository prüft die tatsächliche YAJL-Verarbeitung des offiziellen ModSecurity v3. Es enthält die vollständige vorherige Bestandsaufnahme, neue Tests aus den jsonbench-PRs und beiden ModSecurity-Kommentaren sowie **alle erklärenden Dokumente auf Deutsch und Englisch**. Das deutsche Repository enthält beide Sprachfassungen; das separate englische Repository enthält ausschließlich die vollständige englische Dokumentation. Die ausführbaren Tests sind in beiden identisch.

Beginne mit der [neuen Ergebnisübersicht und Reproduktionsanleitung](doc/json-pr-extension/README.de.md), der [Testmatrix](doc/json-pr-extension/matrix.de.md) oder [So verarbeitet ModSecurity einen JSON-Request](doc/json-yajl-code-analysis.md). Die [vollständige englische Dokumentation](doc/json-pr-extension/README.en.md#complete-documentation-in-both-languages) ist ebenfalls enthalten.

Getestete Produktionsbasis: [`8271c9d3bc0abcbd1189f5414c196c336c959047`](https://github.com/owasp-modsecurity/ModSecurity/commit/8271c9d3bc0abcbd1189f5414c196c336c959047). Die Engine und YAJL werden nicht verändert. Diese Distribution enthält Tests und Dokumentation; ein vollständiger, gesonderter ModSecurity-Checkout ist zum Bauen erforderlich.

Die frischen Charakterisierungsläufe ergeben **399 PASS**. Die getrennten Anforderungen ergeben **6 PASS / 20 FAIL**; ihre Fehlschläge bleiben sichtbar. Darunter sind ausdrücklich noch nicht beschlossene Policy-Vorschläge. Kein XFAIL verdeckt sie. Die [maschinenlesbaren Ergebnisse](doc/json-pr-extension/results-summary.json) erklären Umfang und Wiederholungen.

```sh
python3 test/json-baseline/tools/prepare_source.py --destination ../modsecurity-original --build
python3 test/json-baseline/tools/build.py --source ../modsecurity-original --output ../json-probes
python3 test/json-baseline/tools/run_all.py --probes ../json-probes --output ../json-results --profile characterization
python3 test/json-baseline/tools/run_all.py --probes ../json-probes --output ../json-requirements --profile requirements
```

Der letzte Befehl endet beim untersuchten Stand mit Exit 1. Voraussetzungen, Builddetails, Statusklassen und unveränderte historische Berichte stehen in der verlinkten Anleitung. Der CI-Entwurf führt die beiden Profile separat aus; sein Anforderungsjob bleibt entsprechend rot.

Die Testideen wurden unabhängig implementiert; GPL-Adaptercode aus jsonbench wurde nicht übernommen. Originaldokumentation und Lizenzhinweise bleiben erhalten. Lizenz: [Apache-2.0](LICENSE).
