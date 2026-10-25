# Repository-spezifischer Kontext: ModSecurity-Kern

Die [AGENTS.md](../../AGENTS.md) im Hauptverzeichnis ist eine wiederverwendbare
Arbeitsvereinbarung für beliebige Repositories. Diese Datei enthält die konkrete Identität,
Quellenregeln, den Einstieg und technische Vorgaben für diesen Checkout. Halte
projektspezifische Angaben hier oder in den verlinkten Leitfäden fest, damit die
Arbeitsvereinbarung unverändert wiederverwendet werden kann.

## Identität, Sprache und Umfang

| Rolle | Wert für dieses Repository |
| --- | --- |
| Arbeitsrepository | `Easton97-Jens/ModSecurity`, der bestehende öffentliche Fork |
| Offizielle Produktquelle | `owasp-modsecurity/ModSecurity`, Branch `v3/master` |
| Dokumentierter Quellcode-Commit | `effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729` |
| Arbeitsbranch dieser Kontextvariante | `codex/kontext-v3-de` |
| Englische Übersetzungsgrundlage | `codex/kontext-v3`, Commit `a35e8c3a1116a763f286f965f718f36d5f8876cc` |
| Beauftragte Bereitstellung | Deutsche Variante im bestehenden Fork hochladen; keinen Pull Request erstellen und nicht mergen |
| Gepflegte Kontextdateien | Deutsch; tatsächliche Bezeichner und Diagnosemeldungen erhalten |

Prüfe aktuellen Branch, Commit, Arbeitsbaum und Remote-Stand vor Aktionen erneut.
Die Quellmetadaten und die beiden getrennt aufgezeichneten Fork-Beobachtungen stehen in
[upstream-lock.json](upstream-lock.json) und [provenance.md](provenance.md).
[PR #120](https://github.com/Easton97-Jens/ModSecurity/pull/120) gehört zur früheren
englischen Kontextarbeit und ist kein Pull Request für diese deutsche Variante.
Die Dateisprache legt keine Chatsprache fest; jede Person entscheidet selbst.

Die `.codex`-Struktur ist aus dem Connector-Repository des Eigentümers übernommen,
wie in [template-adaptation.md](template-adaptation.md) beschrieben. Dies ist eine
Arbeitskonvention und keine Funktion der Upstream-Bibliothek. Nur die Arbeitsvereinbarung
im Hauptverzeichnis ist zur unveränderten Wiederverwendung über Repositories hinweg gedacht.
Diese konkreten Leitfäden, Manifeste, Quellenbindungen und Hilfsskripte bleiben auf den Kern-Checkout zugeschnitten.

## Einstieg in diesen Checkout

1. Lies die wiederverwendbare Arbeitsvereinbarung und [index.md](index.md).
2. Lies [current-state.md](current-state.md), die relevanten [Entscheidungen](decisions.md)
   und [upstream-lock.json](upstream-lock.json). Unterscheide den Arbeits-Commit
   von der festgehaltenen Produktgrundlage.
3. Prüfe den tatsächlichen Repository-Zustand, soweit Checkout-Zugriff besteht.
   Erhalte sachfremde Arbeit. Lies bei reinem Chat-Zugriff dieselben Dateien über
   den tatsächlich verfügbaren Zugang und benenne benötigte, nicht zugängliche Dateien genau.
4. Lade die zur Aufgabe passenden technischen Leitfäden, Richtlinien und gegebenenfalls Profile.
   Lies betroffene Implementierungen, Deklarationen, Aufrufer, Build-Bedingungen und Tests.
5. Führe geeignete Kontextprüfungen nur aus, wenn ihre Dateien und Voraussetzungen verfügbar sind.
   [bin/README.md](../bin/README.md) beschreibt den vollständigen Prüfungsumfang und seine Grenzen.

Im Hauptverzeichnis dieses Repositorys stehen folgende Kontextprüfungen bereit:

```sh
python3 .codex/bin/verify-context.py
python3 .codex/bin/validate-structure.py
python3 .codex/bin/validate-codex-inheritance.py --check
```

Nutze beim Quellenprüfer `--strict-source`, wenn der Upstream-Produktcode unverändert sein soll.
Er benötigt Python 3 und Git mit Unterstützung für `--no-lazy-fetch`; die TOML-Prüfer
benötigen Python 3.11 oder neuer. Diese Voraussetzungen gehören zu den Hilfsprogrammen,
nicht zur wiederverwendbaren Arbeitsvereinbarung. Leite aus ihren Ergebnissen keinen
erfolgreichen Bibliotheks-Build oder Lauf der Produkttests ab.

## Nachweise und Implementierungsvorgaben für ModSecurity

Leite ModSecurity-Fakten aus tatsächlich gelesenem offiziellen Code der festgehaltenen
Revision oder einer ausdrücklich überprüften neueren offiziellen Revision ab.
Die Connector-Vorlage liefert nur die Struktur. Halte Widersprüche zwischen Code und
Dokumentation in [fact-check.md](fact-check.md) fest. Unterscheide Kernbibliothek,
externe Connectoren, Core Rule Set und v2-/v3-Branches. Die vier Gitlinks kennzeichnen
getrennte Quellen; ihre Interna müssen separat untersucht werden, und Änderungen daran
müssen vom Nutzerauftrag gedeckt sein.

Erhalte für diesen Stand die C++17-Kompatibilität und untersuche C-/C++-Deklarationen
zusammen mit ihren Implementierungen. Beachte `.editorconfig` und den umgebenden Stil.
Prüfe bei Bedarf Objektbesitz und Lebensdauern, Callbacks, ausdrückliche Datenlängen,
Phasenreihenfolge, Interventionsbehandlung, Fehlerpfade und optionale Funktionsschalter.
Setze weder globale Thread-Sicherheit noch vollständige Gleichwertigkeit der C-/C++-Schnittstellen voraus.
Der [C++-Leitfaden](cpp-guide.md) und die [Architektur](architecture.md) liefern
quellenverknüpfte Details; der Build legt den Standard in
[configure.ac](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L63) fest.

Unterscheide `.ll`-/`.yy`-Parser-Eingaben von generierten Dateien. Nutze beim gewöhnlichen
Einrichten die gespeicherten Abhängigkeitsstände. Ermittle tatsächliche Autotools-/Windows-Befehle
und bedingtes Testverhalten in [build-and-test.md](build-and-test.md); die Make-Targets
eines Connectors belegen keine Befehle für diesen Kern-Checkout.

## Bereitstellung und Fortsetzung

Der bestehende Fork bleibt das freigegebene Ziel. Erstelle kein Ersatzrepository.
Beachte [git-policy.md](git-policy.md), [delivery-and-ci.md](delivery-and-ci.md) und
[master-integration.md](master-integration.md) für die jeweiligen Vorgänge.
Der aktuelle Auftrag umfasst ausschließlich Übersetzung und Hochladen des deutschen Branches.
Er umfasst weder einen neuen Pull Request noch einen Merge nach `v3/master`.

Aktualisiere aktuellen Stand, begründete Entscheidungen und den tatsächlichen Backlog,
wenn sich diese ändern. Nutze für eine hilfreiche längere Übergabe
[handoff-template.md](handoff-template.md) und `.codex/audit/sessions/`.
Beachte [definition-of-done.md](definition-of-done.md) und berichte tatsächlich erfolgte Prüfungen.
Portabilität, Dateisprache und die deutsche Variante sind in [decisions.md](decisions.md) festgehalten.
