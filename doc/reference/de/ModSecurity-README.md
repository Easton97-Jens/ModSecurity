# Projekt-README von ModSecurity — historische Referenz

> Vollständige deutsche Übersetzung der README aus dem [offiziellen Produktionscommit `8271c9d3bc0abcbd1189f5414c196c336c959047`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/README.md), die auch im veröffentlichten Baseline-Fork-Commit `2bfb2180d8c0fa35ae7a3f2665e999600607d896` enthalten ist. Relative Bild-/Dateilinks sind an diesen offiziellen Commit gebunden. Die ursprüngliche README folgt vollständig übersetzt; ihre Projektaussagen, Datumsangaben, Beispiel-APIs und Befehle sind historische Quelldokumentation und keine zusätzlichen Schlussfolgerungen der JSON-Tests. Buildpfade und Befehle beziehen sich auf einen **vollständigen ModSecurity-Quellcode-Checkout**, nicht auf dieses eigenständige Begleitrepository. Die Live-Badges unten zeigen aktuelle externe Dienste und nicht das historische Testergebnis. Codeblöcke bleiben wortgetreu im Original.

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/owasp-modsecurity/ModSecurity/8271c9d3bc0abcbd1189f5414c196c336c959047/others/modsec_white_bg.png">
  <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/owasp-modsecurity/ModSecurity/8271c9d3bc0abcbd1189f5414c196c336c959047/others/modsec.png">
  <img src="https://raw.githubusercontent.com/owasp-modsecurity/ModSecurity/8271c9d3bc0abcbd1189f5414c196c336c959047/others/modsec.png" width="50%">
</picture>

