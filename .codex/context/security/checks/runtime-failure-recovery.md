# Fragen zu Fehlern, Freigabe und Folgefällen

Wähle Fehler, die in der tatsächlichen Bibliothek beziehungsweise im Testgerüst erreichbar sind: ungültige Regeln, nicht verfügbare optionale Verarbeitung, fehlerhafte Körper, Grenzwertbehandlung, fehlgeschlagene Datei- oder Backend-Operationen, Callback-Probleme oder Fehlerbehandlung des Aufrufers. Prüfe den konkreten Fehlermechanismus, statt seine Existenz anzunehmen.

- Welcher Rückgabewert, Fehlertext, welche Variable, Intervention oder Ausnahme transportiert den Fehler?
- Erhält der Aufrufer dieses Signal oder wandelt er es in scheinbaren Erfolg um?
- Welche Transaktionsressourcen oder gemeinsamen Ressourcen bleiben nach dem Fehler zurück?
- Erfolgt die Freigabe einmal durch den richtigen Eigentümer, und bleibt ein zulässiger Folgefall unbeeinträchtigt?
- Folgte ein späterer Erfolg auf eine Einrichtungskorrektur, oder gibt es unerklärtes sporadisches Verhalten?

Halte den ursprünglichen Fehler und jede wesentliche Korrektur fest. Ein erfolgreicher Wiederholungsversuch löscht die frühere Beobachtung nicht. Host- oder Dienstneustarts sowie Netzwerkzeitüberschreitungen gehören nur dann hierher, wenn eine gesondert abgegrenzte Integration sie tatsächlich bereitstellt.

Einstiegspunkte: [API-Grenze](../components/api-boundary.md), [Verarbeitung der Nachrichtenkörper](../components/body-processors.md), [Protokollierung und Speicherung](../components/logging-storage.md) und [Nachweisintegrität](evidence-integrity.md).
