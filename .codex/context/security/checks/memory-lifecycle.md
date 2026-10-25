# Fragen zu Speicher, Eigentümerschaft und Lebenszyklus

Verfolge für jedes vom ausgewählten Pfad berührte Objekt Erzeugung, Speicherung, Ausleihen, Eigentumsübertragung, Freigabe und Callback-Nutzung. Lies Deklarationen, Implementierungen und tatsächliche Aufrufer gemeinsam.

- Überdauern Engine- und Regelwerkobjekte die Transaktionen, die ihre Zeiger ausleihen?
- Sind Puffer für ihre expliziten Längen beziehungsweise erforderliche Nullterminierung gültig?
- Passt die Freigabe zur Allokation, einschließlich `new`/`delete`, `strdup`/`free`, intelligenter Zeiger und API-Freigabehilfen?
- Kann ein Fehler oder eine vorzeitige Rückgabe die Freigabe auslassen, wiederholen oder einen aufbewahrten Verweis auf nicht mehr gültigen Speicher hinterlassen?
- Bleiben Callback-Daten während des Aufrufs gültig, und wird danach eine Kopie benötigt?
- Kann ein Überlauf, eine verengende Konvertierung, ein Wechsel zwischen vorzeichenbehafteten und vorzeichenlosen Typen oder ein veralteter Bereich eine Allokation oder einen Zugriff verändern?

Leite Eigentümerschaft nicht allein aus `const` oder einem rohen Zeigertyp ab. Bewerte vermutete Auswirkungen unter tatsächlich unterstütztem Aufruferverhalten und halte den zulässigen Kontrollfall fest.

Einstiegspunkte: [API-Grenze](../components/api-boundary.md), [C++-Leitfaden](../../cpp-guide.md) und [Fehler und Wiederherstellung](runtime-failure-recovery.md). Sanitizer-Nutzung ist optional und gesondert zu belegen.
