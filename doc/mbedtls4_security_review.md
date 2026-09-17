# OWASP ModSecurity v3 – Deep Security Review (Crypto/TLS, libcurl, Mbed TLS 4.x)

Date: 2026-04-02

## A) Executive Summary

### Gesamtbewertung
ModSecurity v3 hat in den untersuchten Pfaden **keine triviale RCE-Klasse**, aber es gibt mehrere **reale security-relevante Schwachstellen bzw. riskante Defaults**:

1. **SecRemoteRules kann bei Download-Fehlern effektiv fail-open laufen**, wenn `SecRemoteRulesFailAction` nicht explizit auf `Abort` gesetzt ist.
2. **MD5/SHA-1 sind aktiv in Laufzeitpfaden**, teils nur Legacy/Kompatibilität, teils aber weiterhin als Integritäts-/Indexwert genutzt.
3. **Nicht-kryptographische Zufallsquellen (`rand()/srand`)** werden für Boundary/ID-nahe Werte genutzt (vorhersagbar).

Die geplante Mbed TLS 4.x-Migration ist buildseitig schon vorbereitet (TF-PSA-Crypto Layout), wird aber derzeit primär für Kompatibilität genutzt, nicht für konsequente Security-Modernisierung.

### Top 3 echte Risiken
1. Remote Rules Fail-Open (konfigurationsabhängig, aber praktisch ausnutzbar in realen Störszenarien).
2. Legacy-Hashes in aktivem Produktpfad (MD5/SHA-1) bleiben ohne klare Policy aktiv.
3. Schwache RNG-Nutzung in Token-/Boundary-Nahbereich.

### Top 3 wichtigste Fixes
1. `SecRemoteRulesFailAction` Default auf `Abort` (oder PropertyNotSet wie Abort behandeln).
2. SHA-256-first: neue sichere Transformation/Indexoption + Legacy klar markieren.
3. `rand()/srand` in Boundary/ID-Pfaden ersetzen (OS RNG/CSPRNG).

---

## B) Verifizierte Schwachstellen

## 1. SecRemoteRules – verifiziertes Fail-Open-Verhalten

### Codepfad
1. `RulesSet::loadFromUri()` ruft `Driver::parseFile()` auf. Fehler nur, wenn Parser fehl schlägt (`parseFile()==0`).
2. Scanner-Regel für `SecRemoteRules` ruft `HttpsClient::download(url)`.
3. Bei `ret == false` wird **nur dann** abgebrochen, wenn `m_remoteRulesActionOnFailed == Abort...`.
4. Für `PropertyNotSetRemoteRulesAction` gibt es **keinen Abort** und auch keinen implementierten Warn-Log-Pfad (TODO).
5. Danach läuft `yy_scan_string(c.content.c_str())`; bei leerem Content wird Parsing fortgesetzt.

### Evidenz
- Default: `m_remoteRulesActionOnFailed(PropertyNotSetRemoteRulesAction)` in `RulesSetProperties` ctor.
- Branching in Scanner: Abort nur bei explizitem `Abort...`.
- Parse-Rückgabe in Driver: Erfolg, wenn Parser syntaktisch durchläuft.

### Warum unsicher
Wenn ein Deployment auf Remote Rules vertraut und der Download fehlschlägt (DNS/Netz/Timeout/TLS-Handshake), werden Regeln ggf. nicht geladen, ohne harten Fail-stop.

### Realistische Ausnutzung
- **DNS Manipulation / Resolver-Ausfall / egress blockiert / künstliche Timeouts** → Remote Rule Fetch schlägt fehl.
- Ergebnis: Instanz läuft ggf. ohne erwartete Regelbasis weiter (Policy-Lücke).

### Einstufung
**Hoch (konfigurationsabhängig, aber real).**

---

## 2. MD5/SHA-1 in aktiven Pfaden

### Fundstellen und Einordnung
1. **Transformationen `t:md5`, `t:sha1`**
   - Zweck: regelbasierte Transformation.
   - Risiko: Kollisionsresistenz schwach; Angreifer kann Input oft kontrollieren.
   - Einstufung: **Mittel (Legacy-Kompatibilität, aber veraltet)**.

2. **Audit-Indexformat `md5:<hash>`**
   - Zweck: altes Audit-Indexformat (`toOldAuditLogFormatIndex`).
   - Risiko: primär Integritäts-/Referenzcharakter; kein direkter Auth-Mechanismus.
   - Einstufung: **Mittel (technischer Debt mit sicherheitsrelevantem Beigeschmack)**.

3. **UniqueId via SHA-1(MAC+Hostname)**
   - Zweck: stabiler Host-Identifier.
   - Risiko: kein Signatur-/Auth-Kontext; Kollision praktisch weniger kritisch.
   - Einstufung: **Niedrig bis Mittel (eher Debt)**.

### Urteil
- **Nicht alles ist „kritische Schwachstelle“**.
- Aber: aktive Verfügbarkeit schwacher Hashes ohne harte Leitplanken ist für Security-Produktionsumgebungen nicht mehr zeitgemäß.

---

## 3. RNG (`rand`/`srand`) in sicherheitsnahen Pfaden

### Fundstellen
- Globales Seeding: `srand(time(NULL))` im `ModSecurity`-Konstruktor.
- Boundary-Generierung nutzt `rand() % ...`.

### Risiko
- Vorhersagbarkeit bei bekannter Startzeit und PRNG-Zustand möglich.
- Für Audit-Boundary eher Integritäts-/Robustheits- als direkter Kryptobruch.

