/*
Datei: main.cpp
Zweck:
  Startpunkt des Arduino-Programms. Initialisiert die serielle
  Schnittstelle und startet die CLI-Hauptschleife.
  `setup()` läuft einmalig, `loop()` läuft wiederholt.
Autor: Umut Ali Cigci
Datum: 18. Oktober 2025
Version: 0.0.4
*/

#include <Arduino.h>
#include "Router.h"
#include "CommandParser.h"

// Globale Objekte:
// - router: hält den Gerätezustand und zeigt Ausgaben an
// - parser: liest Zeichen von Serial und wertet Befehle aus
Router router;
CommandParser parser(router);

void setup()
{
  // Serielle Schnittstelle starten (9600 Baud).
  // Kurze Pause, damit sich das Terminal verbinden kann.
  Serial.begin(9600);
  delay(500);

  // Boot-Meldungen ausgeben und Startzustand setzen.
  router.boot();
}

void loop()
{
  // Laufend prüfen, ob neue Zeichen ankommen.
  // Der Parser sammelt Zeichen, erkennt ENTER (CR/LF/CRLF)
  // und verarbeitet fertige Eingabezeilen.
  parser.listen();
}
