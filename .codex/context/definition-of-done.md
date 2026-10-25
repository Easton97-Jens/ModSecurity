# Abschlusskriterien

Der Abschluss wird an der aktuellen Nutzeraufgabe gemessen, einschließlich akzeptierter
Präzisierungen und früherer Freigaben im Gespräch. Er behauptet nicht, dass jede
ModSecurity-Funktion oder nachgelagerte Integration geprüft wurde.

## Prüfergebnisse

Beschreibe Ergebnisse präzise; diese Bezeichnungen stehen zur Verfügung, wenn eine Tabelle hilft:

| Status | Bedeutung |
| --- | --- |
| `passed` | Die angegebene Prüfung wurde ausgeführt und erfüllte ihr Abnahmekriterium |
| `failed` | Die Prüfung wurde ausgeführt und schlug fehl oder widerlegte das behauptete Ergebnis |
| `blocked` | Eine notwendige Voraussetzung, ein Zugriff, eine Entscheidung oder eine Umgebung fehlt |
| `not_run` | Die Prüfung wurde nicht ausgeführt; nenne den wesentlichen Grund |
| `not_applicable` | Die Prüfung gilt nicht für den vereinbarten Umfang |
| `not_verified` | Für eine Aussage fehlen ausreichende Nachweise zur Bestätigung |

Eine vorhandene Testdatei, ein vorgeschlagener Befehl oder ein früherer Durchlauf gegen
anderen Code ist keine neue bestandene Prüfung.

## Aufgabenergebnis

- **Vollständig:** Die angefragten Ergebnisse und notwendigen Abnahmekriterien sind erfüllt,
  mit Nachweisen, die dem Umfang entsprechen.
- **Teilweise:** Sinnvolle Arbeit ist abgeschlossen, aber ein angefragtes Ergebnis oder eine
  erforderliche Prüfung bleibt unvollständig.
- **Blockiert:** Eine notwendige fehlende Voraussetzung oder Entscheidung verhindert weitere
  Fortschritte bei der verbleibenden Arbeit.
- **Fehlgeschlagen:** Die Prüfung zeigt, dass das angefragte Ergebnis nicht erreicht wurde.

Dokumentationsarbeit kann ohne C++-Build abgeschlossen sein, wenn sie das Produktverhalten
nicht ändert und Quellen- beziehungsweise Verweisprüfungen ihre Abnahmekriterien belegen.
Eine erforderliche, nicht ausgeführte Verhaltensprüfung muss als ausdrückliche Lücke bestehen bleiben.
Optionale sachfremde Tests werden nicht allein durch ihre Existenz zu Anforderungen.

## Abschlussprüfung

Bestätige vor der Abschlussmeldung Folgendes in einer für die Aufgabe hilfreichen Detailtiefe:

1. Das tatsächliche Ergebnis erfüllt den angefragten Umfang und alle wesentlichen Vorgaben.
2. Änderungen sind vorhanden und prüfbar; vorgeschlagene oder geplante Arbeit ist als solche gekennzeichnet.
3. Die geänderten Dateien und der abschließende Diff wurden geprüft. Behebe relevante
   Leerraumbefunde aus `git diff --check` oder einer gleichwertigen Prüfung.
4. Prüfergebnisse beschreiben, was tatsächlich ausgeführt wurde, einschließlich Fehlschlägen,
   ausgelassenen Prüfungen und Grenzen der Umgebung.
5. Technische Erklärungen unterscheiden Quellfakten, Ausführungsergebnisse und verbleibende
   Unsicherheit gemäß der [Quellenherkunft](provenance.md).
6. Die relevante Dokumentation und der wiederverwendbare Kontext spiegeln das Ergebnis wider;
   eine weitere Sitzung kann den nächsten Schritt bestimmen, ohne dieses Gespräch zu rekonstruieren.
7. Sachfremde Arbeit und Quellgrenzen bleiben erhalten. Zur Aufgabe gehörende Prozesse,
   Agenten und temporäre Artefakte haben einen bekannten aufbewahrten oder abgeschlossenen Zustand.
8. Aussagen zur Bereitstellung entsprechen beobachteten Commits, Branches, PRs oder Merge-Ergebnissen.
   Handlungen bleiben innerhalb der Nutzerfreigabe und tatsächlichen Berechtigungen.

Nutze den [Änderungsablauf](change-workflow.md) für die Umsetzung,
die [Befundrichtlinie](findings-policy.md) für offene Befunde und
[Integration in den Hauptbranch](master-integration.md) nur, wenn Integration zum Umfang gehört.
Wiederhole für eine triviale Änderung nicht jede Richtlinie als eigene Prüfliste.

## Abschlussbericht und gespeicherter Kontext

Benenne Ergebnis, Änderungen und Gründe, aussagekräftige Prüfungen und Ergebnisse,
verbleibende Grenzen sowie den tatsächlichen Bereitstellungsstand. Erkläre ein teilweises
oder blockiertes Ergebnis direkt, statt nicht ausgeführte Arbeit als abgeschlossen zu markieren.

Aktualisiere den [aktuellen Stand](current-state.md), die [Entscheidungen](decisions.md) oder
[offenen Aufgaben](backlog.md), wenn sich ihre Angaben geändert haben. Nutze die
[Übergabevorlage](handoff-template.md) und das
[Sitzungsverzeichnis](../audit/sessions/) für eine hilfreiche dauerhafte Aufzeichnung
umfangreicher Arbeit. Erstelle keine leeren Aufzeichnungen allein zur Erfüllung eines Rituals.
