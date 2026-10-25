# Dokumentation und Nachvollziehbarkeit

Diese Dateien bewahren hilfreichen Kontext über Chats hinweg. Sie halten fest, was bekannt ist,
was sich geändert hat und wie das Ergebnis geprüft wurde. Sie geben einem Assistenten kein
Gedächtnis für nicht zugängliche Gespräche und beweisen nicht, dass jede Datei geprüft wurde.

## Sprache und Quellentreue

Pflege alle ergänzten Dokumente, Anweisungen, Prompts und Meldungen der Prüfprogramme
in dieser Variante auf Deutsch. Erhalte Quellsymbole, Pfade, Befehle, Konfigurationsschlüssel,
Git-IDs, URLs und zitierte Diagnosemeldungen exakt. Die englische Fassung liegt separat in
`codex/kontext-v3`; der Nutzer hat die deutsche Fassung ausdrücklich für `codex/kontext-v3-de`
beauftragt. Die Dateisprache schreibt keine Chatsprache vor.

Zitiere für technische Aussagen die tatsächlich gelesene Implementierung oder den Vertrag
mit Quellpfad, relevantem Symbol und vollständigem Quell-Commit. Ein an einen Commit
gebundener GitHub-Link mit passendem Zeilenbereich ist hilfreich, wenn er die Aussage stützt.
Nutze [provenance.md](provenance.md) und [upstream-lock.json](upstream-lock.json),
um die Dokumentationsgrundlage zu bezeichnen.

Unterscheide vier Arten von Aussagen:

| Art | Was Lesende benötigen |
| --- | --- |
| Quellenfakt | Implementierung, Deklaration, Build-Bedingung oder Testdefinition eines bezeichneten Stands |
| Ausgeführtes Ergebnis | Tatsächlicher Befehl, Quellrevision, relevante Umgebung/Optionen, Ergebnis und Grenzen |
| Schlussfolgerung | Stützende Beobachtungen und Begründung; als Schlussfolgerung kennzeichnen |
| Vorschlag oder Unsicherheit | Beabsichtigte Änderung oder offene Frage; nicht als umgesetzt oder überprüft beschreiben |

Eine Testdefinition legt einen erwarteten Fall fest. Ein ausgeführter Test zeigt das
beobachtete Ergebnis unter den Bedingungen dieses Laufs. README, Klassenname,
Scanner-Ergebnis oder generierter Bericht ersetzen keinen Nachweis für eine weitergehende Aussage.
Halte hilfreiche Abweichungen zwischen Dokumentation und Code in [fact-check.md](fact-check.md) fest.

## Wohin Informationen gehören

| Information | Primärer Ort |
| --- | --- |
| Projektzweck und Grenze zwischen Bibliothek und Connector | [project-context.md](project-context.md) |
| Laufzeitaufbau und Navigation im Code | [architecture.md](architecture.md), [repository-map.md](repository-map.md) und das relevante technische Kapitel |
| Tatsächlicher Arbeitsstand, Prüfungen und nächster Schritt | [current-state.md](current-state.md) |
| Dauerhafte Entscheidung und ihr Grund | [decisions.md](decisions.md) |
| Tatsächlich offene Arbeit oder ungeklärte Frage | [backlog.md](backlog.md) |
| Umfangreichere sitzungsspezifische Historie und Nachweise | [Sitzungsaufzeichnungen](../audit/sessions/), anhand von [handoff-template.md](handoff-template.md) |
| Quellgrundlage und bewusste Aktualisierungen | [provenance.md](provenance.md), [upstream-sync.md](upstream-sync.md) |

Pflege je Thema eine maßgebliche Erklärung und verlinke sie aus ergänzenden Richtlinien.
Halte den aktuellen Stand knapp. Erhalte wesentliche historische Beobachtungen mit ihren
ursprünglichen Daten und Revisionen, statt sie als aktuell umzuschreiben. Entferne oder
kennzeichne überholte Pläne, wenn spätere Anweisungen die Aufgabe verändern.

## Was nach einer Änderung festgehalten wird

Bewahre bei einer wesentlichen Änderung genügend Informationen, um Folgendes zu erklären:

- Auftrag und betroffener Umfang;
- bisheriges und neues Verhalten oder bisherige und neue Dokumentation;
- tatsächlich geänderte Dateien und wesentliche Entscheidungen;
- tatsächlich ausgeführte Befehle, Ergebnisse und nicht ausgeführte Prüfungen;
- offene Punkte und ein konkreter nächster Schritt;
- Commit-, Remote-Branch-, PR- oder Integrationsfakten, soweit tatsächlich beobachtet.

Ein kurzer Sitzungseintrag oder ein aktualisierter Abschnitt zum aktuellen Stand genügt meist.
Führe kein zweites Berichtssystem und keine verpflichtende zweisprachige Änderungsaufzeichnung ein.
Erfinde niemals Commit, PR-Nummer, Review-Ergebnis, CI-Status, Merge, Testergebnis,
Leistungswert oder Sicherheitsgarantie.

Halte Zugangsdaten, private Anfrageinhalte, personenbezogene Daten und sensible Protokolle
aus dem versionierten Kontext heraus. Bevorzuge eine minimale bereinigte Reproduktion
und begrenzte Diagnosen. Speichere keine vollständigen Umgebungsabbilder als Nachweis.

## Generierte Daten und Konsistenzprüfungen

Ermittle vor Änderungen an generierten Dateien deren tatsächliche Quelle und Generator.
Erzeuge sie bei Bedarf mit diesem Verfahren neu; manuelles Bearbeiten der Ausgabe
ersetzt es nicht. Die Aktualisierung des Quelleninventars beschreibt [upstream-sync.md](upstream-sync.md).

Die lokalen Kontextprüfungen vom Repository-Hauptverzeichnis aus lauten:

```sh
python3 .codex/bin/verify-context.py
python3 .codex/bin/validate-structure.py
```

Lies ihren gemeldeten Umfang und ihre Grenzen. Strukturelle Konsistenz und gültige
Quellverweise belegen weder die inhaltliche Richtigkeit jedes Absatzes noch einen
erfolgreichen ModSecurity-Build. Ist ein benötigtes Werkzeug oder Objekt nicht verfügbar,
halte die Einschränkung fest und führe die weiterhin möglichen sinnvollen Prüfungen aus.