### Einstufung
**Mittel (Hardening mit realem Missbrauchspotenzial).**

---

## 4. `assert()` in Crypto-relevantem Digestpfad

### Fundstellen
- `assert(mdInfo != nullptr)` und `assert(ret == 0)` in `DigestImpl`.

### Risikoanalyse
- In Release (`NDEBUG`) entfallen asserts.
- Fehlerfälle werden dann nicht kontrolliert behandelt; definierte Fehlerpropagation fehlt.
- Das ist eher Robustheits-/Sicherheits-Härtung als unmittelbare exploitable Memory-Corruption.

### Einstufung
**Mittel (sicherheitsrelevante Robustheitslücke).**

---

## 5. TLS/libcurl Integration

### Positiv verifiziert
- TLS minimum explizit auf 1.2 gesetzt.
- Peer- und Host-Verification gesetzt.

### Einschränkungen
- `CURLOPT_SSL_VERIFYHOST` ist auf `1` gesetzt (historisch uneinheitlich; explizit `2L` ist der robuste Wert).
- Kein explizites Pinning-/Trust-Override-Konzept.
- Build-Makro `WITH_CURL_SSLVERSION_TLSv1_2` wird zwar in `build/curl.m4` gesetzt, im Code aber nicht genutzt (Kompatibilitäts-/Wartungsrisiko).

### Einstufung
**Niedrig bis Mittel** (kein offensichtlicher MITM-default, aber Härtung sinnvoll).

---

## C) False Positives / Entwarnung

1. **„ModSecurity implementiert seinen eigenen TLS-Stack“** → Nein. In den geprüften Pfaden erfolgt TLS über libcurl für Remote Rules.
2. **„MD5/SHA-1 überall sofort kritisch“** → Nein. Mehrere Verwendungen sind Legacy-/Format-/ID-bezogen, nicht direkt Signatur-/Auth-Kontrolle.
3. **„Mbed TLS 4.x komplett unvorbereitet“** → Nein. Buildsystem nutzt bereits TF-PSA-Crypto-Pfade.

---

## D) Technischer Debt vs. echte Risiken

### Echte Risiken
- SecRemoteRules Fail-Open bei nicht gesetzter FailAction.
- Vorhersagbare RNG in Boundary/ID-nahen Werten (missbrauchbar je nach Integrationskontext).

### Technischer Debt (mit Security-Relevanz)
- MD5/SHA-1 weiterhin aktiv ohne moderne Default-Policy.
- assert-basierte Digest-Fehlerbehandlung.
- curl-Optionen nicht maximal explizit gehärtet.

---

## E) Konkrete Verbesserungen (priorisiert)

### Kurzfristig (must-fix)
1. **Fail-Closed für SecRemoteRules**
   - `PropertyNotSetRemoteRulesAction` wie `Abort` behandeln.
2. **`CURLOPT_SSL_VERIFYHOST` auf `2L` setzen**.
3. **Digest-asserts ersetzen** durch kontrollierte Fehlerpfade (Return-Fehler/Exception mit sauberer Behandlung).

### Mittelfristig
1. SHA-256 Transformation einführen; MD5/SHA-1 als legacy kennzeichnen.
2. Audit-Index zusätzlich mit SHA-256 ausgeben (parallel zum alten Format zur Kompatibilität).
3. RNG-Ablösung (`rand/srand`) durch CSPRNG/OS RNG.

### Optional
1. Optionales Certificate Pinning für SecRemoteRules.
2. Sicherheits-Baseline zwischen Autotools und Win32 CMake harmonisieren.

---

## F) Patch-Level Vorschläge

## 1. SecRemoteRules fail-closed
- Datei: `src/parser/seclang-scanner.ll`
- Änderung:
  - bei `ret == false` zusätzlich auf `PropertyNotSetRemoteRulesAction` prüfen und aborten.

## 2. Digest wrapper runtime-sicher
- Datei: `src/utils/sha1.h`
- Änderung:
  - `assert` entfernen, stattdessen:
    - `if (mdInfo == nullptr) { ... }`
    - `if (ret != 0) { ... }`
  - klaren Fehlervertrag definieren.

## 3. RNG-Härtung
- Dateien: `src/modsecurity.cc`, `src/audit_log/writer/writer.cc`
- Änderung:
  - `srand/rand` entfernen.
  - Ersatz über CSPRNG-nahe Quelle (z. B. `std::random_device` + starke Engine / OS-RNG Wrapper).

## 4. TLS-Härtung
- Datei: `src/utils/https_client.cc`
- Änderung:
  - `CURLOPT_SSL_VERIFYHOST` => `2L`.
  - optional: Zeitouts (`CONNECTTIMEOUT`, `TIMEOUT`) und restriktive Protokoll-/Redirect-Policy.

---

## G) Offene Unsicherheiten / was zusätzlich zu testen wäre

1. Ob `SecRemoteRules` in produktiven Connector-Deployments standardmäßig genutzt wird (Impact variiert stark).
2. Wie externe Parser/SIEM auf das alte `md5:`-Auditindex-Format angewiesen sind.
3. Ob es Deployments mit sehr alter libcurl gibt, bei denen TLSv1.2-Setzen Build-/Runtime-Sonderfälle auslöst.
4. Ob organisatorisch FIPS/PSA-only Vorgaben existieren (würde Priorisierung der Hash-Migration erhöhen).

