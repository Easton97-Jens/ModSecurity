# Ablauf für reproduzierbare Sitzungen

Dieses Kapitel legt den für dieses Arbeitsrepository gewählten Zusammenarbeitsablauf fest.
Es ist keine von ModSecurity implementierte Funktion.

## 1. Aufgabe und Ausgangspunkt klären

Lies die wiederverwendbare [AGENTS.md](../../AGENTS.md) und die Startanweisungen
dieses Checkouts in [repository-context.md](repository-context.md). Prüfe Branch und
Status und ermittle die Aufgabe. Vergleiche den Checkout mit
[upstream-lock.json](upstream-lock.json). Ältere Notizen gelten für ihre angegebenen
Revisionen. Prüfe, ob relevante Dateien seit diesen Notizen verändert wurden.

Halte Nutzerentscheidungen, Code-Erkenntnisse und Arbeitshypothesen unterscheidbar.
Frage nach einer offenen Entscheidung mit wesentlichen Folgen. Routinemäßige Entscheidungen
innerhalb des freigegebenen Auftrags können getroffen, erklärt und festgehalten werden.

## 2. Erklärungen aus dem Code ableiten

Suche konkrete Symbole und Pfade, beispielsweise mit `rg`. Lies neben Deklarationen
auch Implementierungen, Aufrufer, Fehlerpfade und relevante Tests. Prüfe bei optionalen
Funktionen außerdem Build-Definitionen und die vom Parser akzeptierten Eingaben.

Eine hilfreiche Erklärung beantwortet diese Fragen in Reihenfolge:

1. Welchen Zweck erfüllt die Funktion in diesem Repository?
2. Welche Daten erhält sie und wem gehören diese Daten?
3. Welchen Zustand oder welche Rückgabewerte kann sie ändern?
4. Wer ruft sie auf und in welcher Reihenfolge?
5. Welche Bedingungen, Funktionsschalter und Grenzen gelten?
6. Welcher Test legt das erwartete Verhalten fest und wurde dieser Test ausgeführt?

Verwende Links mit vollständigen Commit-IDs. Veränderliche Branch-Links sind zur
Navigation hilfreich, bilden aber keine stabile Nachweisgrundlage. Kennzeichne aus
mehreren Funktionen abgeleitete Schlussfolgerungen und nenne ihre Nachweise.

## 3. Änderungen vornehmen

Definiere zuerst das erforderliche beobachtbare Verhalten. Begrenze Änderungen auf
betroffene Dateien und prüfe Anweisungen für ihre Verzeichnisse. Erhalte sachfremde
Änderungen und vermeide eine nicht angefragte Neugestaltung.

Nutze für einen Fehler einen vorhandenen oder gezielten neuen Regressionstest, der ihn
tatsächlich erkennen kann. Erklärungen und Dokumentationskorrekturen benötigen entsprechende
Quellen- und Dokumentprüfungen. Ein Test, der lediglich ein Implementierungsdetail
wiederholt, ersetzt keine Anforderung.

## 4. Prüfergebnisse festhalten

| Feld | Festzuhaltende Information |
| --- | --- |
| Ziel | Geprüftes Verhalten oder geprüfte Dokumentationskonsistenz |
| Revision | Commit, Branch und relevante noch nicht committete Änderungen |
| Umgebung | Relevante Plattform und verfügbare Abhängigkeiten |
| Befehl | Exakt ausgeführter Befehl, keine idealisierte spätere Fassung |
| Arbeitsverzeichnis | Repository-relatives Verzeichnis oder eindeutiger lokaler Pfad |
| Ergebnis | Exit-Code, wesentliche Ausgabe, ausgeführte und übersprungene Tests |
| Grenze | Was der Lauf nicht abdeckte oder was seinen Abschluss verhinderte |

Der Kontextprüfer ist ein ergänztes Projekthilfsprogramm und kein Upstream-Test.
Ein erfolgreicher Lauf belegt keine Korrektheit für jede Plattform, Konfiguration
oder Sicherheitseigenschaft.

## 5. Den nächsten Chat vorbereiten

Halte `current-state.md` knapp und aktuell. Dokumentiere den letzten verlässlichen
Stand, offene Fragen und die nächste konkrete Aktion. Speichere längere historische
Aufzeichnungen unter `.codex/audit/sessions/`.

Nutze `decisions.md` für dauerhafte Entscheidungen und ihre Gründe. Nutze `backlog.md`
für tatsächlich verbleibende Arbeit. Verlinke das relevante Kapitel, statt eine lange
Erklärung über alle drei Dateien hinweg zu duplizieren.

Nutze [handoff-template.md](handoff-template.md) für eine Übergabe. Speichere den Kontext
in der tatsächlichen gemeinsamen Arbeitskopie oder im freigegebenen Repository.
Eine Aussage im Chat aktualisiert keine Projektdatei.

## Grenzen

Diese Dateien geben neuen Sitzungen einen überprüfbaren Ausgangspunkt. Sie lesen oder
aktualisieren sich nicht selbst und können veralten. Jede Sitzung beginnt daher mit
einer Zugriffs- und Revisionsprüfung sowie gezieltem Lesen. Das reduziert wiederholte
Orientierungsarbeit, ohne nicht verfügbare Informationen als erinnerte Fakten darzustellen.
