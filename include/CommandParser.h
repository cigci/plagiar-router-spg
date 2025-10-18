/*
Datei: CommandParser.h
Zweck:
    Diese Datei beschreibt (deklariert) die Klasse "CommandParser".
    Sie liest Eingaben über die serielle Schnittstelle (Serial),
    speichert sie in einem festen Textpuffer (char-Array)
    und gibt ganze Befehlszeilen an die Router-Klasse weiter.
    Es wird kein dynamischer Speicher (kein "new" oder "malloc") benutzt,
    damit das Programm auf kleinen Mikrocontrollern stabil bleibt.

Autor: Umut Ali Cigci
Datum: 18. Oktober 2025
Version: 0.0.4
*/

#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <Arduino.h>
#include "Router.h"

// Maximale Länge des Eingabepuffers.
// 40 Zeichen reichen für alle Befehle (längster Befehl ~19 Zeichen).
// Der zusätzliche Platz dient als Sicherheitsabstand (Headroom).
#ifndef CMD_BUF_LEN
#define CMD_BUF_LEN 40
#endif

// Die Klasse CommandParser kümmert sich darum,
// dass die Zeichen von Serial eingelesen, im Puffer gesammelt
// und nach einem ENTER als komplette Zeile an den Router übergeben werden.
class CommandParser
{
public:
    // Konstruktor: bekommt eine Referenz auf den Router,
    // damit er nach dem Parsen den richtigen Befehl ausführen kann.
    explicit CommandParser(Router &r);

    // Liest Zeichen von Serial ein und erkennt, wann eine Zeile fertig ist.
    void listen();

    // Wird aufgerufen, wenn eine ganze Zeile (Befehl) empfangen wurde.
    void parse(const char *line);

    // Gibt die maximale Befehlslänge zurück (ohne Nullzeichen am Ende).
    uint8_t maxCommandLen() const { return CMD_BUF_LEN - 1; }

private:
    Router &router;           // Referenz zum Router-Objekt (Zustand und Befehle)
    char cmdBuf[CMD_BUF_LEN]; // Eingabepuffer für die aktuelle Zeile
    uint8_t cmdLen = 0;       // Aktuelle Länge der Eingabe
    bool lastCR = false;      // Merker: letztes Zeichen war ein Wagenrücklauf (CR)
    bool bufOverflow = false; // Wurde der Puffer überfüllt?

    // Wenn true, dann wird gerade ein Passwort (enable secret) eingelesen.
    bool awaitingEnablePassword = false;
};

#endif

// Anmerkung:
// - CMD_BUF_LEN bestimmt, wie viele Zeichen maximal eingegeben werden dürfen
//   (inklusive des Nullzeichens '\0' am Ende).
// - Es gibt keine dynamische Speicherverwendung. Dadurch läuft das Programm
//   stabil auf kleinen Mikrocontrollern wie dem Arduino Uno (mit nur 2 KB RAM).
