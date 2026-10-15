# Vorbereitete Repositories / Prepared repositories

Beide Pakete enthalten die vollständige Dokumentation auf Deutsch **und** Englisch,
dieselben Tests und festen Erwartungen sowie die bisherigen und neuen Berichte.
Nur die Haupt-README startet mit einer anderen Sprache. Neue GitHub-Repositories
wurden nicht angelegt; diese vollständigen lokalen Repository-Stände sind zur
weiteren Veröffentlichung vorbereitet.

Both packages contain the complete German **and** English documentation, identical
tests and fixed expectations, and all prior and new reports. Only the primary
README uses a different entry language. These are complete prepared local Git
repositories; no new GitHub repositories have been created.

| Repository | ZIP | Git bundle | Local Git commit |
|---|---|---|---|
| ModSecurity-JSON-Tests-DE | [Download](ModSecurity-JSON-Tests-DE.zip) | [Bundle](ModSecurity-JSON-Tests-DE.bundle) | `8e033cddd78d0d8484355831f9d5ff00fcd218e5` |
| ModSecurity-JSON-Tests-EN | [Download](ModSecurity-JSON-Tests-EN.zip) | [Bundle](ModSecurity-JSON-Tests-EN.bundle) | `2ae50a74475510445bfa9ed316af0befdb755b02` |

[Exportmanifest / Export manifest](export-manifest.json): vollständige Dateilisten,
SHA-256 aller Dateien und Pakete, Inhaltsvergleich und Originalbezüge.
[Prüfbericht / Verification](verification.json): frische Läufe aus beiden Paketen,
Bundle-/ZIP-Prüfung und unveränderte bisherige Dokumentation.

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
