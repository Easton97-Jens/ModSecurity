# Python-Umgebungen und Abhängigkeiten

## Geltungsbereich

Diese Arbeitsrichtlinie gilt für Dokumentationshilfen, Quellgeneratoren,
optionale Python-Werkzeuge und ausdrücklich zum Auftrag gehörende Arbeiten an
Anbindungen. Sie macht Python nicht zur Sprache der Core-Engine und setzt
keine bestimmte vorhandene virtuelle Umgebung voraus.

## Die tatsächliche Python-Aufgabe bestimmen

| Aufgabe | Einstiegspunkt und Grenze |
| --- | --- |
| Dokumentationsvalidierung | `python3 .codex/bin/verify-context.py --strict-source` und `python3 .codex/bin/validate-structure.py` im Stammverzeichnis des Repositorys ausführen, wie in [testing-and-evidence.md](testing-and-evidence.md) beschrieben. Dies sind Hilfsprogramme des Arbeitsrepositorys. |
| Erzeugung von HTML-Entities | `tools/gen-html-entities.py` liest eine JSON-Eingabe und gibt erzeugte Tabelleninhalte aus. Importiert werden `json`, `sys` und `collections.defaultdict`. Vor einer Neuerzeugung Eingabe und Ausgabeziel prüfen. |
| Python-Anbindungen | `bindings/python` ist ein eigenständiges deklariertes Submodul. Vor Änderungen müssen dessen eigener Quellstand, Anweisungen und Abhängigkeitsvorgaben geprüft werden. |
| Windows-Build-Werkzeuge | Die versionierten Windows-Workflows verwenden Conan. Eine kompatible Umgebung anhand des tatsächlichen Build-Wegs wählen, statt einen global installierten Paketmanager vorauszusetzen. |

Upstream-Belege: [Entity-Generator][generator],
[Deklaration der Anbindungen][bindings], [Windows-CI-Werkzeuge][conan-ci],
[Windows-Release-Werkzeuge][conan-release]. Die beiden angeführten
Windows-Workflows verwenden unterschiedliche Conan-Installationsbefehle;
leite nicht aus einem einzelnen eine allgemeingültige Versionsvorgabe ab.

## Einen Interpreter bewusst auswählen

Verwende einen verfügbaren Python-3-Interpreter, der zum Skript und seiner
dokumentierten Umgebung passt. Lies bei unklarer Kompatibilität die Imports
und versionsabhängige Syntax. Eine lokale `.venv` oder Datei zur
Interpreterauswahl ist ein Beleg für diesen Checkout, keine neue
Kompatibilitätszusage von Upstream.

Ein Skript, das nur die Standardbibliothek benötigt, erfordert weder ein
Paketmanifest noch die Installation eines Fremdpakets. Benötigt eine Aufgabe
externe Abhängigkeiten, verwende eine geeignete isolierte Umgebung innerhalb
der aktuellen Dateisystemberechtigungen. Halte Interpreter, benötigte
Paketversionen und alle für die Reproduzierbarkeit relevanten
Abhängigkeitsänderungen fest.

Lass System-Python und das Benutzer-Paketverzeichnis bei gewöhnlicher
Repository-Arbeit unverändert. Sprich bei einer autorisierten Paketinstallation
den gewählten Interpreter ausdrücklich über seine Schnittstelle `-m pip` an.
Umgebungssteuerungen wie `PYTHONNOUSERSITE=1`, `PIP_REQUIRE_VIRTUALENV=true` und
`PIP_DISABLE_PIP_VERSION_CHECK=1` können helfen, eine isolierte Installation auf
ihren Bereich zu beschränken; sie erstellen oder aktivieren selbst keine
Umgebung.

## Änderungen an Abhängigkeiten und Werkzeugen

Unterscheide von gepflegtem Code oder Tests importierte Pakete von
eigenständigen Analysewerkzeugen. Eine neue dauerhaft gepflegte Abhängigkeit
benötigt einen ausdrücklichen Zweck, eine kompatible Versionsauswahl,
entsprechende Manifest-/Lock-Änderungen, sofern die Komponente diese verwendet,
und eine Prüfung des betroffenen Verhaltens.

Eine optionale externe CLI darf nicht stillschweigend zur Produktabhängigkeit
werden. Installiere oder aktualisiere Werkzeuge nicht allein deshalb, weil ein
Prüfprogramm dazu rät. Wenn eine Installation notwendig ist und zum Auftrag
gehört, verwende die für diese Aufgabe geltende vertrauenswürdige Paketquelle
und Versionsauswahl. Halte ungeklärte Verfügbarkeit oder Kompatibilität als
Einschränkung fest, statt auf eine systemweite Installation auszuweichen.

Kopiere keine Zugangsdaten in Paket-URLs, Befehle, Berichte oder Manifeste.
Ändere weder das Submodul der Anbindungen noch seinen Gitlink, nur weil eine
Dokumentationshilfe Python benötigt. Die tatsächlichen Submodulgrenzen stehen
in [resources-and-storage.md](resources-and-storage.md).

## Validierung

Syntaxprüfungen, Importprüfungen, Verhalten der Hilfsprogramme, Formatierung,
Typanalyse, Paketkonsistenz und Abhängigkeitsaudits belegen unterschiedliche
Sachverhalte. Wähle die für die Änderung relevanten Prüfungen aus und berichte
ihre tatsächlichen Ergebnisse. Kein Linter, Typprüfer, Paketaudit-Dienst oder
Installationsablauf wird allein durch seine Erwähnung hier verpflichtend.

Prüfe bei Änderungen an Hilfsprogrammen das relevante veränderte Erfolgs- oder
Fehlerverhalten, ohne eine Markdown-Aufgabe auf sachfremde Umgebungsarbeiten
auszuweiten. Ordne erzeugte Ausgaben und temporäre Testdaten eindeutig der
Aufgabe zu und bereinige sie gemäß
[cleanup-and-restoration.md](cleanup-and-restoration.md).

[generator]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/tools/gen-html-entities.py#L20-L86
[bindings]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.gitmodules#L7-L9
[conan-ci]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.github/workflows/ci_new.yml#L197-L206
[conan-release]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.github/workflows/release-windows.yml#L45-L53
