# 🧩 Aufbau eines C++-Projekts

C++-Projekte bestehen meist aus zwei Arten von Dateien:

- **Header-Dateien (`.h`)**
- **Implementierungs-Dateien (`.cpp`)**

Diese Trennung sorgt für mehr Übersicht, Ordnung und Wiederverwendbarkeit.

## 🔹 Header-Dateien (`.h`)

Header-Dateien enthalten **Deklarationen**: also Beschreibungen, _was_ im Code existiert.  
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
