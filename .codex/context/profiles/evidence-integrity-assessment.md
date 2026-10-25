# Bewertung der Integrität von Nachweisen und Aussagen

## Anlass

Nutze dieses Profil, wenn der Nutzer fragt, ob bestehende Dokumentation, Testberichte, Abdeckung, Scannerergebnisse oder Laufzeitaussagen vertrauenswürdig sind. Aufgabe ist die Bewertung von Nachweisen, nicht die Annahme, dass ein neuer vollständiger Scan erforderlich ist.

## Zu lesen

[Prüfung der Nachweisintegrität](../security/checks/evidence-integrity.md), [Quellenherkunft](../provenance.md), [bekannte Quellwidersprüche](../fact-check.md) und die relevanten [Komponentenleitfäden](../security/components/README.md).

## Abgleichen

Ordne Aussagen Arbeits- und Upstream-Revisionen, gegebenenfalls Submodulbindungen, tatsächlichen Binärdateien, wirksamen Regeln und Einstellungen, Funktionsmakros, Abhängigkeits- und Werkzeugversionen, Fallkennungen, Befehl und Arbeitsverzeichnis, Zeitstempeln beziehungsweise Laufkennungen, soweit verfügbar, sowie unverarbeiteten Beobachtungen zu.

Suche nach veralteten oder vermischten Ergebnissen, nicht ausgeführten Fällen, als Laufzeitnachweise ausgegebenen Quellen- oder Build-Nachweisen, verschluckten Fehlern, unbelegter Interpretation von Exit-Codes und generierten Berichten ohne nachvollziehbare Eingabe.

Ein fehlendes Artefakt beweist keinen Produktfehler. Ein vorhandenes Artefakt beweist keine frische Ausführung. Unterscheide nicht verfügbare Nachweise, widerlegte Aussagen und Nachweise, die nur für eine engere Schlussfolgerung ausreichen.

## Ergebnis

Halte für jede wesentliche Aussage den ursprünglichen Wortlaut, stützende Nachweise, die verifizierte Grenze, Abweichung beziehungsweise Lücke und korrigierte Schlussfolgerung fest. Schreibe historische Ergebnisse nicht so um, als seien sie neue Durchläufe.