![Quality Assurance](https://github.com/owasp-modsecurity/ModSecurity/workflows/Quality%20Assurance/badge.svg)
[![Build Status](https://sonarcloud.io/api/project_badges/measure?project=owasp-modsecurity_ModSecurity&metric=alert_status)](https://sonarcloud.io/dashboard?id=owasp-modsecurity_ModSecurity)
[![](https://sonarcloud.io/api/project_badges/measure?project=owasp-modsecurity_ModSecurity&metric=sqale_rating
)](https://sonarcloud.io/dashboard?id=owasp-modsecurity_ModSecurity)
[![](https://sonarcloud.io/api/project_badges/measure?project=owasp-modsecurity_ModSecurity&metric=reliability_rating
)](https://sonarcloud.io/dashboard?id=owasp-modsecurity_ModSecurity)
[![](https://sonarcloud.io/api/project_badges/measure?project=owasp-modsecurity_ModSecurity&metric=security_rating
)](https://sonarcloud.io/dashboard?id=owasp-modsecurity_ModSecurity)
[![](https://sonarcloud.io/api/project_badges/measure?project=owasp-modsecurity_ModSecurity&metric=vulnerabilities
)](https://sonarcloud.io/dashboard?id=owasp-modsecurity_ModSecurity)



Libmodsecurity ist eine Komponente des Projekts ModSecurity v3. Die Codebasis der
Bibliothek dient als Schnittstelle zu ModSecurity-Connectoren, die Webverkehr
entgegennehmen und die übliche ModSecurity-Verarbeitung anwenden. Grundsätzlich
kann die Bibliothek Regeln im ModSecurity-SecRules-Format laden und interpretieren
und auf HTTP-Inhalte anwenden, die Ihre Anwendung über Connectoren bereitstellt.

Wenn Sie ModSecurity für Apache suchen (auch ModSecurity v2.x genannt), wird es
weiterhin gewartet und ist [hier](https://github.com/owasp-modsecurity/ModSecurity/tree/v2/master) verfügbar.

### Was unterscheidet dieses Projekt vom alten ModSecurity (v2.x.x)?

* Alle Apache-Abhängigkeiten wurden entfernt.
* Höhere Leistung.
* Neue Funktionen.
* Neue Architektur.

Libmodsecurity ist eine vollständige Neuentwicklung der ModSecurity-Plattform.
Das ModSecurity-Projekt begann ursprünglich als reines Apache-Modul. Im Laufe der
Zeit wurde das Projekt auf vielfachen Wunsch um die Unterstützung anderer
Plattformen erweitert, darunter Nginx und IIS, aber nicht darauf beschränkt. Um der
wachsenden Nachfrage nach weiteren unterstützten Plattformen gerecht zu werden,
war es erforderlich, die diesem Projekt zugrunde liegenden Apache-Abhängigkeiten
zu entfernen und es dadurch unabhängiger von der Plattform zu machen.

Mit diesem Ziel wurde die Architektur von Libmodsecurity so überarbeitet, dass
weder beim Übersetzen noch zur Laufzeit eine Abhängigkeit vom Apache-Webserver
besteht. Eine Folge davon ist, dass Benutzer auf allen Plattformen eine höhere
Leistung erwarten können. Außerdem wurde diese Gelegenheit genutzt, die Grundlage
für neue, schon lange gewünschte Funktionen zu schaffen. Beispielsweise ist eine
native Unterstützung von Audit-Logs im JSON-Format vorgesehen, zusammen mit vielen
weiteren Funktionen in künftigen Versionen.

### Es ist nicht mehr nur ein Modul.

Der Branch „ModSecurity“ enthält nicht mehr die bisher gemeinsam ausgelieferte
herkömmliche Modullogik für Nginx, Apache und IIS. Stattdessen enthält dieser Branch
nur den Bibliotheksteil des Projekts (libmodsecurity). Diese Bibliothek wird von
sogenannten „Connectoren“ verwendet: Sie verbinden sich mit Ihrem Webserver und
stellen der Bibliothek Daten in einem gemeinsamen Format bereit, das sie versteht.
Jeder dieser Connectoren wird als separates GitHub-Projekt gepflegt. Der
Nginx-Connector wird beispielsweise vom Projekt ModSecurity-nginx bereitgestellt
(https://github.com/owasp-modsecurity/ModSecurity-nginx).

Durch die Trennung der Connectoren kann jedes Projekt eigene Veröffentlichungszyklen,
Issues und Entwicklungsbäume haben. Außerdem erhalten Sie bei der Installation von
ModSecurity v3 genau das, was Sie benötigen, ohne ungenutzte Zusatzbestandteile.

# Kompilierung

Stellen Sie vor dem Kompilieren sicher, dass alle erforderlichen Abhängigkeiten
installiert sind. Weitere Informationen stehen in den Abschnitten
[Abhängigkeiten](#abhängigkeiten) und [Git-Submodule](#git-submodule).

Vergewissern Sie sich nach dem Kompilieren, dass es keine Probleme mit Ihrem Build
oder Ihrer Plattform gibt. Es wird dringend empfohlen, die Unit- und
Regressionstests auszuführen. Diese Testprogramme befinden sich im Unterverzeichnis
[`tests/`](#patch-testen).

Als dynamische Bibliothek muss `libmodsecurity` an einem Ort installiert werden,
an dem Ihr Betriebssystem dynamische Bibliotheken finden kann.

### Unix (Linux, macOS, FreeBSD, …)

Auf Unix-ähnlichen Systemen verwendet das Projekt Autotools zum Kompilieren.

Wenn Sie mit einem Git-Checkout arbeiten, müssen Sie das Repository vor dem Build
rekursiv klonen oder alle Submodule initialisieren. Siehe auch den Abschnitt
[Git-Submodule](#git-submodule).

```sh
git clone https://github.com/owasp-modsecurity/ModSecurity ModSecurity
cd ModSecurity
````

Dieses Repository verwendet Git-Submodule. Initialisieren und laden Sie nach dem
Klonen alle Submodule:

```sh
git submodule update --init --recursive
```

Mit folgendem Befehl können Sie prüfen, ob alle Submodule korrekt initialisiert sind:

```sh
git submodule status
```

Korrekt initialisierte Submodule zeigen einen Commit-Hash an. Ein vorangestelltes
`-` zeigt an, dass das Submodul nicht initialisiert wurde.

Anschließend können Sie den Build starten:

```sh
./build.sh
./configure
make
sudo make install
```

Einzelheiten zu distributionsspezifischen Builds stehen im Wiki:
[Kompilierungsrezepte](https://github.com/owasp-modsecurity/ModSecurity/wiki/Compilation-recipes).

### Windows

Informationen zum Windows-Build stehen [hier](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/build/win32/README.md).

## Abhängigkeiten

* Diese Bibliothek ist in C++ geschrieben und verwendet den Standard C++17.
* Sie verwendet Flex und Bison (Yacc), um den Parser für die „Sec Rules Language“ zu erzeugen.
* Zu den Pflichtabhängigkeiten gehört YAJL, weil ModSecurity JSON für die Protokollierung und sein Testframework verwendet.
* libXML2 (optional) wird zum Parsen von XML-Requests verwendet.

### Engine für reguläre Ausdrücke (PCRE2 / PCRE)

* Die Verarbeitung regulärer Ausdrücke in SecRules wird über die Hilfsklasse `Regex` (`src/utils/regex.*`) implementiert.
* Standardmäßig verwendet ModSecurity **PCRE2** zur Verarbeitung regulärer Ausdrücke.
* Dies wird von Operatoren wie `@rx`, `@rxGlobal` und `@verifyCC` verwendet.
* Verhalten beim Build:

  * **Standard:** PCRE2 wird erkannt und verwendet.
  * **Alternative:** Das ältere PCRE kann verwendet werden, wenn `--with-pcre` ausdrücklich angegeben wird (`WITH_PCRE`).
* Aktuelle Builds erwarten somit PCRE2, sofern sie nicht ausdrücklich anders konfiguriert werden.

Alle übrigen Abhängigkeiten beziehen sich auf in SecRules angegebene Operatoren
oder auf Konfigurationsdirektiven und sind möglicherweise nicht zum Kompilieren
erforderlich.

### Operatorbezogene Abhängigkeiten

* `libinjection` wird für die Operatoren `@detectXSS` und `@detectSQL` benötigt.
* `curl` wird für die Direktive `SecRemoteRules` benötigt.

Fehlen diese Bibliotheken, wird ModSecurity ohne Unterstützung für die jeweiligen
Operatoren oder Direktiven kompiliert.

### Git-Submodule

Das Repository enthält folgende Submodule:

* `others/libinjection` – wird von den Operatoren `@detectSQLi` und `@detectXSS` verwendet.

* `others/mbedtls` (TF-PSA-Crypto-Teilmenge) – wird für kryptografische Funktionen und Hilfsfunktionen verwendet, etwa Hashing und Base64.

  **Hinweis:** Die neuere Verzeichnisstruktur von mbedTLS v4 ist nicht mit der älteren v3-Struktur kompatibel.
  Der interne Aufbau hat sich erheblich geändert, und viele Komponenten wurden in Submodule verschoben, etwa TF-PSA-Crypto.

  Nach dem Merge von PR #3532 muss folgender Befehl ausgeführt werden:

  ```sh
  git submodule update --init --recursive
  ```

  Dadurch werden alle erforderlichen Submodule geladen. Ohne diesen Schritt kann das Projekt nicht erfolgreich gebaut werden.

  Mit folgendem Befehl können Sie prüfen, ob alle Submodule korrekt initialisiert sind:

  ```sh
  git submodule status
  ```

  Beispielausgabe:

  ```sh
  bc625d5... bindings/python
  2117822... others/libinjection (v4.0.0)
  0fe989b... others/mbedtls (v4.1.0)
  a3d4405... test/test-cases/secrules-language-tests
  ```

  Ein fehlendes Submodul wird mit einem vorangestellten `-` angezeigt, beispielsweise:

  ```sh
  -bc625d5... bindings/python
  ```

  Ein vorangestelltes `-` zeigt an, dass das Submodul nicht initialisiert oder geladen wurde.

* `test/test-cases/secrules-language-tests` – gemeinsame SecRules-Konformitäts- und Regressionstestsuite, die von `make check` verwendet wird.

* `bindings/python` – Python-Anbindungen für ModSecurity (zum Kompilieren der Kernbibliothek nicht erforderlich).

`others/libinjection` und `others/mbedtls` werden für Builds aus dem Quellcode
faktisch benötigt und müssen vor dem Build initialisiert werden.

### Optionale externe Abhängigkeiten

Mehrere externe Bibliotheken sind optional und ermöglichen zusätzliche Funktionen,
darunter:

* `libcurl` – erforderlich für `SecRemoteRules`.

* LMDB – Unterstützung für persistente Speicherung.

* Lua – Unterstützung für Skripte.

* XML-Bibliotheken – erweiterte XML-Verarbeitung.

* **GeoIP (ältere Variante) / MaxMind**

  Die ältere **GeoIP-C-API** (libGeoIP) ist **veraltet und wird von MaxMind nicht mehr gepflegt**.
  Das Upstream-Repository wurde archiviert und sollte für neue Installationen nicht verwendet werden.

  Stattdessen unterstützt ModSecurity die moderne **MaxMind-DB-API (libmaxminddb)**, die aktiv gepflegt wird.

  Bei der Konfiguration kann beispielsweise Folgendes erscheinen:

  ```sh
  + GeoIP/MaxMind                                 ....found
      * (MaxMind) v1.12.2
         -lmaxminddb , -I/usr/include/x86_64-linux-gnu
  ```

  Dies zeigt an, dass **libmaxminddb** verwendet wird (empfohlen).

  Es wird dringend empfohlen, MaxMind DB anstelle der älteren GeoIP-Bibliothek zu verwenden.

# Bibliotheksdokumentation

Die Dokumentation der Bibliothek steht im Code im Doxygen-Format. Verwenden Sie zum
Erzeugen dieser Dokumentation das Programm Doxygen mit der bereitgestellten
Konfigurationsdatei „doxygen.cfg“ im Unterverzeichnis „doc/“. Dadurch entsteht eine
Dokumentation im HTML-Format einschließlich Anwendungsbeispielen.

# Verwendung der Bibliothek

Die Bibliothek bietet eine C++- und eine C-Schnittstelle. Einige Möglichkeiten sind
derzeit nur über die C++-Schnittstelle verfügbar, beispielsweise das Erstellen
eigener Protokollierungsmechanismen; deren Funktionsweise zeigen die
Regressionstests. Ziel ist, dass beide APIs (C und C++) dieselben Funktionen
bereitstellen. Wenn ein Teil der API über eine der Schnittstellen fehlt, öffnen
Sie bitte ein Issue.

Im Unterverzeichnis „examples“ stehen einfache Beispiele zur Verwendung der API.
Einige davon werden im Folgenden gezeigt:

### Einfaches Beispiel mit C++

```c++
using ModSecurity::ModSecurity;
using ModSecurity::Rules;
using ModSecurity::Transaction;

ModSecurity *modsec;
ModSecurity::Rules *rules;

modsec = new ModSecurity();

rules = new Rules();

rules->loadFromUri(rules_file);

Transaction *modsecTransaction = new Transaction(modsec, rules);

modsecTransaction->processConnection("127.0.0.1");
if (modsecTransaction->intervention()) {
   std::cout << "There is an intervention" << std::endl;
}
```

### Einfaches Beispiel mit C

```c
#include "modsecurity/modsecurity.h"
#include "modsecurity/transaction.h"


char main_rule_uri[] = "basic_rules.conf";

int main (int argc, char **argv)
{
    ModSecurity *modsec = NULL;
    Transaction *transaction = NULL;
    Rules *rules = NULL;

    modsec = msc_init();

    rules = msc_create_rules_set();
    msc_rules_add_file(rules, main_rule_uri);

    transaction = msc_new_transaction(modsec, rules);

    msc_process_connection(transaction, "127.0.0.1");
    msc_process_uri(transaction, "http://www.modsecurity.org/test?key1=value1&key2=value2&key3=value3&test=args&test=test");
    msc_process_request_headers(transaction);
    msc_process_request_body(transaction);
    msc_process_response_headers(transaction);
    msc_process_response_body(transaction);

    return 0;
}

```

# Mitwirken

Beiträge zu diesem Projekt sind sehr willkommen. Wir freuen uns auf eine wachsende
Community rund um diese neue ModSecurity-Version. Mögliche Beiträge sind neue
Funktionen, Korrekturen, Fehlerberichte, Unterstützung für Einsteiger oder andere
Bereiche, in denen Sie helfen möchten.

## Patches bereitstellen

Wir bevorzugen Patches innerhalb der GitHub-Infrastruktur, um unsere Review-Arbeit
und die Integration unserer Qualitätssicherung zu erleichtern. GitHub bietet eine
ausführliche Dokumentation zum Erstellen von „Pull Requests“; weitere Informationen:
https://help.github.com/articles/using-pull-requests/

Beachten Sie bitte den Codestil. Pull Requests können mehrere Commits enthalten;
stellen Sie daher pro Commit eine Korrektur oder eine Funktion bereit. Ändern Sie
nichts außerhalb des vorgesehenen Arbeitsumfangs, beispielsweise den Codestil in
einer Funktion, an der Sie lediglich vorbeigekommen sind. Weitere Informationen zum
Codestil des Projekts: https://www.chromium.org/blink/coding-style

Schreiben Sie erklärende Commit-Nachrichten. Die erste Zeile sollte die wichtigsten
Punkte Ihres Patches nennen; ab der dritten Zeile folgen eine ausführlichere
Erklärung und technische Einzelheiten. Eine Erklärung des Patches ist beim Review
hilfreich.

### Sie wissen nicht, wo Sie anfangen sollen?

Im Code stehen verschiedene mit TODO oder FIXME markierte Stellen, die Ihre
Aufmerksamkeit benötigen könnten. Mit folgendem grep-Aufruf finden Sie diese
Einträge:

```
$ cd /path/to/modsecurity-nginx
$ egrep -Rin "TODO|FIXME" -R *
```

Eine TODO-Liste ist auch Bestandteil der Doxygen-Dokumentation.

### Patch testen

Zusätzlich zu manuellen Tests empfehlen wir dringend die Verwendung unserer
Regressions- und Unit-Tests. Wenn Sie einen Operator implementiert haben, vergessen
Sie nicht, Unit-Tests dafür zu erstellen. Bei anderen Implementierungen wird
empfohlen, ergänzende Regressionstests zu entwickeln.

Die Programme für Regressions- und Unit-Tests sind native Programme und benötigen
keine externen Werkzeuge oder Skripte. Allerdings müssen Sie die Testfälle aus
anderen Repositorys laden, weil sie gemeinsam mit anderen ModSecurity-Versionen
verwendet werden; diese Repositorys sind als Git-Submodule eingebunden. Laden Sie
die Submodule und führen Sie die Testprogramme mit folgenden Befehlen aus:

```shell
$ cd /path/to/your/ModSecurity
$ git submodule update --init --recursive
$ make check
 ```

### Fehlersuche

Klären Sie vor Beginn der Fehlersuche, wo sich Ihr Fehler befindet. Das Problem
könnte im Connector oder in libmodsecurity liegen. Zur Eingrenzung wird empfohlen,
einen Regressionstest zu entwickeln, der das Fehlerszenario nachbildet. Lässt sich
der Fehler mit dem Regressionstestprogramm reproduzieren, vereinfacht das die
Fehlersuche erheblich und hilft sicherzustellen, dass er nicht erneut auftritt.
Unter Linux wird empfohlen, bei Bedarf gdb und/oder valgrind zu verwenden.

Während der Konfiguration und Kompilierung können Sie die Compileroptimierung
deaktivieren, damit Ihre Stacktraces lesbare Daten enthalten. Verwenden Sie CFLAGS,
um die Optimierungsparameter für die Kompilierung zu deaktivieren:

```shell
$ export CFLAGS="-g -O0"
$ ./build.sh
$ ./configure --enable-assertions=yes
$ make
$ sudo make install
```

„Assertions ermöglichen es uns, Annahmen zu dokumentieren und Verletzungen früh im
Entwicklungsprozess zu erkennen. Darüber hinaus erlauben Assertions, solche
Verletzungen mit minimalem Aufwand zu erkennen.“
https://dl.acm.org/doi/pdf/10.1145/240964.240969

Es wird empfohlen, Assertions an geeigneten Stellen zu verwenden und sie beim
Testen und bei der Fehlersuche mit '--enable-assertions=yes' zu aktivieren.

### Benchmarking

Der Quellbaum enthält ein Benchmark-Werkzeug, das beim Messen der
Bibliotheksleistung helfen kann. Es liegt im Verzeichnis `test/benchmark/`. Der
Build erzeugt dort auch die ausführbare Datei, sodass das Werkzeug nach Abschluss
der Kompilierung verfügbar ist.

Geben Sie zum Ausführen einfach Folgendes ein:

```shell
cd test/benchmark
$ ./benchmark
Doing 1000000 transactions...

```

Sie können auch einen niedrigeren Wert übergeben:

```shell
$ ./benchmark 1000
Doing 1000 transactions...
```

Zum Messen der Zeit:

```shell
$ time ./benchmark 1000
Doing 1000 transactions...

real	0m0.351s
user	0m0.337s
sys	0m0.022s
```

Dies ist sehr schnell, weil der Benchmark die minimale Konfiguration
`modsecurity.conf.default` verwendet, die nur wenige Regeln enthält:

```shell
$ cat basic_rules.conf

Include "../../modsecurity.conf-recommended"

```

Führen Sie zum Messen mit echten Regeln eines der Downloadskripte im selben
Verzeichnis aus:

```shell
$ ./download-owasp-v3-rules.sh
Cloning into 'owasp-v3'...
remote: Enumerating objects: 33007, done.
remote: Counting objects: 100% (2581/2581), done.
remote: Compressing objects: 100% (907/907), done.
remote: Total 33007 (delta 2151), reused 2004 (delta 1638), pack-reused 30426
Receiving objects: 100% (33007/33007), 9.02 MiB | 16.21 MiB/s, done.
Resolving deltas: 100% (25927/25927), done.
Switched to a new branch 'tag3.0.2'
/path/to/ModSecurity/test/benchmark
Done.

$ cat basic_rules.conf

Include "../../modsecurity.conf-recommended"

Include "owasp-v3/crs-setup.conf.example"
Include "owasp-v3/rules/*.conf"
```

Nun liefert der Befehl einen wesentlich höheren Wert.

#### So funktioniert der Benchmark

Das Werkzeug ist eine einfache Wrapper-Anwendung, die die Bibliothek verwendet.
Es erzeugt eine ModSecurity-Instanz und eine RuleSet-Instanz und führt dann eine
Schleife entsprechend der angegebenen Anzahl aus. Innerhalb dieser Schleife wird
ein Transaction-Objekt erzeugt, um echte HTTP-Transaktionen nachzubilden.

Jede Transaktion ist ein HTTP/1.1-GET-Request mit einigen GET-Parametern. Es werden
übliche Header hinzugefügt, anschließend die Response-Header und ein XML-Body.
Zwischen den Phasen prüft das Werkzeug, ob eine Intervention aufgetreten ist.
Alle Transaktionen werden mit denselben Daten erzeugt.

Beachten Sie, dass das Werkzeug die letzte Phase (Protokollierung) nicht aufruft.

Denken Sie daran, `basic_rules.conf` zurückzusetzen, wenn Sie einen anderen
Regelsatz ausprobieren möchten.

## Probleme melden

Bei einem Konfigurationsproblem oder unerwartetem Verhalten verwenden Sie bitte
die Mailingliste für ModSecurity-Anwender. Issues auf GitHub sind ebenfalls
willkommen; wir bevorzugen jedoch, dass Fragen zunächst auf der Mailingliste
gestellt werden, damit die gesamte Community erreicht wird. Suchen Sie außerdem
nach bestehenden Issues, bevor Sie ein neues öffnen.

Wenn Sie ein neues Issue auf GitHub öffnen, nennen Sie die Version Ihrer
libmodsecurity und, falls vorhanden, die Version des verwendeten Connectors.

### Sicherheitsproblem

Melden Sie Sicherheitsprobleme bitte nicht öffentlich. Öffnen Sie stattdessen einen
[Entwurf eines Security Advisory](https://github.com/owasp-modsecurity/ModSecurity/security/advisories/new)
auf GitHub, damit der Bericht privat bleibt, während die Maintainer ihn prüfen
und eine Korrektur vorbereiten. Das Advisory wird zusammen mit der korrigierten
Version veröffentlicht; die meldenden Personen werden darin genannt.

Wenn Sie GitHub nicht verwenden können, schreiben Sie stattdessen eine E-Mail an
modsecurity@owasp.org.

## Funktionswünsche

Wir sind offen dafür, neue Funktionswünsche mit der Community über die
Mailinglisten zu diskutieren. Alternativ können Sie GitHub-Issues für neue
Funktionen öffnen. Prüfen Sie vor einem neuen Issue, ob zum selben Thema bereits
eines existiert.

## Sprach- und Systemanbindungen

Das Design von libModSecurity erlaubt die Integration über Anbindungen (Bindings).
Es wird darauf geachtet, die binäre API-Kompatibilität nicht zu brechen, um die
Integration möglicher Anbindungen zu erleichtern. Derzeit gibt es einige
bemerkenswerte, von der Community gepflegte Projekte:

* Python - https://github.com/actions-security/pymodsecurity
* Rust - https://github.com/rkrishn7/rust-modsecurity
* Varnish - https://github.com/xdecock/vmod-modsecurity

## Paketierung

Wir wünschen uns, dass unsere Pakete zeitnah in Distributionen verfügbar sind.
Teilen Sie uns daher bitte mit, wenn wir Ihre Arbeit als Paketbetreuer
unterstützen können.

## Hinweis zum Sponsor

Die Entwicklung von ModSecurity wird von Trustwave gesponsert. Das Sponsoring endet
am 1. Juli 2024. Weitere Informationen:
https://www.trustwave.com/en-us/resources/security-resources/software-updates/end-of-sale-and-trustwave-support-for-modsecurity-web-application-firewall/
