# Projektlokale Kontextprüfer

Diese Hilfsprogramme sind Ergänzungen für diesen Fork. Sie sind vom Build-System
und den Tests von ModSecurity getrennt. Führe sie in einem Checkout aus; sie ermitteln
das Hauptverzeichnis aus ihrem eigenen Speicherort. Sie laden nichts herunter, bauen
oder installieren nichts, rufen keine Scanner auf und ändern keine Dateien, außer
bei Verwendung der unten ausdrücklich beschriebenen Option zur Prüfsummenaktualisierung.

| Befehl | Was er nachweist |
| --- | --- |
| `python3 .codex/bin/verify-context.py` | Festgehaltene Git-Grundlage, vollständiges Quelleninventar, lokale Markdown-Ziele, ModSecurity-Quellpfade/Zeilenbereiche mit vollständiger Commit-ID und gemeldete Quellabweichungen |
| `python3 .codex/bin/verify-context.py --strict-source` | Dieselben Prüfungen; schlägt bei Quellabweichungen außerhalb von `AGENTS.md`, `CODEX_START.md` und `.codex/` fehl |
| `python3 .codex/bin/validate-structure.py` | Erforderliche Struktur, Schema, Verweisziele, Profil-/Komponenten-/Prüfinventare und Zuordnungsmarkierung in der Hauptanweisung |
| `python3 .codex/bin/validate-codex-inheritance.py --check` | Der lokale Katalog der primären Richtlinien stimmt genau mit den zugeordneten Richtliniendateien und ihren SHA-256-Prüfsummen überein |

Der Quellenprüfer verwendet die Python-Standardbibliothek und Git mit `--no-lazy-fetch`,
damit fehlende Objekte gemeldet und nicht heruntergeladen werden. Die TOML-Prüfer benötigen
**Python 3.11 oder neuer** (`tomllib`); eine Paketinstallation ist nicht erforderlich.
Gewöhnliche Abweichungen führen zum Exit-Code `1`; unlesbare Eingaben oder nicht verfügbare
Voraussetzungen zum Exit-Code `2`. Erfolgreiche Prüfungen enden mit `0`.

## Bewusste Richtlinienpflege

Erzeuge nach der Überprüfung einer Änderung an einer primären Richtlinie nur den lokalen Katalog neu:

```sh
python3 .codex/bin/validate-codex-inheritance.py --refresh
python3 .codex/bin/validate-codex-inheritance.py --check
```

`--refresh` schreibt `.codex/inheritance-manifest.toml` aus der aktuellen Zuordnungsdatei neu.
Prüfe diesen Diff. Aktualisierte Hashwerte halten geänderten Inhalt fest; sie beweisen
nicht die Richtigkeit der Änderung. Routinemäßige Sitzungsprüfungen verwenden `--check`.
Es gibt keine aktive oder automatisch erkannte Richtlinienvererbung an untergeordnete Repositories/Submodule.

## Grenzen

Quellverweise werden gegen lokal verfügbare Git-Objekte geprüft. Historische
Quellrevisionen sind nur in Entscheidungs- und Sitzungsaufzeichnungen zulässig.
Eingezäunte Codebeispiele werden beim Auswerten von Markdown-Links ausgeschlossen.
Externe Websites und lokale Überschriftenanker werden nicht abgerufen oder vollständig geprüft.
Quellauszüge müssen weiterhin inhaltlich geprüft werden, um festzustellen, ob sie die
zugehörige Aussage tatsächlich stützen.

Eine erfolgreiche Kontextprüfung belegt weder einen erfolgreichen Bibliotheks-Build
noch Laufzeitverhalten, Sicherheit, Integrationsabdeckung oder vollständige C++-Dokumentation.
