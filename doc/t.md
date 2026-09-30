1. Konkrete YAJL-Verwendungsstellen

    configure.ac: führt YAJL-Konfiguration über PROG_YAJL aus und setzt ein AM_CONDITIONAL dafür.

    build/yajl.m4: definiert PROG_YAJL, prüft Header/Lib (yajl/yajl_parse.h) und setzt YAJL-Flags (YAJL_CFLAGS, YAJL_LDADD).

    src/request_body_processor/json.h: kompiliert den JSON-Processor nur unter #ifdef WITH_YAJL, inkludiert <yajl/yajl_parse.h>, nutzt YAJL-Typen (yajl_handle, yajl_status).

    src/request_body_processor/json.cc: implementiert JSON-Parsing mit YAJL (yajl_alloc, yajl_parse, yajl_complete_parse, Error-Handling über YAJL-APIs).

    headers/modsecurity/transaction.h: nutzt YAJL-Generatorfunktionen in Makros (yajl_gen_string, yajl_gen_number, yajl_gen_integer).

    src/Makefile.am: bindet request_body_processor/json.cc in BODY_PROCESSORS ein und nutzt $(YAJL_CFLAGS) beim Build der Hauptbibliothek libmodsecurity.la.

    build/win32/CMakeLists.txt: setzt HAVE_YAJL auf 1 und verknüpft libModSecurity sowie Tests mit yajl::yajl (WITH_YAJL).

    build/win32/conanfile.txt: enthält YAJL als Conan-Abhängigkeit (yajl/2.1.0).

    README.md: beschreibt YAJL als (laut Dokumentation) mandatory dependency für JSON-Logs und Testframework.

2. Technische Einordnung

    Die YAJL-Nutzung liegt im nativen C/C++-Kernpfad des Projekts (Autotools/CMake, Kernbibliothek libmodsecurity, Request-Body-JSON-Parser, Transaktions-Logging-Makros).

    Ein klar belegter „austauschbarer Java-Randbereich“ für diese YAJL-Stellen ist im Repository nicht belegt.

    Dass YAJL nur in einem leicht isolierbaren Nebenmodul liegt, ist durch die gezeigten Build- und Source-Verknüpfungen nicht belegt.

3. Voraussetzungen für einen Ersatz durch Jackson

Für einen echten Ersatz YAJL → Jackson wären zusätzlich nötig:

    Eine Integrationsschicht zwischen C/C++-Kern und Java (z. B. JNI, IPC oder andere Brücke), weil Jackson eine Java-Bibliothek ist und YAJL hier nativ in C/C++ verwendet wird.

        Ein solcher Mechanismus ist im Repository nicht belegt (kein vorhandener JNI/JNA-/Java-Buildpfad im Repo nachweisbar).

    Ein definierter Java-Build/Dependency-Pfad (z. B. Maven-Modul) für Jackson.

        Im Repository sind entsprechende Java-/Maven-Dateien nicht belegt (Dateisuche ohne Treffer).

    Anpassungen der nativen Aufrufstellen, die heute direkt YAJL-APIs verwenden (json.cc, transaction.h).

4. Belastbare Antwort auf die Kernfrage

YAJL könnte eventuell ersetzbar sein, aber nur mit zusätzlicher Integrationsschicht

Begründung (nur Repository-Evidenz):

    YAJL ist direkt im nativen Kern und in den nativen Buildpfaden verdrahtet (Autotools/CMake/Conan + C/C++-Code).

    Konkrete YAJL-API-Nutzung im Kerncode ist vorhanden (yajl_parse, yajl_gen_*).

    Ein vorhandener Java-Bridge-Mechanismus, der „kleinen Ersatzaufwand“ stützen würde, ist im Repository nicht belegt.

5. Was dafür fehlen würde

    Nachweisbarer bestehender Java/JVM-Integrationspfad im Repository: im Repository nicht belegt.

    Nachweisbare vorhandene JNI/JNA/IPC-Brücke vom C/C++-Kern zu Java: im Repository nicht belegt.

    Nachweisbarer Maven-/Java-Modulpfad im Repository: im Repository nicht belegt.

    Nachweis, dass die YAJL-Aufrufe in json.cc/transaction.h ohne wesentliche Kernanpassung austauschbar sind: im Repository nicht belegt.

Explizite Antwort auf die Zusatzfrage:
„Ist Jackson hier ein echter Ersatz für YAJL oder nur ein zusätzlicher Java-Baustein, der eine neue Integrationsschicht erfordern würde?“
→ Auf Basis des Repositorys ist Jackson nicht als direkter kleiner Ersatz von YAJL belegt; belastbar erscheint Jackson hier nur als zusätzlicher Java-Baustein mit neuer Integrationsschicht.
