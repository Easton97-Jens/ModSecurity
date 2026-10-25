# Fragen zu Konfigurations-, Befehls- und Pfadeingaben

Verfolge den ausgewählten extern beeinflussten Wert von seiner tatsächlichen Quelle zu Parser, Eigenschaft, Befehl, Pfad, Serialisierung oder Build-Schritt. Ermittle, wer ihn setzen kann; Regelautor, CI-Mitwirkender und HTTP-Client haben nicht automatisch dieselben Befugnisse.

- Führt die Erkennung zu einer implementierten Einstellung, einem ausdrücklichen Fehler oder einer leeren Kompatibilitätsaktion?
- Wo werden Anführungszeichen, Maskierungen, Zeilenumbrüche, Steuerzeichen, Optionspräfixe, Ersetzungen oder Pfadbestandteile interpretiert?
- Deckt die Validierung die an der Zielstelle tatsächlich verwendete Darstellung ab?
- Kann ein Rückgriff auf Umgebung oder Suchpfad eine andere Binärdatei, Bibliothek, Regeldatei oder einen anderen Ausgabeort auswählen?
- Ist überhaupt eine Shell-Auswertung beteiligt, oder übergibt die API einen Argumentvektor?

Nutze einen begrenzten zulässigen Wert und einen gezielten Negativwert an derselben Grenze. Anführungszeichen allein belegen nicht das Verhalten des gesamten Pfads. Halte das tatsächliche Parse-, Ausführungs- oder Schreibergebnis und die Berechtigungsannahmen fest.

Einstiegspunkte: [Regeln und Parser](../components/rules-parser.md), [Build und Abhängigkeiten](../components/build-dependencies.md) und [bekannte Quellwidersprüche](../../fact-check.md).
