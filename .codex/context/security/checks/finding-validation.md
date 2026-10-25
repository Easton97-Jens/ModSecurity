# Fragen zur Kandidatenvalidierung und Auswirkung

Ein verdächtiges Muster, ein Treffer zu einem Sicherheitshinweis, eine Scannermeldung oder ein KI-Vorschlag beginnt als Kandidat.

Kläre vor der Bezeichnung als bestätigter Befund die kontrollierte Eingabe und Berechtigungen, den erreichbaren Quellpfad, die beabsichtigte Invariante, die fehlende beziehungsweise defekte Kontrolle, die betroffene Zielstelle, beobachtbare Folge, relevante Schutzprüfungen und stärkste Gegenbelege. Nenne genaue Revision, Datei und Symbol.

Wähle einen zulässigen Kontrollfall an derselben Grenze und, soweit hilfreich, eine relevante alternative Eingabe oder einen alternativen Pfad. Bevorzuge vorhandene gezielte Fälle oder ein begrenztes Testgerüst, das zur tatsächlichen API passt. Ändere keinen Produktionscode allein, um eine Hypothese reproduzierbar zu machen.

Unterscheide quellcodegestützte Begründung von ausgeführter Reproduktion. Wurde die Laufzeitvalidierung nicht durchgeführt oder ist sie nicht eindeutig, benenne diese Grenze ausdrücklich. Eine Testdefinition ist kein Testergebnis.

Fasse gemeinsame Grundursachen zusammen und erhalte dabei unabhängig erreichbare Vorkommen, deren Erreichbarkeit oder Korrektur sich unterscheidet. Benenne externe Zuständigkeitsgrenzen, ohne eine Änderungsfreigabe dafür anzunehmen.

Nutze die [Nachweisintegrität](evidence-integrity.md) für die Ergebniszuordnung. Diese Fragen beschreiben eine Methode; sie behaupten keine vorhandene Sicherheitslücke.
