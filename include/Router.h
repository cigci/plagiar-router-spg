/*
Datei: Router.h
Zweck:
    Diese Datei beschreibt (deklariert) die Klasse "Router".
    Sie simuliert den Zustand und die Oberfläche (CLI) eines Routers,
    also z. B. die Modi, den Hostnamen und das Passwort.
    Außerdem enthält sie alle Funktionsnamen (Prototypen), die in
    Router.cpp später umgesetzt werden.
Autor: Umut Ali Cigci
Datum: 18. Oktober 2025
Version: 0.0.4

Hinweise:
    - Dies ist nur die Beschreibung (Header). Der eigentliche Code steht
      in src/Router.cpp.
    - Feste Texte liegen in PROGMEM, damit weniger RAM verbraucht wird.
*/

#ifndef ROUTER_H
#define ROUTER_H

#include <Arduino.h>
#include <avr/pgmspace.h>

// Variablen von avr-libc, um die RAM-Größe berechnen zu können
extern int __heap_start, *__brkval;

// ---------------------------------------------------------------------------
// RouterMode (Aufzählung der Betriebsmodi)
// ---------------------------------------------------------------------------
// "enum class" wird verwendet, um Namen sauber zu kapseln
// (z. B. RouterMode::UserExec statt nur UserExec).
// Außerdem spart uint8_t Speicher auf kleinen Mikrocontrollern.
enum class RouterMode : uint8_t
{
    BootDialog,     // Startdialog nach dem Einschalten
    UserExec,       // Einfacher Benutzer-Modus (Prompt ">")
    PrivilegedExec, // Erweiteter Modus mit Admin-Rechten ("#")
    GlobalConfig,   // Globaler Konfigurationsmodus ("(config)#")
    InterfaceConfig // Interface-Konfigurationsmodus (nicht aktiv genutzt)
};

// ---------------------------------------------------------------------------
// Feste Längenbegrenzungen (Speicherschutz)
// ---------------------------------------------------------------------------
// +1, weil am Ende jedes Textes ein Nullzeichen ('\0') steht.
static const uint8_t HOSTNAME_MAX = 16;
static const uint8_t SECRET_MAX = 16;

// ---------------------------------------------------------------------------
// Die Router-Klasse
// ---------------------------------------------------------------------------
// Diese Klasse verwaltet:
// - den aktuellen CLI-Modus
// - Hostname und Passwort
// - Textausgaben (Prompt, Hilfe, Version, Config)
// - kleine Dienstfunktionen (Speicheranzeige, Moduswechsel)
class Router
{
public:
    Router(); // Konstruktor: setzt Startwerte

    // ===== Lebenszyklus =====
    // Startet den "Boot"-Text und fragt, ob der Benutzer konfigurieren will.
    void boot();

    // ===== Modi und Befehle =====
    void enable();                   // Wechsel in den privilegierten Modus
    void exitMode();                 // Einen Modus zurückspringen
    void configureTerminal();        // Wechsel in den Konfigurationsmodus
    void showVersion();              // Software-Version anzeigen
    void showRunningConfig();        // Zeigt eine "Fake"-Konfiguration (ASCII)
    void printHelp(RouterMode mode); // Hilfe für den aktuellen Modus
    void printPrompt();              // CLI-Eingabezeichen (z. B. "> " oder "#")
    void printMemoryUsage();         // RAM-Nutzung anzeigen
    void endFunc();                  // Konfig-Modus beenden und zurück zu UserExec

    // ===== Konfiguration (Hostname & Passwort) =====
    bool setHostname(const char *name); // hostname <name>
    const char *getHostname() const { return hostname; }

    bool setEnableSecret(const char *pass);                    // enable secret <pass>
    bool hasEnableSecret() const { return secret[0] != '\0'; } // Prüft, ob Passwort existiert
    bool verifyEnableSecret(const char *pass) const;           // Vergleicht eingegebenes Passwort
    void printPasswordPrompt() const { Serial.print(F("Password: ")); }

    // ===== Modusverwaltung =====
    RouterMode getMode() const { return mode; } // Aktuellen Modus abfragen
    void setMode(RouterMode m) { mode = m; }    // Modus manuell setzen

private:
    // ===== Interne Daten =====
    char hostname[HOSTNAME_MAX + 1];          // Routername (z. B. "Router1")
    char secret[SECRET_MAX + 1];              // Passwort für "enable"
    RouterMode mode = RouterMode::BootDialog; // Start im Boot-Dialog

    // Versions-Text im Flash-Speicher (PROGMEM)
    // (liegt nicht im RAM und spart Platz)
    static const char version_P[] PROGMEM;
};

// ---------------------------------------------------------------------------
// Globale Hilfsfunktion (nicht Teil der Klasse)
// ---------------------------------------------------------------------------
// Liefert den aktuell freien RAM in Bytes zurück.
// Die Umsetzung steht in Router.cpp.
int freeMemory();

#endif
