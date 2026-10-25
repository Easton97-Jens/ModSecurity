# Richtlinie zur Machbarkeit

Bewerte die Machbarkeit dem Aufgabenumfang entsprechend. Ziel ist ein umsetzbarer
nächster Schritt, keine Freigabepflicht für jede reversible Handlung und kein
formaler Plan für jede kleine Änderung.

## Die Voraussetzungen der Aufgabe klären

Kläre vor einer umfangreichen Implementierung, einem Build, einem externen Vorgang
oder einer Bereitstellung die relevanten Fragen:

- Ist das gewünschte Ergebnis klar genug, um es umzusetzen und zu prüfen?
- Welches Repository und welcher Quellstand sind für das betroffene Verhalten zuständig?
- Welche Quelldateien, Schnittstellen oder vorhandenen Tests liefern Nachweise?
- Sind die benötigten Werkzeuge, Abhängigkeiten und konfigurierten Funktionen verfügbar?
- Passt die Arbeit zu den tatsächlichen Grenzen für Speicherplatz, Arbeitsspeicher, CPU und Prozesse?
- Liegt die Handlung innerhalb der Nutzerfreigabe und der verfügbaren Berechtigungen?
- Gibt es eine aussagekräftige Möglichkeit, das gewünschte Ergebnis zu prüfen?

Nutze [Architektur und Grenzen](architecture-and-boundaries.md) für den Umfang,
[Build und Tests](build-and-test.md) für Build-Voraussetzungen und
[Git-Richtlinie](git-policy.md) für Bereitstellungsziele.

## Den tatsächlichen Stand beschreiben

Diese Bezeichnungen können umfangreiche Arbeit übersichtlicher machen. Eine kurze Erklärung
genügt, wenn eine formale Statustabelle keinen zusätzlichen Nutzen bietet:

| Status | Bedeutung und nächste Handlung |
| --- | --- |
| `feasible_now` | Die relevanten Voraussetzungen sind vorhanden; innerhalb des Umfangs fortfahren |
| `feasible_after_local_setup` | Reversible, zur Aufgabe gehörende Einrichtung kann die Voraussetzungen schaffen; bei vorhandener Freigabe ausführen |
| `requires_user_decision` | Eine wesentliche Entscheidung lässt sich nicht aus der Aufgabe oder den Nachweisen ableiten; konkrete Optionen vorbereiten |
| `blocked_environment` | Eine benötigte lokale Fähigkeit ist nicht verfügbar; unabhängige Arbeit abschließen und die Lücke erklären |
| `blocked_external_dependency` | Eine notwendige externe Eingabe oder ein Dienst ist nicht verfügbar |
| `blocked_permissions` | Eine erforderliche Handlung liegt außerhalb der erteilten Befugnisse oder des erlaubten Zugriffs |
| `blocked_missing_evidence` | Die verfügbaren Nachweise können eine notwendige Aussage nicht belegen |
| `already_fixed` | Der bezeichnete Stand behebt das gemeldete Problem bereits; Nachweis angeben |
| `unsupported_with_evidence` | Konkrete Quellen-, Build- oder Ausführungsnachweise belegen die genannte Einschränkung |
| `not_applicable` oder `out_of_scope` | Der Punkt gehört nicht zur aktuellen Aufgabe; bei wesentlicher Bedeutung begründen |

Ein fehlender Compiler, eine nicht verfügbare Abhängigkeit, ein nicht initialisiertes
Submodul oder eine fehlgeschlagene Netzwerkanfrage beweist für sich genommen nicht,
dass die Funktion in ModSecurity fehlt. Trenne Grenzen der Umgebung von Grenzen des Produkts.

## Sinnvolle Arbeit fortsetzen

Bereits im Gespräch festgelegte Freigaben und Präferenzen gelten weiter, solange der
Nutzer sie nicht ändert. Frage sie nicht erneut ab, nur weil die Arbeit eine neue Datei,
Phase oder ein anderes Chat-Werkzeug erreicht. Notwendige reversible Untersuchungen,
Änderungen und Einrichtungsschritte innerhalb der freigegebenen Aufgabe dürfen fortgesetzt werden.

Schließe unabhängige Recherche, Änderungen und prüfbare Vorbereitungen ab, wenn ein anderer
Teil blockiert ist. Frage nur nach einer fehlenden Entscheidung oder Freigabe, die die
verbleibende Arbeit tatsächlich verhindert. Erkläre die konkrete Handlung und warum sie
die Mitwirkung des Nutzers benötigt; lasse keinen unvorbereiteten Vorschlag freigeben.

Nicht verfügbare optionale Werkzeuge blockieren keine Arbeit, die sich mit vorhandenen
repository-eigenen Methoden angemessen prüfen lässt. Installiere keine Systempakete,
ändere keine Dienste und nutze keine kostenpflichtigen externen Ressourcen allein zur
Erfüllung einer kopierten Werkzeugliste. Kläre, ob eine solche Einrichtung notwendig
und bereits freigegeben ist.

Ändern sich Nachweise oder Umgebung, aktualisiere die Bewertung. Berichte die tatsächliche
verbleibende Lücke gemäß den [Abschlusskriterien](definition-of-done.md).
