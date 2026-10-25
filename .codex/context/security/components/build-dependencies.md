# Prüfung von Build, Abhängigkeiten und CI

## Einstiegspunkte im Quellcode

Lies `configure.ac`, die relevanten Makefiles, plattformspezifischen Build-Dateien, `.gitmodules` und ausgewählten Workflow-Definitionen. Die festgelegten Unix- und Windows-Builds setzen C++17. Testwerkzeuge werden durch die configure-Bedingung nur bei `YAJL_FOUND=1` aktiviert. Quellen: [Unix-Standard](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L49-L63), [Windows-Standard](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/win32/CMakeLists.txt#L102-L104), [Bedingung für Testwerkzeuge](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L313-L324).

## Prüffragen

- Welcher Compiler, welche Abhängigkeitsversionen, Funktionsmakros, Verknüpfungsart und tatsächliche Binärdatei gehören zum behaupteten Ergebnis?
- Stimmen generierte Parser-Ausgaben mit ihren `.ll`/`.yy`-Eingaben und Build-Regeln überein?
- Entsprechen Submodul-Checkouts den Gitlink-Commits des Haupt-Repositorys statt einem veränderlichen Remote-Branch?
- Welche Workflow-Eingaben können Shell-Befehle, Pfade, Downloads, Caches, Artefakte oder Zugangsdaten erreichen?
- Wurden die vorgesehenen Tests gebaut, gefunden und mit aussagekräftigen Prüfaussagen ausgeführt?
- Gehören lokale und externe Ergebnisse zu denselben Quellen, derselben Konfiguration und demselben Artefakt?

Ein mit `NO_LOGS` gebauter Regressionsrunner kann `0` zurückgeben, bevor Tests geladen werden. Das Scanner-Dockerfile fängt einen Fehler von `make check` mit einer Warnung ab. Das sind Gründe, Ausführungsnachweise zu prüfen, keine Aussagen darüber, ob ein bestimmter Lauf bestanden wurde oder fehlgeschlagen ist. Quellen: [vorzeitige Runner-Rückgabe](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/regression/regression.cc#L437-L442), [Testbehandlung im Dockerfile](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.oss-scanner/Dockerfile#L29-L37).

## Relevante Prüfungen auswählen

[Lieferkette](../checks/supply-chain.md), [Konfigurations- und Eingabeinjektion](../checks/config-injection.md), [Recherche zu Sicherheitshinweisen](../checks/advisory-research.md), [Offenlegung](../checks/logging-disclosure.md) und [Nachweisintegrität](../checks/evidence-integrity.md).

Nutze die [Build- und Testhinweise](../../build-and-test.md) für tatsächliche Repository-Befehle. Inventarisiere Submodulinhalte und externe Pakete nur, wenn sie zum Umfang gehören; ihre Existenz oder Versionsbindung belegt kein abgeschlossenes Abhängigkeitsaudit.
