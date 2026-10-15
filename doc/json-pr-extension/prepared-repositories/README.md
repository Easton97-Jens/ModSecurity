# Vorbereitete Repositories / Prepared repositories

Das EN-Paket enthält ausschließlich englische Dokumentation, einschließlich der
vollständig übersetzten bisherigen Berichte. Das DE-Paket enthält Deutsch und
Englisch. Beide enthalten dieselben ausführbaren Tests, festen Erwartungen und
maschinenlesbaren Testergebnisse. Neue GitHub-Repositories wurden nicht angelegt;
diese vollständigen lokalen Repository-Stände sind zur Veröffentlichung vorbereitet.

The EN package contains English documentation only, including complete translations
of the historical reports. The DE package contains German and English. Both contain
identical executable tests, fixed expectations and machine-readable test results.
These are complete prepared local Git repositories; no new GitHub repositories
have been created.

| Repository / languages | ZIP | Git bundle | Local Git commit |
|---|---|---|---|
| ModSecurity-JSON-Tests-DE / DE + EN | [Download](ModSecurity-JSON-Tests-DE.zip) | [Bundle](ModSecurity-JSON-Tests-DE.bundle) | `f84b9f701effcc3fb2710fba72682a705f73c35c` |
| ModSecurity-JSON-Tests-EN / EN only | [Download](ModSecurity-JSON-Tests-EN.zip) | [Bundle](ModSecurity-JSON-Tests-EN.bundle) | `55dff1f618fc4e67828d64e909a8cb690b79ac31` |

[Exportmanifest / Export manifest](export-manifest.json): vollständige Dateilisten,
SHA-256 aller Dateien und Pakete, Inhaltsvergleich und Originalbezüge.
[Prüfbericht / Verification](verification.json): frische Läufe aus beiden Paketen,
Bundle-/ZIP-Prüfung, Sprachprofile und unveränderte Testdaten. Im DE-Paket bleiben
die bisherigen Originaldokumente erhalten; im EN-Paket stehen ihre vollständigen
englischen Übersetzungen.

Die eigenständigen Git-SHAs oben gehören zu den lokalen Test-Repositories, nicht
zum offiziellen ModSecurity oder zum Fork-Aufgabenbranch. Dessen verifizierte
Veröffentlichungs-SHA steht im abschließenden Übergabebericht. Ein erfolgreicher
Push macht die 20 sichtbaren Anforderungsfehler nicht zu bestandenen Tests.

The Git SHAs above identify the local standalone test repositories, not official
ModSecurity or the fork task branch. The handover reports the verified fork
publication SHA separately. Publishing does not make the 20 unmet requirement
assertions pass.

```sh
git clone ModSecurity-JSON-Tests-DE.bundle ModSecurity-JSON-Tests-DE
git -C ModSecurity-JSON-Tests-DE bundle verify ../ModSecurity-JSON-Tests-DE.bundle
```

Für EN die Dateinamen ersetzen; nur neue Zielverzeichnisse verwenden.
For EN, substitute its filenames; use new destination directories only.

Die ZIPs enthalten keine `.git`-Historie; die Bundles enthalten sie vollständig.
Beide enthalten alle erforderlichen Fixtures. Für Build und Ausführung wird ein
gesonderter vollständiger Original-Checkout benötigt, wie in den enthaltenen
deutschen und englischen Anleitungen beschrieben.

ZIPs contain the tracked files; bundles also preserve their complete Git history.
Both include every required fixture. Building/running needs a separate complete
original checkout, as explained in the included German and English guides.
