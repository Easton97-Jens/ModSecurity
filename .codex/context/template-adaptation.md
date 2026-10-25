# Anpassung des Kontextaufbaus aus dem Connector-Repository

## Gewünschte Vorlage und untersuchter Stand

Der Eigentümer wählte die `.codex`-Struktur von
[`Easton97-Jens/ModSecurity-conector`](https://github.com/Easton97-Jens/ModSecurity-conector/tree/42fd5a8e895aca3a36b09eb09fe5c7cf9c3399e3/.codex),
Branch `master`, untersucht am Commit `42fd5a8e895aca3a36b09eb09fe5c7cf9c3399e3`
am 25.10.2026. Die versionierten `.codex`-Dateien, `AGENTS.md`, `README.md`,
`.gitignore` und die Build-Steuerung im Hauptverzeichnis wurden hinsichtlich Struktur
und Repository-spezifischer Annahmen untersucht. Das
[Referenzinventar](../audit/template-source-index.tsv) hält die ausgewählten Git-Objekte fest.
Dies behauptet keine Prüfung des gesamten Produktcodes des Connectors.

Die Vorlage liefert ausschließlich Organisation und Ablaufideen. Technische Aussagen über
libmodsecurity stammen aus dem offiziellen Quellstand in [upstream-lock.json](upstream-lock.json).
Richtlinien, Berichte und Laufzeitaussagen des Connector-Repositorys belegen kein
Verhalten der Kernbibliothek.

## Zuordnung zu diesem Repository

| Connector-Vorlage | ModSecurity-Anpassung |
| --- | --- |
| Knappe `AGENTS.md` im Hauptverzeichnis | Wiederverwendbare Vereinbarung für beliebige Repositories, in diesem Branch auf Deutsch; Kernvorgaben stehen in `context/repository-context.md` |
| `.codex/context/index.md` und `manifest.toml` | Zugeordnete technische Leitfäden, zuständige Richtlinien, Profile und gespeicherter Stand |
| Zentrale Arbeitsrichtlinien | Dieselben Themengebiete, angepasst an Autotools, C++-/C-APIs, `test/` und `v3/master` |
| `context/profiles/` | Prüfabläufe für Kernbibliothek, Komponenten, Build/CI, Antwortphase und Nachweise |
| `context/security/checks/` | Gezielte Prüfmodule für Bibliothekseingaben, Lebensdauer, Regeln, Protokollierung und Abhängigkeiten |
| `context/security/connectors/` | `context/security/components/` für tatsächliche Kernbereiche |
| `.codex/bin/` | Nur lesende Prüfer für Struktur, Katalogprüfsummen und Quellverweise |
| `.codex/audit/` | Vollständiges festgehaltenes Hauptquelleninventar, Vorlageninventar und datierte Sitzungsaufzeichnungen |
| Struktur- und Vererbungsmanifeste | Lokale Dateizuordnung und Richtlinien-Prüfsummenkatalog ohne erfundene untergeordnete Repositories |
| `.codex/inheritance-templates/` | Ausdrücklich wählbare Wiederverwendungsvorlage und Erklärung; keine aktive Vererbung an Submodule |
| Historische Berichte außerhalb der normalen Richtlinienzuordnung | Sitzungs-/Prüfaufzeichnungen getrennt von aktuellem Stand und zuständigen Richtlinien |

Ausführliche Repository- und C++-Leitfäden, Quellenherkunft, Start-/Abschlussprompts sowie
Dateien für aktuellen Stand, Entscheidungen und Backlog ergänzen die Vorlage, um den
gewünschten wiederverwendbaren Kontext über Chats hinweg bereitzustellen. Die erste
Anpassung war auf Englisch; eine verpflichtende englisch-deutsche Begleitdatei-Konvention
des Referenzrepositorys wurde nicht übernommen. Auf späteren ausdrücklichen Auftrag
liegt nun diese eigenständige deutsche Variante in `codex/kontext-v3-de` vor.
Die Arbeitsvereinbarung ist portabel; der `.codex`-Kontext bleibt auf die Kernbibliothek
zugeschnitten. Die Einrichtung schreibt keine Chatsprache vor; jede Person entscheidet selbst.

## Nicht übernommene Repository-spezifische Regeln

- Übergeordnete/Framework-/MRTS-Topologien, Connector-Host-/Profilmatrizen, historische
  PR-Stände und Migrationsergebnisse beschreiben diesen Kern-Checkout nicht.
- Referenzmaschinen-Pfade, feste Speicherwurzeln, numerische Grenzen, erforderliche globale
  Skills, RTK-Wrapper, Authentifizierungs-Wrapper und Dienstkonfigurationen werden hier nicht vorausgesetzt.
- SonarQube, Laufzeithosts, externe Testframeworks und Qualitätsprüfungen sind bedingte
  Werkzeuge oder gesonderter Aufgabenumfang, keine behaupteten Installationen.
- Zielverhalten einer Referenzrichtlinie wird nicht als implementiertes Kernverhalten
  umbenannt. Insbesondere darf aus einem Bibliotheks-Phasenergebnis nicht abgeleitet werden,
  dass ein externer Host eine Antwort bereits verbindlich gesendet hat.
- Die Richtlinienversion `3.2` der Vorlage ist weder eine ModSecurity-Version noch eine
  Version dieses neuen lokalen Schemas. Die angepassten Manifeste haben eine eigene Schemaversion.

Der Eigentümer wünschte anschließend RTK-Hinweise. Der [RTK-Leitfaden](rtk.md) ergänzt
bedingte Verwendung anhand der eigenen festgehaltenen RTK-Quellen und offizieller Codex-Dokumentation.
Er übernimmt keine maschinenspezifischen Wrapper der Vorlage und schreibt keine Installation vor.
Die wiederverwendbare Arbeitsvereinbarung erhält den Rückgriff auf native Werkzeuge.

Die vier Upstream-Gitlinks der Submodule bleiben unverändert. Darin werden keine
Richtlinien installiert. Eine spätere Aufgabe muss ihre eigene Repository-Grenze untersuchen
und freigeben, bevor dort gemeinsame Anweisungen übernommen werden.

## Pflege der Anpassung

Nutze den lokalen Index als maßgebliche Zuordnung dieser ergänzten Dokumente.
Aktualisiere nach einer bewussten Richtlinienänderung den Katalog und führe die lokalen
Prüfprogramme aus. Überschreibe diese Anpassung nicht regelmäßig aus dem Connector-Repository:
Prüfe relevante Änderungen, halte deren Quellrevision fest und entscheide, ob sie auf
die Kernbibliothek anwendbar sind. Nutzeranweisungen und tatsächlicher Code bleiben maßgeblich.
