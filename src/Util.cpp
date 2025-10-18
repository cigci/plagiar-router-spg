#include "Util.h"

/*
Datei: Util.cpp
Zweck:
  Diese Datei enthält kleine Hilfsfunktionen (Utilities),
  die an mehreren Stellen im Projekt verwendet werden.
  Zum Beispiel: Zufallszahlen oder kleine Wartezeiten für Animationen.
Autor: Umut Ali Cigci
Datum: 18. Oktober 2025
Version: 0.0.4
*/

// Gibt eine Zufallszahl im Bereich [minValue, maxValue] zurück.
// Die Arduino-Funktion random(a, b) liefert Werte von a bis b-1 (oberes Ende exklusiv).
// Durch das +1 wird das obere Ende eingeschlossen.
int getRandomRangeInt(int minValue, int maxValue)
{
    return random(minValue, maxValue + 1);
}

// ---------------------------------------------------------------------------
// Beschreibung:
//   Diese Datei enthält reine Hilfsfunktionen ohne Seiteneffekte.
//   Das bedeutet: Sie greifen nicht direkt auf Hardware oder globale Variablen zu.
//   Dadurch kann man sie leicht wiederverwenden.
// ---------------------------------------------------------------------------
