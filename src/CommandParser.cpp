/*
Datei: CommandParser.cpp
Zweck:
  Diese Datei liest die eingegebenen Zeichen von der seriellen Schnittstelle (Serial),
  speichert sie in einem festen Textpuffer (char-Array) und erkennt, wenn der Benutzer
  eine ganze Befehlszeile abgeschlossen hat.
  Außerdem werden Backspace (Löschen) und verschiedene Zeilenenden (CR, LF, CRLF)
  richtig behandelt.
  Um RAM zu sparen, werden feste Texte – also Literale – im Flash (PROGMEM) gespeichert.

Autor: Umut Ali Cigci
Datum: 18. Oktober 2025
Version: 0.0.4

Wichtige Begriffe:
  - PROGMEM / F(): Speichert feste Texte im Flash-Speicher, nicht im RAM.
  - __FlashStringHelper: Hilfstyp, damit Serial.print Texte aus Flash lesen kann.
  - reinterpret_cast: Sichere C++-Umwandlung von Zeigern oder Typen.
  - CR/LF-Handling: Behandelt Zeilenenden aus Windows (CRLF), Linux (LF)
    und alten Terminals (CR) gleich, damit ENTER immer gleich funktioniert.
*/

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "CommandParser.h"
#include "Router.h"
#include "Util.h"

// -------- Hilfsfunktionen (nur für diese Datei) --------

// Vergleicht zwei Texte ohne auf Groß-/Kleinschreibung zu achten.
// Beide Texte liegen im normalen RAM.
static inline bool ieq(const char *a, const char *b)
{
    // Diese Schleife vergleicht zwei Texte Buchstabe für Buchstabe,
    // ohne auf Groß- oder Kleinschreibung zu achten.
    for (;;)
    {
        // *a++ bedeutet:
        // 1. Nimm das Zeichen, auf das 'a' gerade zeigt (*a).
        // 2. Danach gehe einen Schritt weiter zum nächsten Zeichen (a++).
        // Dasselbe gilt für *b++.
        //
        // Beispiel:
        // a zeigt auf "HALLO"
        // *a = 'H'
        // a++ -> zeigt danach auf 'A'
        char A = *a++, B = *b++;

        // Wenn der Buchstabe zwischen 'A' und 'Z' liegt,
        // also ein Großbuchstabe ist, +32 ergibt den Kleinbuchstaben.
        // Das funktioniert, weil im ASCII-Code 'A' bis 'Z' = 65–90
        // und 'a' bis 'z' = 97–122 ist.
        if (A >= 'A' && A <= 'Z')
            A += 32;
        if (B >= 'A' && B <= 'Z')
            B += 32;

        // Wenn die Zeichen unterschiedlich sind -> Texte sind nicht gleich.
        if (A != B)
            return false;

        // '\0' ist das Ende eines C-Strings.
        // Wenn beide Texte gleichzeitig hier ankommen, sind sie gleich.
        if (A == '\0')
            return true;
    }
}

// Vergleicht Texte: der erste im RAM, der zweite im Flash-Speicher (PROGMEM)
static inline bool ieq_P(const char *a, const __FlashStringHelper *fb)
{
    // Der Flash-Text wird mit reinterpret_cast zu PGM_P umgewandelt,
    // damit man ihn Byte für Byte mit pgm_read_byte() lesen kann.
    PGM_P pb = reinterpret_cast<PGM_P>(fb);
    for (;;)
    {
        uint8_t A = static_cast<uint8_t>(*a++);
        uint8_t B = pgm_read_byte(pb++);
        if (A >= 'A' && A <= 'Z')
            A += 32;
        if (B >= 'A' && B <= 'Z')
            B += 32;
        if (A != B)
            return false;
        if (A == 0)
            return true;
    }
}

