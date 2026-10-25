# Kontextverzeichnis für ModSecurity

Die `AGENTS.md` im Hauptverzeichnis ist eine wiederverwendbare Vereinbarung für beliebige
Repositories. Lies zuerst [repository-context.md](repository-context.md) für Identität,
Quellenvorgaben, Startprüfungen und Dateisprache dieses Checkouts. Lies danach
[current-state.md](current-state.md), [decisions.md](decisions.md) und
[upstream-lock.json](upstream-lock.json), anschließend nur die für die Aufgabe
relevanten Leitfäden und Richtlinien. Die Zuordnung steht in [manifest.toml](manifest.toml).
Diese `.codex`-Dateien bleiben auf den Fork zugeschnitten. Der Branch `codex/kontext-v3-de`
enthält die deutsche Variante; er legt keine Chatsprache fest.

## Technische Leitfäden: das tatsächliche Repository kennenlernen

| Frage | Hauptleitfaden |
| --- | --- |
| Was bietet libmodsecurity? | [Projektkontext](project-context.md) |
| Wo liegen Quellcode, Sprachen, Builds, Tests und Hilfsprogramme? | [Repository-Übersicht](repository-map.md) |
| Wie wirken Transaktionen, Regeln, Phasen und Interventionen zusammen? | [Architektur](architecture.md) |
| Wie funktionieren C++17, Besitzverhältnisse, Callbacks und die C-API hier? | [C++-Leitfaden](cpp-guide.md) |
| Wo wird eine Funktion implementiert und getestet? | [Funktionsübersicht](feature-map.md) |
| Wie hängen SecRules, Konfiguration, Operatoren und Transformationen zusammen? | [Konfiguration und Regeln](configuration-and-rules.md) |
| Wie bereite ich diesen Checkout vor, baue und teste ihn? | [Build und Tests](build-and-test.md) |
| Welche Kommentare, Beispiele oder vermeintlichen Funktionen erfordern Vorsicht? | [Faktenprüfung](fact-check.md) |
| Welche Quellstände und Grenzen stützen die Erklärungen? | [Quellenherkunft](provenance.md) |
| Wie übernehme ich eine neuere Upstream-Revision? | [Upstream-Aktualisierung](upstream-sync.md) |
| Wie öffne ich diese Arbeit im bestehenden Fork? | [Diesen Fork verwenden](using-this-fork.md) |

Die technischen Leitfäden zitieren unveränderliche offizielle ModSecurity-Quellen.
Ein Inventareintrag behauptet nicht, dass jede Funktion geprüft wurde. Lies vor
Änderungen den tatsächlich betroffenen Code.

## Arbeitsrichtlinien: eine primäre Zuständigkeit je Entscheidung

| Aufgabe oder Entscheidung | Primär zuständige Richtlinie |
| --- | --- |
| Anweisungskonflikte, Produktautorität, aktueller Nutzerwunsch | [Rangfolge der Vorgaben](policy-precedence.md) |
| Aufgabenabgrenzung, Anforderungen, Annahmen, angemessene Planung | [Aufgabenablauf](task-workflow.md) |
| Fehlende Voraussetzungen und Machbarkeit | [Machbarkeit](feasibility-policy.md) |
| Kernbibliothek, Connectoren, Abhängigkeiten und Quellzuständigkeit | [Architektur und Grenzen](architecture-and-boundaries.md) |
| Neue Funktion, Fehlerbehebung, Refactoring, Verhaltensänderung | [Änderungsablauf](change-workflow.md) |
| Befehle und tatsächliche Werkzeugverfügbarkeit | [Befehlsausführung](command-execution-policy.md) |
| Optionale RTK-Zusammenfassungen und Codex-Einrichtung | [RTK-Leitfaden](rtk.md), als Ergänzung zur Befehlsrichtlinie |
| Prüfungen, Testauswahl, Ausführungsnachweise | [Tests und Nachweise](testing-and-evidence.md) |
| CPU, Arbeitsspeicher, Build-Ausgaben und Speicherorte | [Ressourcen und Speicher](resources-and-storage.md) |
| Programmiersprachen, vorhandene Build-Dateien, generierte Eingaben | [Werkzeuge und Sprachen](tooling-and-language.md) |
| Projektlokale Python-Hilfsprogramme | [Python-Richtlinie](python-policy.md) |
| Git-Zustand, Branches, Commits, Remotes | [Git-Richtlinie](git-policy.md) |
| Push, PR und beobachtete CI-Ergebnisse | [Bereitstellung und CI](delivery-and-ci.md) |
| Review-Rückmeldungen und fehlgeschlagene CI | [PR-Nachbearbeitung](pr-remediation.md) |
| Integration nach `v3/master` | [Integration in den Zielbranch](master-integration.md) |
| Aufgabenbezogene temporäre Daten und Aufräumen | [Aufräumen und Wiederherstellen](cleanup-and-restoration.md) |
| Deutsche Dokumentation dieser Variante, Quellenangaben und Übergaben | [Dokumentation und Nachvollziehbarkeit](documentation-and-traceability.md) |
| Parallele Arbeit und Hintergrundprozesse | [Unteragenten und Hintergrundarbeit](subagents-and-background.md) |
| Sicherheitsrelevante Aufgaben | [Sicherheitsrichtlinie](security-policy.md) |
| Isolierte Laufzeitvoraussetzungen für eine Sicherheitsaufgabe | [Vorprüfung der Sicherheits-Testumgebung](security-runtime-preflight.md) |
| Eingabegrenzen der Bibliothek und gezielte Härtung | [Protokoll und Härtung](protocol-and-hardening.md) |
| Beobachtungen, bestätigte Befunde und Behebung | [Befundrichtlinie](findings-policy.md) |
| Optionale SonarQube-Nutzung bei tatsächlicher Konfiguration | [SonarQube-Richtlinie](sonarqube-policy.md) |
| Abschluss und ehrlicher Bereitstellungsstand | [Abschlusskriterien](definition-of-done.md) |

