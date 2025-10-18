# 🧩 Aufbau eines C++-Projekts

C++-Projekte bestehen meist aus zwei Arten von Dateien:

- **Header-Dateien (`.h`)**
- **Implementierungs-Dateien (`.cpp`)**

Diese Trennung sorgt für mehr Übersicht, Ordnung und Wiederverwendbarkeit.

## 🔹 Header-Dateien (`.h`)

Header-Dateien enthalten **Deklarationen** : also Beschreibungen, _was_ im Code existiert.  
Zum Beispiel Klassen, Funktionen, Konstanten oder Strukturen.  
Sie beinhalten keine eigentliche Logik, sondern nur „Ankündigungen“.

**Beispiel:**

```cpp
// Datei: Beispiel.h
class Beispiel {
public:
    void sagHallo(); // nur angekündigt, noch nicht erklärt
};
```

## 🔹 Implementierungs-Dateien (`.cpp`)

Diese Dateien enthalten die **Implementierung** : also den eigentlichen Code, _wie_ etwas funktioniert.
Hier wird das, was in der `.h`-Datei beschrieben wurde, wirklich ausgeführt.

**Beispiel:**

```cpp
// Datei: Beispiel.cpp
#include "Beispiel.h"
#include <Arduino.h>

void Beispiel::sagHallo() {
    Serial.println("Hallo Welt!");
}
```

## 🔹 Warum trennt man das?

| Grund                       | Erklärung                                                                       |
| --------------------------- | ------------------------------------------------------------------------------- |
| **Übersichtlichkeit**       | Große Projekte bleiben klar strukturiert.                                       |
| **Wiederverwendbarkeit**    | Eine Header-Datei kann von mehreren `.cpp`-Dateien genutzt werden.              |
| **Schnelleres Kompilieren** | Nur geänderte `.cpp`-Dateien werden neu gebaut.                                 |
| **Klare Schnittstellen**    | Andere Dateien sehen nur, _was_ sie nutzen dürfen, nicht _wie_ es funktioniert. |

## 🔹 Wie es im PlagiarRouter aussieht

| Datei                  | Aufgabe                                                      |
| ---------------------- | ------------------------------------------------------------ |
| `Router.h`             | Deklariert die Router-Klasse, ihre Funktionen und Variablen. |
| `Router.cpp`           | Enthält die eigentliche Logik, wie der Router reagiert.      |
| `CommandParser.h/.cpp` | Liest Eingaben, analysiert Befehle und leitet sie weiter.    |
| `Util.h/.cpp`          | Kleine Hilfsfunktionen (z. B. Zufallszahlen, Delay).         |
| `Compat.h`             | Kompatibilitätsmakros für PROGMEM und Flash-Strings.         |

## 📘 Fazit

Diese Struktur ist der Standard in C++-Projekten.
Sie macht den Code **sauber, verständlich und erweiterbar** : besonders auf kleinen Geräten wie dem **Arduino UNO**.
