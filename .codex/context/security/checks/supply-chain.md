# Fragen zur Herkunft von Abhängigkeiten und Builds

Inventarisiere nur tatsächliche Compiler, Bibliotheken, Submodul-Commits, Werkzeuge, Workflows, heruntergeladene Quellen, Container und Artefakte des ausgewählten Builds. Trenne Quellen des Haupt-Repositorys von Submodul- und Systempaketinhalten.

- Welche unveränderliche Revision, Paketversion, Quelle oder welcher Digest kennzeichnet jede Eingabe?
- Entspricht der Checkout seinen Gitlinks, und lassen sich generierte Quellen auf ihre Eingaben zurückführen?
- Welche optionalen Abhängigkeiten und Funktionsmakros sind tatsächlich aktiviert?
- Wie werden Downloads, Archivextraktion, Build-Skripte, Caches und Artefakte ausgewählt und geprüft?
- Kann eine nicht vertrauenswürdige Workflow-Eingabe oder ein Pull Request ein privilegiertes Zugangsmittel, einen Befehl oder ein gemeinsames Artefakt erreichen?
- Entspricht die gebaute beziehungsweise geladene Bibliothek den Headern und der Revision, die der Test verwendet?

Eine Versionsbindung ist kein abgeschlossenes Audit; ein neueres Release oder ein Sicherheitshinweis ist ein Rechercheansatz. Eine rein lesende Workflow-Prüfung erlaubt weder das Auslösen einer Bereitstellung noch Änderungen an Abhängigkeiten.

Einstiegspunkte: [Build und Abhängigkeiten](../components/build-dependencies.md), [Konfigurations- und Eingabeinjektion](config-injection.md), [Recherche zu Sicherheitshinweisen](advisory-research.md) und [Nachweisintegrität](evidence-integrity.md).
