# PlagiarRouter (FakeIOS)

**Autor:** Umut Ali Cigci
**Version:** `0.0.4`
**Plattform:** Arduino UNO , 9600 Baud

## 1) Was ist das?

PlagiarRouter ist ein kleines Lern-Projekt. Es zeigt eine einfache, Cisco-ähnliche Kommando-Zeile (CLI) auf dem Arduino.
Du kannst Befehle eingeben, zwischen Modi wechseln und Einstellungen ändern, wie bei einem echten Router, aber sehr klein und speicherschonend.

## 2) Hauptfunktionen

- **CLI-Modi:** _User Exec_ → _Privileged Exec_ → _Global Config_
- **Wichtige Befehle (mit Kürzeln):**

  - `enable` (`en`) : wechselt in _Privileged Exec_
  - `configure terminal` (`conf t`) : öffnet _Global Config_
  - `hostname <name>` : setzt den Gerätenamen
  - `enable secret <passwort>` : setzt das Admin-Passwort
  - `show version` (`sh ver`) : zeigt die Version
  - `show running-config` (`show run`) : zeigt aktuelle Einstellungen
  - `end`, `exit`, `help` (`?`)
  - optional: `ram` : zeigt Debug-Infos zum Speicher

- **Passwort-Eingabe:** Zeichen werden als `*` angezeigt.
- **Stabile Eingabe:** Sauberes Handling von **CR/LF/CRLF**, Backspace und Puffer-Grenzen.
- **Wenig RAM-Verbrauch:** Text liegt im Flash (PROGMEM), feste `char[]`-Puffer statt `String`.

## 3) Projektstruktur

```text
.
├─ src/
│  ├─ main.cpp              # setup()/loop()
│  ├─ Router.cpp            # Zustandsautomat, Prompts, Befehls-Implementierung
│  ├─ CommandParser.cpp     # Serielles Lesen + Zeilen-Parsing
│  └─ Compat.h              # kleine Kompatibilitäts-Helfer
├─ include/
│  ├─ Router.h
│  ├─ CommandParser.h
│  └─ Util.h                # z.B. delayDebug(), Zufall, kleine Helfer
├─ lib/                     # (optional) externe Libraries
├─ platformio.ini           # Build-Konfiguration (z. B. env:uno)
└─ README.md
```

## 4) Build & Upload (PlatformIO)

Voraussetzung: VS Code + PlatformIO

1. Projekt in VS Code öffnen.
2. In PlatformIO das Environment `env:uno` wählen.
3. **Build:**

   ```bash
   platformio run
   ```

4. **Upload:** (Port anpassen, z. B. `/dev/ttyACM0`, `/dev/ttyUSB0`, `COM3`)

   ```bash
   platformio run --target upload
   ```

5. **Seriellen Monitor** mit **9600 Baud** öffnen.

> Tipp: Wenn Upload scheitert, prüfe USB-Kabel, Port und Board-Auswahl.

## 5) Kurze Bedienungs-Beispiele

```text
Press RETURN to get started!
PlagiarRouter> enable
Entering privileged mode...
PlagiarRouter# conf t
PlagiarRouter(config)# hostname MyRouter
MyRouter(config)# enable secret 1234
MyRouter(config)# end
MyRouter> show version
```

## 6) Technik : einfach erklärt

- **Warum so wenig RAM?**
  Auf dem UNO gibt es nur 2 KB SRAM. Deshalb liegen Texte im **Flash** (Programmspeicher) : mit `F("...")` oder `PROGMEM`.
  Vorteil: Mehr Platz für Variablen und stabile Laufzeit.

- **`F()` und `PROGMEM`:**
  `Serial.print(F("Text"))` hält den String im Flash. Eigene Textblöcke kann man als `const char[] PROGMEM` ablegen.

- **Feste Puffer statt `String`:**
  Wir nutzen `char cmdBuf[...];` mit fester Größe. Das ist vorhersagbar und fragmentiert den Heap nicht.

- **CR/LF/CRLF : was ist das?**

  - **CR** (`\r`): Wagenrücklauf (Carriage Return)
  - **LF** (`\n`): Zeilenvorschub (Line Feed)
  - **CRLF**: erst `\r`, dann `\n` (Windows-Stil)
    Der Parser erkennt alle Varianten und verarbeitet _ENTER_ nur einmal.

- **`Compat.h`:**
  Kleine Hilfen für Kompatibilität und Lesbarkeit (z. B. Makros, Typ-Aliase, Inline-Utils). Inhalt ist bewusst klein gehalten.

## 📘 Technischer Hintergrund

Dieses Projekt wurde in **C++** geschrieben und folgt der typischen Struktur mit
**Header-Dateien (`.h`)** und **Implementierungs-Dateien (`.cpp`)**.  
Dadurch bleibt der Code übersichtlich, wiederverwendbar und leicht zu pflegen.

Eine ausführlichere Erklärung zum Aufbau eines C++-Projekts findest du hier:  
➡️ [C++ Projektstruktur erklärt](docs/C++_Projektaufbau.md)

## Hinweis zur Dokumentation

Teile dieser Dokumentation (Kommentare, Erklärungen, README-Struktur) wurden
unter Verwendung von **ChatGPT (GPT-5, OpenAI)** erstellt.  
Alle Inhalte wurden anschließend von **Umut Ali Cigci** manuell kontrolliert,
sprachlich angepasst und für die finale Version bestätigt.
Dieser Hinweis dient der Nachvollziehbarkeit und Transparenz.