## Prüfprofile: nur für die angefragte Untersuchung laden

| Auftrag | Profil |
| --- | --- |
| Gezielte Prüfung des Verhaltens der Kernbibliothek | [Kernbibliothek](profiles/core-library-assessment.md) |
| Funktionale Abdeckung über Kernkomponenten hinweg | [Alle Komponenten](profiles/all-components-assessment.md) |
| Sicherheitsprüfung einer gewählten Komponente | [Komponentensicherheit](profiles/component-security-assessment.md) |
| Sicherheitsprüfung über Kernkomponenten hinweg | [Sicherheit aller Komponenten](profiles/all-components-security-assessment.md) |
| Build, CI, Abhängigkeiten und Lieferkette | [Build und CI](profiles/build-ci-supply-chain-assessment.md) |
| Aussagen, Quellenangaben, Ergebnisse und Nachweisintegrität | [Integrität der Nachweise](profiles/evidence-integrity-assessment.md) |
| Ausdrückliche Prüfung des gesamten Repositorys | [Gesamtes Repository](profiles/full-repository-assessment.md) |
| Antwortphase und Interventionsverhalten | [Antwortphase](profiles/response-phase-assessment.md) |
| Strenge Vollständigkeit für ausdrücklich gewählte Abdeckung | [Ergänzung zur Vollständigkeit](profiles/strict-completeness-assessment.md) |

Das [Profilverzeichnis](profiles/README.md) und das [Sicherheitsverzeichnis](security/README.md)
führen zu [Leitfäden der Kernkomponenten](security/components/README.md) und gezielten
Modulen unter `security/checks/`. Ihr Vorhandensein meldet keine abgeschlossenen Scans
und verpflichtet nicht dazu, für jede Änderung sämtliche Untersuchungen durchzuführen.

## Dauerhaft gespeicherter Sitzungskontext

| Datei | Verwendung |
| --- | --- |
| [Aktueller Stand](current-state.md) | Tatsächlicher Fortschritt, geprüfte Revision, Ergebnisse und nächster Schritt |
| [Entscheidungen](decisions.md) | Dauerhafte Festlegungen mit Gründen und Nachweisen |
| [Backlog](backlog.md) | Tatsächlich verbleibende Aufgaben und Unbekanntes |
| [Sitzungsablauf](session-workflow.md) | Erklären, ändern, prüfen, übergeben |
| [Übergabevorlage](handoff-template.md) | Einen reproduzierbaren Abschluss festhalten |
| [Sitzungsaufzeichnungen](../audit/sessions/README.md) | Datierte Historie außerhalb der normalen Richtlinienzuordnung |
| [Startprompt](prompts/start-new-chat.md) | In einen neuen Chat mit Repository-Zugriff kopieren |
| [Abschlussprompt](prompts/end-session.md) | Mit gespeichertem Kontext abschließen |
| [Vorlagenanpassung](template-adaptation.md) | Was übernommen, angepasst oder bewusst ausgelassen wurde |

Zum ersten Kennenlernen: Projektkontext → Repository-Übersicht → Architektur →
C++-Leitfaden → eine konkrete Implementierung und ihre Tests. Für einen Fehler:
aktueller Stand → Funktionsübersicht → Implementierung und Aufrufer →
ein relevanter Regressionstest → Build-Anleitung.

## Validierung und historisches Material

[bin/README.md](../bin/README.md) beschreibt die lokalen Prüfprogramme,
[audit/README.md](../audit/README.md) die Inventare. Historie belegt ihren angegebenen
Stand und ersetzt keinen aktuellen Quelltext. Die Manifeste werden von diesen
Hilfsprogrammen gelesen; sie sind kein eingebauter Codex-Konfigurationsmechanismus.
