# ModSecurity v3 – Auswahl kryptografischer Primitive (SHA-256 vs HMAC vs SHA-512 vs Verschlüsselung)

Datum: 2026-04-02

## 1) Bestandsaufnahme (verifizierte Fundstellen)

### A. Regel-Transformationen (angreiferkontrollierter Input möglich)
- `t:md5` und `t:sha1` sind aktiv registriert und ausführbar.
- Dateien:
  - `src/actions/transformations/transformation.cc`
  - `src/actions/transformations/md5.cc`
  - `src/actions/transformations/sha1.cc`
  - Parser-Tokens: `src/parser/seclang-scanner.ll`
- Zweck: Regel-Transformation / Datennormalisierung, **kein** verschlüsselter Speicher und **keine** Authentizitätsgarantie.

### B. Digest-Wrapper
- Datei: `src/utils/sha1.h` + `src/utils/md5.h`.
- Zweck: allgemeine Digest-Helfer für unkeyed Hashes.

### C. Audit-Index (Legacy-Format)
- Datei: `src/audit_log/writer/parallel.cc` und `src/transaction.cc` (`md5:<hash>`).
- Zweck: altes Indexformat/Referenz, stark formatgebunden.

### D. Machine Unique ID
- Datei: `src/unique_id.cc`.
- Zweck: stabiler technischer Identifier aus Hostmerkmalen (`SHA-1(mac+hostname)`), **kein** Auth-Token.

### E. TLS/Remote Rules
- Datei: `src/utils/https_client.cc`.
- Zweck: Transportschutz via TLS für Remote Rules Download.
- In den geprüften Pfaden gibt es keinen lokalen HMAC- oder Verschlüsselungs-Workflow für Regelinhalte.

## 2) Sicherheitsziel je Fundstelle

- **Transformationen (`t:md5`, `t:sha1`)**
  - Ziel: unkeyed Digest-Transformation (Kategorie 1) + Legacy-Kompatibilität (Kategorie 4).
- **Digest-Wrapper**
  - Ziel: unkeyed Digest (Kategorie 1).
- **Audit-Index**
  - Ziel: Formatkompatibilität / Referenz (Kategorie 4), nicht Authentizität.
- **Unique-ID**
  - Ziel: technischer Identifier (Kategorie 5), nicht kryptografische Authentizität.
- **TLS Remote Rules**
  - Ziel: Transportintegrität und Vertraulichkeit via TLS, nicht Payload-HMAC im lokalen Codepfad.

## 3) Bewertung alternativer Verfahren

### SHA-256
**Sinnvoll und ausreichend** für alle Stellen, die heute unkeyed MD5/SHA-1 als Digest verwenden und nicht formatgebunden sind.

### HMAC-SHA-256
**Nur sinnvoll, wenn keyed Authentizität fachlich gefordert ist.**
In den geprüften Pfaden wird aktuell kein lokaler geheimer Schlüssel für Hash-Verifikation verwendet; daher ist HMAC **nicht** der direkte Ersatz für bestehende unkeyed Transformations-/Indexpfade.

### SHA-512 / SHA-384
Kein belegbarer Zusatznutzen in den aktuellen ModSecurity-v3-Pfaden:
- keine gezeigten Anforderungen, die SHA-256 übersteigen,
- höhere Kosten/Komplexität ohne klares Sicherheitsziel.

### Echte Verschlüsselung (z. B. AES-GCM/ChaCha20-Poly1305)
In den geprüften MD5/SHA-1-Stellen ist **kein Vertraulichkeitsziel** erkennbar.
Daher ist „Hashing durch Verschlüsselung ersetzen“ fachlich falsch.
Transport-Vertraulichkeit für Remote Rules wird bereits über TLS adressiert.

## 4) Antwort auf die Hauptfrage

### Wo SHA-256 allein die richtige Wahl ist
- Neue unkeyed Digest-Pfade als Ersatz für MD5/SHA-1 (insbesondere künftige nicht-legacy Erweiterungen).

### Wo HMAC-SHA-256 besser wäre
- Nur bei zukünftigen Features mit expliziter keyed Authentizität (z. B. signierte/geschützte Remote-Policy-Protokolle).
- Für die aktuell gefundenen Pfade **nicht** als Drop-in-Ersatz belegt.

### Wo SHA-512/SHA-384 keinen Zusatznutzen bringt
- Bei Transformationen, Audit-Index-Referenzen und technischen Identifiern im aktuellen Design.

### Wo Verschlüsselung relevant wäre
- Nur wenn Anforderungen an Vertraulichkeit der gespeicherten/transportierten Daten über TLS hinaus gefordert werden; in den untersuchten Hash-Pfaden nicht ersichtlich.

### Wo Legacy unverändert bleiben sollte
- `t:md5`, `t:sha1` und altes Audit-Indexformat nicht still brechen; nur additiv migrieren.

## 5) Konkrete, nicht-brechende nächste Schritte

1. **SHA-256-first additiv einführen** (z. B. neue `t:sha256` Transformation).
2. **MD5/SHA-1 als Legacy markieren** (Doku + Hinweise), nicht sofort entfernen.
3. Audit-Index mittelfristig additiv um SHA-256 erweitern, Legacy-`md5:` zunächst behalten.
4. HMAC-SHA-256 nur bei neuem, klar spezifiziertem keyed-Authentizitätsbedarf einführen.

## 6) Offene Punkte

- Ob externe Parser zwingend das exakte `md5:`-Format benötigen, ist ohne Integrationsinventar nicht vollständig belegbar.
- Ob ein zukünftiges Remote-Rules-Protokoll Payload-Authentizität (zusätzlich zu TLS) fordert, ist eine Produktentscheidung außerhalb der aktuellen Codepfade.
