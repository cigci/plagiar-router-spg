#pragma once
#include <Arduino.h>

/*
Datei: Util.h
Zweck:
  Diese Datei enthält kleine Hilfsfunktionen (Utilities) und Makros,
  die an mehreren Stellen im Projekt genutzt werden.
  Zum Beispiel: Wartezeiten (delay) oder Zufallszahlen.

Autor: Umut Ali Cigci
Datum: 18. Oktober 2025
Version: 0.0.4

Hinweise:
  - "static inline" wird verwendet, damit Funktionen wie delayDebug()
    direkt im Header stehen dürfen, ohne Linkerfehler zu verursachen.
  - Jede Übersetzungseinheit (jede .cpp-Datei) bekommt dabei
    ihre eigene kleine Kopie.
*/

// Wenn ENABLE_BOOT_DELAYS = 1, dann sind Boot-Verzögerungen aktiv.
// Wenn = 0, werden sie übersprungen (z. B. für Tests oder schnelleren Upload).
#ifndef ENABLE_BOOT_DELAYS
#define ENABLE_BOOT_DELAYS 1
#endif

// delayDebug()
// --------------
// Wartet eine bestimmte Zeit in Millisekunden (ms).
// Wenn Boot-Delays deaktiviert sind, passiert nichts.
// Das ist praktisch, um Animationen ein- oder auszuschalten,
// ohne den Code zu ändern.
static inline void delayDebug(uint16_t ms)
{
#if ENABLE_BOOT_DELAYS
    delay(ms); // normale Wartezeit in Millisekunden
#else
    (void)ms; // macht nichts (Parameter wird ignoriert)
#endif
}

// getRandomRangeInt()
// --------------------
// Gibt eine Zufallszahl im Bereich [minValue, maxValue] zurück.
// Wird oft für Animationen oder Zufallstests verwendet.
int getRandomRangeInt(int minValue, int maxValue);

// ---------------------------------------------------------------------------
// Beschreibung:
//   Diese Datei fasst einfache, oft genutzte Hilfsfunktionen zusammen.
//   Sie ist bewusst klein gehalten, damit sie leicht wiederverwendet
//   und in mehrere Dateien eingebunden werden kann.
// ---------------------------------------------------------------------------
