# Optionale Wiederverwendung von Anweisungen

Dieses Verzeichnis entspricht dem ausdrücklichen Vererbungsbereich der Vorlage.
In dieser Einrichtung gibt es **keine aktiven untergeordneten Repositories, die diese
Richtlinien erben**. Inhalte der Upstream-Submodule wurden nicht geändert.

[../inheritance-manifest.toml](../inheritance-manifest.toml) ist der Prüfsummenkatalog
für die lokalen primären Richtlinien. Der Katalog unterstützt überprüfbare Wiederverwendung;
er veranlasst Codex oder ChatGPT nicht, Anweisungen über Repository-Grenzen hinweg zu laden.

[child-inheritance-manifest.toml.example](child-inheritance-manifest.toml.example)
ist ein deaktiviertes Beispiel für ein späteres, ausdrücklich abgegrenztes untergeordnetes
Repository. Lies vor seiner Verwendung dessen eigene Anweisungen, prüfe die tatsächlichen
relativen Pfade, wähle nur anwendbare Richtlinien und ersetze jede Prüfsumme durch den
überprüften Katalogwert. Halte Konflikte und den gewählten übergeordneten Commit fest.
Lege dieses Beispiel nicht als gewöhnlichen Einrichtungsschritt in einer Upstream-Abhängigkeit ab.

Der aktuelle Prüfer kontrolliert nur den aktiven lokalen Katalog dieses Repositorys.
Er prüft oder aktiviert keine hypothetischen untergeordneten Manifeste. Die Unterstützung
eines tatsächlichen untergeordneten Repositorys würde eine separate, überprüfte Änderung
am Schema und am Prüfprogramm erfordern.
