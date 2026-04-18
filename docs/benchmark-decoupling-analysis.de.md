# Analyse: Entkopplung des `benchmark`-Programms von einer fest verdrahteten Testumgebung

## Aktueller Aufbau

Der aktuelle Performance-Benchmark ist ein eigenes C++-Programm in `test/benchmark/benchmark.cc`.

- Iterationen: standardmäßig `1000000`, optional als erstes positionsbasiertes CLI-Argument (`benchmark 1000`).
- Rule-Quelle: fest auf `basic_rules.conf` verdrahtet.
- Request-/Response-Daten: fest im Quellcode hinterlegt (URI, Header, Body, Methode, HTTP-Version, Ports, Response-Code).
- Ablauf: pro Iteration wird eine neue `Transaction` erzeugt und durch die typischen Phasen geführt (`processConnection`, `processURI`, Header/Body, Response, Logging).

Zusätzlich ist in der Dokumentation beschrieben, dass `basic_rules.conf` als minimaler Default dient und per Hilfsskript mit CRS-Includes erweitert wird.

## Stellen mit harter Kopplung

### 1) Feste Rule-Datei
`benchmark.cc` lädt ausschließlich `basic_rules.conf`.

Folge: Ohne Dateiänderung oder C++-Änderung kann kein alternativer Rule-Einstiegspunkt gewählt werden.

### 2) Feste Testtransaktion im Code
Die Transaktion ist komplett als statisches Szenario kodiert:

- URI ist fest,
- Methode/Version sind fest,
- alle Request-Header sind fest,
- Response-Header und XML-Response-Body sind fest.

Folge: Der Benchmark misst immer genau diesen einen synthetischen Ablauf.

### 3) Nur positionsbasiertes, minimales CLI
Der aktuelle Parser akzeptiert nur `num_iterations` als erstes Argument oder Help.

Folge: Es gibt keine parametrierbare Auswahl von Rulesets, Input-Daten oder Test-Suiten.

### 4) Rule-Konfiguration über Dateimutation
Die Skripte `download-owasp-v3-rules.sh` / `download-owasp-v4-rules.sh` hängen `Include`-Zeilen an **dieselbe** `basic_rules.conf` an.

Folge: Die Testumgebung wird durch Dateiänderung umgeschaltet, nicht per Laufzeitparameter.

## Risiken / Unsicherheiten

- Unklar ist, ob externe Nutzer aktuell von der bestehenden, positionsbasierten CLI-Auslegung abhängen. **Das kann ich aus dem vorhandenen Code nicht sicher belegen.**
- Unklar ist, ob es intern Anforderungen gibt, exakt dieses Legacy-Szenario als unveränderlichen Vergleichswert zu behalten. **Das kann ich aus dem vorhandenen Code nicht sicher belegen.**
- Unklar ist, ob weitere Benchmark-Automation außerhalb des Repos die Datei `basic_rules.conf` direkt voraussetzt. **Das kann ich aus dem vorhandenen Code nicht sicher belegen.**

## Vorschlag zur Entkopplung

Ziel: Benchmark-Engine bleibt gleich, aber Testfall und Rule-Quelle werden injizierbar.

### A) CLI auf benannte Optionen erweitern (abwärtskompatibel)
Neu z. B.:

- `--iterations N` (zusätzlich zur alten positionsbasierten Form)
- `--rules PATH`
- `--suite PATH` (Datei mit Szenario-Beschreibung)
- optional `--case NAME` (ein Testfall aus einer Suite)

Abwärtskompatibilität:

- Wenn nur `benchmark 1000` aufgerufen wird, Verhalten unverändert.
- Defaults bleiben kompatibel (`rules=basic_rules.conf`, Legacy-Case).

### B) Datenmodell für Benchmark-Fall einführen
Ein `BenchmarkCase`-Struct kapselt alle aktuell harten Werte (Connection, Request, Response, optional Bodies).

Effekt:

- Die Schleife misst weiterhin denselben Ablauf,
- aber die Daten kommen aus einem Objekt statt aus Hardcoded-Strings.

### C) Loader-Schicht für Testquelle
Start mit minimalem Loader:

1. `--suite` nicht gesetzt → built-in Legacy-Case (heutiges Verhalten).
2. `--suite` gesetzt → einfache Parser-Funktion lädt `BenchmarkCase` aus Datei (z. B. key/value oder JSON).

Wichtig: Für einen minimal-invasiven ersten Schritt reicht **ein** externer Case pro Datei.

### D) Ausführungslogik in Funktion kapseln
`runTransactionCase(...)` führt exakt den bisherigen Phasenablauf aus, bekommt aber `BenchmarkCase` übergeben.

