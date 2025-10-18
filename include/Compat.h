#include <Arduino.h>
#include <avr/pgmspace.h>

/*
Datei: Compat.h
Zweck:
  Diese Datei enthält kleine Kompatibilitäts-Hilfen (Makros),
  die das Arbeiten mit Flash-Speicher (PROGMEM) vereinfachen.
  Auf AVR-Mikrocontrollern liegen feste Texte normalerweise im Flash,
  nicht im Arbeitsspeicher (RAM). Damit `Serial.print()` diese Texte
  direkt lesen kann, wird das Makro FPSTR verwendet.

Autor: Umut Ali Cigci
Datum: 18. Oktober 2025
Version: 0.0.4
*/

#ifndef FPSTR
// FPSTR-Makro:
// Wandelt einen Zeiger auf einen PROGMEM-String in einen Zeiger
// vom Typ __FlashStringHelper um. Dadurch kann Serial.print()
// direkt aus dem Flash-Speicher lesen, ohne die Daten vorher
// in den RAM zu kopieren.
//
// Beispiel:
// const char version[] PROGMEM = "1.0.0";
// Serial.print(FPSTR(version));
#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#endif

// ---------------------------------------------------------------------------
// Beschreibung:
//   Dieses Makro sorgt dafür, dass Text, der im Flash-Speicher liegt,
//   richtig von Print-/Serial-Funktionen ausgegeben werden kann.
//   Ohne FPSTR würde Serial.print sonst versuchen, aus dem RAM zu lesen,
//   was auf AVR zu falschen Ausgaben führen würde.
//
//   -> PROGMEM: speichert Texte im Flash (Programmspeicher).
//   -> __FlashStringHelper: spezieller Typ, den Print/Serial versteht.
//   -> reinterpret_cast: sicheres Umwandeln des Zeigers.
// ---------------------------------------------------------------------------