// Kürzere Makros zum Vergleichen von Befehlen (alle Texte liegen im Flash)
#define IEQ1P(s, a) (ieq_P((s), F(a)))
#define IEQ2P(s, a, b) (ieq_P((s), F(a)) || ieq_P((s), F(b)))
#define IEQ3P(s, a, b, c) (ieq_P((s), F(a)) || ieq_P((s), F(b)) || ieq_P((s), F(c)))

// Überspringt Leerzeichen und Tabs am Anfang eines Textes.
static inline const char *skipWS(const char *p)
{
    while (*p == ' ' || *p == '\t')
        ++p;
    return p;
}

// Wenn der Benutzer im normalen Modus (nicht Boot) nur ENTER drückt,
// wird einfach das Prompt neu angezeigt.
static inline bool handleEmptyNonBoot(const RouterMode m, const char *line, Router &router)
{
    if (m != RouterMode::BootDialog && *line == '\0')
    {
        router.printPrompt();
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// CommandParser.cpp (Kurzbeschreibung)
// ---------------------------------------------------------------------------
// - listen(): Liest jedes einzelne Zeichen, erkennt ENTER, CR/LF-Kombinationen
//   und zeigt bei Passwörtern Sternchen statt Zeichen.
// - parse(): Prüft eine komplette Befehlszeile und führt passende Funktionen aus.
//   Texte werden, wenn möglich, aus Flash gelesen (RAM-sparend).
// ---------------------------------------------------------------------------

// ================================================================

CommandParser::CommandParser(Router &r) : router(r)
{
    cmdBuf[0] = '\0';
}

// Liest Zeichen (Bytes) von Serial, speichert sie im Puffer,
// erkennt, wenn ENTER gedrückt wird, und ruft dann parse() auf.
// Diese Funktion läuft in der Hauptschleife (loop) immer wieder.
void CommandParser::listen()
{
    // Solange noch Zeichen im seriellen Eingangs-Puffer sind:
    while (Serial.available())
    {
        // Ein Zeichen lesen
        char c = Serial.read();

        // ===== ENTER (CR, LF oder CRLF) =====
        // Wenn der Benutzer ENTER drückt, sendet das Terminal ein Zeilenende.
        // Je nach Betriebssystem kann das '\r' (Carriage Return),
        // '\n' (Line Feed) oder beides nacheinander (CRLF) sein.
        if (c == '\r' || c == '\n')
        {
            // Wenn es das zweite Zeichen eines CRLF-Paares ist, überspringen.
            // So wird die Eingabe nur einmal verarbeitet.
            if (c == '\n' && lastCR)
            {
                lastCR = false;
                continue;
            }

            // Merken, ob das Zeichen ein '\r' war, um CRLF zu erkennen.
            lastCR = (c == '\r');

            // Zeilenumbruch ausgeben, damit der nächste Text unten steht.
            // Hier kein "\r\n"-Text, um RAM zu sparen (F-Makro nicht nötig).
            Serial.println();

            // Am Ende der Zeile alle Leerzeichen und Tabs löschen.
            // (z. B. wenn jemand "show version   " tippt)
            while (cmdLen && (cmdBuf[cmdLen - 1] == ' ' || cmdBuf[cmdLen - 1] == '\t'))
                cmdLen--;

            // Nullterminator setzen, um den Text als C-String zu beenden.
            cmdBuf[cmdLen] = '\0';

            // Wenn der Benutzer mehr Zeichen eingegeben hat als erlaubt:
            if (bufOverflow)
            {
                Serial.print(F("% Befehl zu lang (max "));
                Serial.print(maxCommandLen());
                Serial.println(F("). Rest wurde abgeschnitten."));
                bufOverflow = false; // Warnung zurücksetzen
            }

            // Jetzt ist eine ganze Zeile fertig → an parse() übergeben.
            parse(cmdBuf);

            // Danach Puffer leeren, um die nächste Eingabe zu starten.
            cmdLen = 0;
            cmdBuf[0] = '\0';

            // Hier "return", damit CRLF nicht doppelt ausgelöst wird.
            return;
        }

        // Wenn das Zeichen kein ENTER war, merken wir uns,
        // dass zuletzt kein CR kam.
        lastCR = false;

        // ===== Backspace/Delete =====
        // Wenn der Benutzer die Rücktaste (Backspace) drückt:
        // ASCII-Code 0x08 oder 0x7F
        if (c == 0x08 || c == 0x7F)
        {
            // Wenn der Puffer nicht leer ist, ein Zeichen löschen.
            if (cmdLen)
            {
                cmdLen--; // ein Zeichen zurückgehen
                // Im Terminal auch das letzte Zeichen löschen:
                // '\b' → Rücksprung, ' ' → überschreiben, '\b' → wieder zurück.
                Serial.write(uint8_t(8));
                Serial.write(' ');
                Serial.write(uint8_t(8));
            }

            // Wenn der Benutzer gelöscht hat, ist kein Überlauf mehr aktiv.
            if (cmdLen < CMD_BUF_LEN - 1)
                bufOverflow = false;
            continue; // nächstes Zeichen lesen
        }

        // ===== Nur druckbare Zeichen anzeigen =====
        // Steuerzeichen (< 0x20) wie TAB, ESC, etc. werden ignoriert.
        if ((uint8_t)c >= 0x20 && c != 0x7F)
        {
            // Begrenzung: Wenn wir gerade ein Passwort eingeben,
            // ist die maximale Länge "SECRET_MAX",
            // sonst die normale Länge für Befehle (CMD_BUF_LEN - 1).
            uint8_t limit = awaitingEnablePassword ? SECRET_MAX : (CMD_BUF_LEN - 1);

            // Wenn der Puffer noch Platz hat:
            if (cmdLen < limit)
            {
                // Zeichen in den Eingabepuffer schreiben.
                cmdBuf[cmdLen++] = c;

                // Wenn wir gerade ein Passwort abfragen,
                // sollen keine Buchstaben sichtbar sein → Stern anzeigen.
                if (awaitingEnablePassword)
                {
                    Serial.write('*');
                }
                else
                {
                    // Normales Echo: zeigt das eingegebene Zeichen im Terminal.
                    Serial.write(c);
                }
            }
            else
            {
                // ===== Zu viele Zeichen eingegeben =====
                if (!bufOverflow)
                {
                    bufOverflow = true; // Merker setzen, damit nur einmal gewarnt wird
                    Serial.write('\a'); // BEL → Piepton (manche Terminals piepen)

                    // Wenn kein Passwort eingegeben wird:
                    if (!awaitingEnablePassword)
                    {
                        Serial.print(F("\r\n% Befehl zu lang (max "));
                        Serial.print(maxCommandLen());
                        Serial.println(F("). Zusätzliche Zeichen ignoriert."));
                        router.printPrompt();         // Prompt neu zeigen
                        Serial.write(cmdBuf, cmdLen); // bisherige Eingabe anzeigen
                    }
                    else
                    {
                        // Wenn es ein Passwort ist:
                        Serial.println(F("\r\n% Passwort zu lang."));
                        // Wir warten weiter auf ENTER,
                        // aber zeigen keine weiteren Zeichen mehr an.
                    }
                }
            }
            continue;
        }

        // ===== Andere Steuerzeichen ignorieren =====
        // (z. B. ESC, TAB, Pfeiltasten-Codes)
    }
}

// ================================================================
// Auswertung einer kompletten Befehlszeile
// ================================================================

void CommandParser::parse(const char *line)
{
    // Wenn wir gerade ein Passwort abfragen:
    if (awaitingEnablePassword)
    {
        awaitingEnablePassword = false; // zurücksetzen
        if (router.verifyEnableSecret(line))
        {
            router.enable(); // korrektes Passwort
        }
        else
        {
            Serial.println(F("% Zugriff verweigert"));
            router.printPrompt();
        }
        return;
    }

    RouterMode m = router.getMode();

    // Leere Eingabe in normalem Modus -> Prompt neu anzeigen.
    if (handleEmptyNonBoot(m, line, router))
        return;

    // ---- Allgemeine Befehle (nicht im Boot-Dialog) ----
    if ((IEQ2P(line, "help", "?")) && m != RouterMode::BootDialog)
    {
        router.printHelp(m);
        return;
    }
    else if (IEQ1P(line, "ram") && m != RouterMode::BootDialog)
    {
        router.printMemoryUsage();
        router.printPrompt();
        return;
    }
    else if (IEQ1P(line, "exit") && (m != RouterMode::BootDialog && m != RouterMode::UserExec))
    {
        router.exitMode();
        router.printPrompt();
        return;
    }
    else if (IEQ1P(line, "end") && (m != RouterMode::BootDialog && m != RouterMode::UserExec))
    {
        router.endFunc();
        router.printPrompt();
        return;
    }

    // ---- Boot-Dialog ----
    if (m == RouterMode::BootDialog)
    {
        if (*line == '\0')
        {
            Serial.println(F("% Bitte antworte mit 'yes' oder 'no'."));
            Serial.print(F("Willst du den Einrichtungsdialog starten? [yes/no]: "));
            return;
        }
        if (IEQ2P(line, "no", "n"))
        {
            Serial.println();
            Serial.println(F("Drücke RETURN, um zu starten!"));
            router.setMode(RouterMode::UserExec);
            router.printPrompt();
            return;
        }
        if (IEQ2P(line, "yes", "y"))
        {
            Serial.println();
            Serial.println(F("Noch nicht implementiert ¯\\_(ツ)_/¯"));
            delayDebug(600);
            Serial.println();
            Serial.println(F("Drücke RETURN, um zu starten!"));
            router.setMode(RouterMode::UserExec);
            router.printPrompt();
            return;
        }
        Serial.println(F("% Bitte antworte mit 'yes' oder 'no'."));
        Serial.print(F("Willst du den Einrichtungsdialog starten? [yes/no]: "));
        return;
    }

    // ---- User EXEC ----
    if (m == RouterMode::UserExec)
    {
        if (IEQ2P(line, "enable", "en"))
        {
            if (router.hasEnableSecret())
            {
                awaitingEnablePassword = true;
                router.printPasswordPrompt(); // zeigt "Password: "
            }
            else
            {
                router.enable(); // kein Passwort nötig
            }
            return;
        }

        if (IEQ2P(line, "show version", "sh ver"))
        {
            router.showVersion();
            router.printPrompt();
            return;
        }

        Serial.println(F("% Ungültige Eingabe"));
        router.printPrompt();
        return;
    }

    // ---- Privileged EXEC ----
    if (m == RouterMode::PrivilegedExec)
    {
        if (IEQ2P(line, "show running-config", "show run"))
        {
            router.showRunningConfig();
            router.printPrompt();
            return;
        }

        if (IEQ2P(line, "configure terminal", "conf t"))
        {
            router.configureTerminal();
            router.printPrompt();
            return;
        }
    }

    // ---- Global Configuration ----
    if (m == RouterMode::GlobalConfig)
    {
        // hostname <name>
        if (strncmp_P(line, PSTR("hostname "), 9) == 0)
        {
            const char *name = skipWS(line + 9);
            if (router.setHostname(name))
                Serial.println(F("Hostname gesetzt."));
            router.printPrompt();
            return;
        }

        // enable secret <pass>
        if (strncmp_P(line, PSTR("enable secret "), 14) == 0)
        {
            const char *pass = skipWS(line + 14);
            if (router.setEnableSecret(pass))
                Serial.println(F("Passwort gesetzt."));
            router.printPrompt();
            return;
        }
    }

    // ---- Standardfall ----
    Serial.println(F("% Ungültige Eingabe"));
    router.printPrompt();
}