Damit bleibt Messlogik stabil, nur Datenquelle wird variabel.

## Konkrete Codeänderungen (minimal-invasiv)

1. **`test/benchmark/benchmark.cc`**
   - `Options`-Struct + `parseOptions()` ergänzen.
   - `BenchmarkCase`-Struct einführen.
   - `buildLegacyCase()` aus bisherigen Hardcoded-Werten erstellen.
   - `loadCaseFromSuiteFile(path)` hinzufügen (initial einfach).
   - `runTransactionCase(Transaction*, const BenchmarkCase&, ...)` extrahieren.
   - Rule-Datei über `options.rules_path` laden statt über feste Konstante.

2. **`test/benchmark/Makefile.am`**
   - Keine zwingende Änderung für die Entkopplung selbst.
   - Optional: Beispiel-Suite-Datei in `EXTRA_DIST` aufnehmen.

3. **`test/benchmark/`**
   - Neue Datei, z. B. `sample_suite.json` (ein alternativer Testfall).

4. **Dokumentation (`README.md` + ggf. `docs/benchmark-tests.*.md`)**
   - Neue CLI-Optionen und Beispiele dokumentieren.

## Beispielcode für die Entkopplung

> Hinweis: Das ist ein Vorschlagscode (Design-Skizze), kein bereits integrierter Stand.

```c++
struct Header {
    std::string name;
    std::string value;
};

struct BenchmarkCase {
    std::string name{"legacy-default"};

    std::string client_ip{"198.51.100.10"};
    int client_port{12345};
    std::string server_ip{"198.51.100.20"};
    int server_port{80};

    std::string uri{"/test.pl?param1=test&para2=test2"};
    std::string method{"GET"};
    std::string http_version{"1.1"};

    std::vector<Header> request_headers;
    std::vector<Header> response_headers;
    std::string request_body;
    std::string response_body;

    int response_status{200};
    std::string response_protocol{"HTTP 1.2"};
};

struct Options {
    unsigned long long iterations{1000000};
    std::string rules_path{"basic_rules.conf"};
    std::string suite_path;   // optional
    std::string case_name;    // optional
};

int runTransactionCase(modsecurity::ModSecurity *modsec,
                       modsecurity::RulesSet *rules,
                       const BenchmarkCase &tc,
                       modsecurity::ModSecurityIntervention *it) {
    modsecurity::Transaction tx(modsec, rules, nullptr);

    tx.processConnection(tc.client_ip.c_str(), tc.client_port,
                         tc.server_ip.c_str(), tc.server_port);
    if (tx.intervention(it)) return 1;

    tx.processURI(tc.uri.c_str(), tc.method.c_str(), tc.http_version.c_str());
    if (tx.intervention(it)) return 1;

    for (const auto &h : tc.request_headers) {
        tx.addRequestHeader(h.name.c_str(), h.value.c_str());
    }
    tx.processRequestHeaders();
    if (tx.intervention(it)) return 1;

    if (!tc.request_body.empty()) {
        tx.appendRequestBody(
            reinterpret_cast<const unsigned char *>(tc.request_body.data()),
            tc.request_body.size());
    }
    tx.processRequestBody();
    if (tx.intervention(it)) return 1;

    for (const auto &h : tc.response_headers) {
        tx.addResponseHeader(h.name.c_str(), h.value.c_str());
    }
    tx.processResponseHeaders(tc.response_status, tc.response_protocol.c_str());
    if (tx.intervention(it)) return 1;

    if (!tc.response_body.empty()) {
        tx.appendResponseBody(
            reinterpret_cast<const unsigned char *>(tc.response_body.data()),
            tc.response_body.size());
    }
    tx.processResponseBody();
    if (tx.intervention(it)) return 1;

    tx.processLogging();
    return 0;
}
```

## Beispiel für Nutzung mit einem alternativen Test

Mit vorgeschlagener CLI:

```bash
./benchmark --iterations 50000 \
  --rules ./my_ruleset.conf \
  --suite ./sample_suite.json \
  --case post_json_heavy
```

Dieser Aufruf würde denselben Benchmark-Loop verwenden, aber Testdaten und Ruleset zur Laufzeit injizieren.

## Fazit

Ja, die Entkopplung ist technisch sinnvoll und mit überschaubarem Risiko möglich, **wenn** die Legacy-Defaults als Fallback erhalten bleiben.

Der Kernpunkt ist: Messlogik und Testdatenquelle sauber trennen, ohne den bisherigen Standardlauf zu entfernen.
