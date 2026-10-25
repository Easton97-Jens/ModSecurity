# Prompt zum Sitzungsabschluss

```text
Bereite eine Übergabe für einen neuen Chat vor.

Prüfe den tatsächlichen Diff und benenne die geänderten Dateien.
Aktualisiere .codex/context/current-state.md mit dem aktuellen Arbeitsstand,
den tatsächlich ausgeführten Prüfungen und ihren Ergebnissen sowie offenen Fragen.
Halte dauerhafte neue Entscheidungen in .codex/context/decisions.md fest und
beschränke .codex/context/backlog.md auf tatsächlich verbleibende Arbeit.

Nutze bei Bedarf .codex/context/handoff-template.md für eine ausführliche
Aufzeichnung unter .codex/audit/sessions/. Nenne Quellpfade, Symbole und
den überprüften Commit. Schreibe den gepflegten Kontext dieser Variante auf Deutsch.

Behaupte keine Prüfung, Veröffentlichung oder Speicherung, die du nicht tatsächlich
durchgeführt und überprüft hast. Halte ungeprüfte Punkte sichtbar.
Benenne die nächste konkrete Aktion und die dafür zuerst zu lesenden Dateien.
Führe die passende abschließende Konsistenzprüfung des Kontexts aus.
```
