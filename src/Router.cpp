/*
Datei: Router.cpp
Zweck:
    Diese Datei enthält die Funktionen der Klasse "Router".
    Sie steuert, wie sich das CLI (Kommandozeilen-Interface) verhält,
    merkt sich den aktuellen Modus (User, Privileged, Config, ...),
    und verwaltet Name, Passwort und Ausgaben.
    Viele Texte liegen im Flash (PROGMEM/F()), damit der knappe RAM
    des Arduino Uno geschont wird.
Autor: Umut Ali Cigci
Datum: 18. Oktober 2025
Version: 0.0.4
*/

#include "Router.h"
#include "Util.h"
#include "Compat.h"
#include <string.h>

// Versions-Text im Flash-Speicher (PROGMEM).
// So wird kein Arbeitsspeicher (RAM) verbraucht.
const char Router::version_P[] PROGMEM = "0.0.4";

Router::Router()
{
    // Standard-Hostname in PROGMEM gespeichert (z. B. "PlagiarRouter").
    // strncpy_P kopiert den Text direkt aus dem Flash in den Arbeitsspeicher.
    strncpy_P(hostname, PSTR("PlagiarRouter"), HOSTNAME_MAX);
    hostname[HOSTNAME_MAX] = '\0'; // Sicherheitsende setzen
    secret[0] = '\0';              // Kein Passwort gesetzt
}

void Router::boot()
{
    // Boot-Texte: Diese simulieren ein echtes Systemstart-Verhalten
    // (rein zu Lernzwecken, kein echtes Boot).
    Serial.print(F("\nSystem Bootstrap, Version "));
    Serial.print(FPSTR(version_P));
    Serial.println(F(" (FakeIOS) RELEASE SOFTWARE"));

    Serial.println(F("PlagiarRouter (CUNO-2025) platform with 32 Kbytes of memory"));
    Serial.println(F("Main memory configured with ECC disabled"));
    delayDebug(800); // kleine Pausen für realistischeren Effekt

    Serial.println(F("\nReadonly ROMMON initialized"));
    delayDebug(600);
    Serial.println(F("\nProgram load complete, entry point: 0x0010A000, size: 0x00A340"));
    delayDebug(600);

    // Fake-"Ladebalken"
    Serial.print(F("\nSelf decompressing the image : "));
    for (uint8_t i = 0; i < (uint8_t)getRandomRangeInt(70, 100); i++)
    {
        Serial.write('#'); // direktes Zeichen -> kein RAM für String
        delayDebug(30);
    }
    Serial.println(F(" [OK]"));
    delayDebug(1000);

    Serial.println(F("\nSmart Init is enabled"));
    delayDebug(400);
    Serial.println(F("smart init is sizing iomem"));
    delayDebug(400);
    Serial.println(F("Using 3 percent iomem. [1Mb/32Mb]"));
    delayDebug(800);

    Serial.println(F("\nPlagiar Systems, Inc."));
    Serial.println(F("Vienna, Austria"));
    delayDebug(600);

    Serial.print(F("\nFakeIOS Software, Version "));
    Serial.print(FPSTR(version_P));
    Serial.println(F("PR, RELEASE BUILD"));
    Serial.println(F("Copyright (c) 2025 by Umut Ali Cigci"));
    delayDebug(800);

    Serial.println(F("\n--- System Configuration Dialog ---"));
    Serial.print(F("Would you like to enter the initial configuration dialog? [yes/no]: "));
}

// ======== Modusfunktionen ========

// Wechsel in den "Privileged EXEC"-Modus (höherer Zugriff)
void Router::enable()
{
    mode = RouterMode::PrivilegedExec;
    Serial.println(F("Entering privileged mode..."));
    printPrompt(); // neues Prompt anzeigen
}

// Wechselt beim Verlassen einen Modus zurück (z. B. config → exec)
void Router::exitMode()
{
    switch (mode)
    {
    case RouterMode::InterfaceConfig:
        mode = RouterMode::GlobalConfig;
        break;
    case RouterMode::GlobalConfig:
        mode = RouterMode::PrivilegedExec;
        break;
    default:
        mode = RouterMode::UserExec;
        break;
    }
}

// Aktiviert den Konfigurationsmodus
void Router::configureTerminal()
{
    if (mode == RouterMode::PrivilegedExec)
    {
        mode = RouterMode::GlobalConfig;
        Serial.println(F("Entering configuration mode..."));
    }
    else
    {
        Serial.println(F("% Must be in privileged mode to configure terminal"));
    }
}

// Zeigt Softwareversion und Copyright
void Router::showVersion()
{
    Serial.print(F("FakeIOS Software, Version "));
    Serial.print(FPSTR(version_P));
    Serial.println(F(" RELEASE BUILD"));
    Serial.println(F("Copyright (c) 2025 by Umut Ali Cigci"));
}

// Zeigt den richtigen Prompt (>) oder (#), je nach Modus.
void Router::printPrompt()
{
    Serial.print(hostname);
    switch (mode)
    {
    case RouterMode::UserExec:
        Serial.print(F("> "));
        break;
    case RouterMode::PrivilegedExec:
        Serial.print(F("# "));
        break;
    case RouterMode::GlobalConfig:
        Serial.print(F("(config)# "));
        break;
    case RouterMode::InterfaceConfig:
        Serial.print(F("(config-if)# "));
        break;
    default:
        Serial.print(F("> "));
        break;
    }
}

// Hilfeanzeige für verschiedene Modi
void Router::printHelp(RouterMode mode)
{
    Serial.println();

    switch (mode)
    {
    case RouterMode::UserExec:
        Serial.println(F("User EXEC commands:"));
        Serial.println(F("  enable                - Enter privileged EXEC mode"));
        Serial.println(F("  show version          - Display system version"));
        Serial.println(F("  help or ?             - Display this help message"));
        break;

    case RouterMode::PrivilegedExec:
        Serial.println(F("Privileged EXEC commands:"));
        Serial.println(F("  configure terminal    - Enter global configuration mode"));
        Serial.println(F("  show running-config   - Display current configuration"));
        Serial.println(F("  exit                  - Exit to user EXEC mode"));
        Serial.println(F("  help or ?             - Display this help message"));
        break;

    case RouterMode::GlobalConfig:
        Serial.println(F("Global Configuration commands:"));
        Serial.println(F("  hostname <name>       - Set the system hostname"));
        Serial.println(F("  enable secret <pass>  - Set the privileged EXEC mode password"));
        Serial.println(F("  exit                  - Return to privileged EXEC mode"));
        Serial.println(F("  help or ?             - Display this help message"));
        break;

    default:
        Serial.println(F("Help is not available in this mode."));
        break;
    }

    Serial.println();
    printPrompt();
}

// Zeigt ein ASCII-Bild und eine kleine „Fake“-Konfiguration an.
void Router::showRunningConfig()
{
    Serial.println(F("        .----.        "));
    Serial.println(F("       /      \\       "));
    Serial.println(F("   ___/  O  O  \\___   "));
    Serial.println(F("  |   \\   __   /   |  "));
    Serial.println(F("   \\___)\\____/(____/  "));
    Serial.println(F("        FakeRouter™   "));
    Serial.println();
    Serial.println(F("! Configuration is imaginary."));
    Serial.println(F("! Life is temporary."));
    Serial.println(F("! Ping yourself to reality."));
    Serial.println();
}

// ======== Speicheranzeige ========

// freeMemory() zeigt den noch verfügbaren Arbeitsspeicher (RAM) an.
// Dabei werden interne AVR-Zeiger (heap_start, __brkval) verwendet.
int freeMemory()
{
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

// Gibt an, wie viel RAM noch frei ist (von 2048 Byte beim Arduino Uno).
void Router::printMemoryUsage()
{
    int freeRam = freeMemory();
    Serial.print(F("Free RAM: "));
    Serial.print(freeRam);
    Serial.print(F(" bytes (used: "));
    Serial.print(2048 - freeRam);
    Serial.println(F(" bytes)"));
}

// ======== Konfigurationsfunktionen (Hostname, Passwort) ========

// Prüft, ob ein Zeichen im Hostnamen erlaubt ist.
static inline bool isHostChar(char c)
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           (c == '-') || (c == '_');
}

// Setzt den Hostnamen (z. B. "Router1")
bool Router::setHostname(const char *name)
{
    // Leerzeichen am Anfang überspringen
    while (*name == ' ' || *name == '\t')
        ++name;

    if (*name == '\0')
    {
        Serial.println(F("% hostname: missing name"));
        return false;
    }

    // Zeichen kopieren und prüfen (nur A-Z, a-z, 0-9, -, _)
    char tmp[HOSTNAME_MAX + 1];
    uint8_t n = 0;
    while (*name && *name != ' ' && *name != '\t' && n < HOSTNAME_MAX)
    {
        if (!isHostChar(*name))
        {
            Serial.println(F("% hostname: invalid character (allowed: A-Z a-z 0-9 - _)"));
            return false;
        }
        tmp[n++] = *name++;
    }

    if (*name && *name != ' ' && *name != '\t')
    {
        Serial.println(F("% hostname: too long (max 16)"));
        return false;
    }

    if (n == 0)
    {
        Serial.println(F("% hostname: empty"));
        return false;
    }

    tmp[n] = '\0';
    memcpy(hostname, tmp, n + 1);
    return true;
}

// Setzt das Passwort für den Privileged-Modus.
bool Router::setEnableSecret(const char *pass)
{
    // Leerzeichen am Anfang überspringen
    while (*pass == ' ' || *pass == '\t')
        ++pass;

    if (*pass == '\0')
    {
        Serial.println(F("% enable secret: missing password"));
        return false;
    }

    // Keine Leerzeichen erlaubt, max. Länge prüfen
    char tmp[SECRET_MAX + 1];
    uint8_t n = 0;
    while (*pass && *pass != ' ' && *pass != '\t' && n < SECRET_MAX)
    {
        tmp[n++] = *pass++;
    }
    if (*pass && *pass != ' ' && *pass != '\t')
    {
        Serial.println(F("% enable secret: too long (max 16)"));
        return false;
    }
    tmp[n] = '\0';
    memcpy(secret, tmp, n + 1);
    return true;
}

// Prüft, ob das eingegebene Passwort stimmt.
bool Router::verifyEnableSecret(const char *pass) const
{
    if (secret[0] == '\0' || pass == nullptr || pass[0] == '\0')
        return false;
    return strcmp(pass, secret) == 0;
}

// ======== Beendet den Config-Modus ========
void Router::endFunc()
{
    mode = RouterMode::UserExec;
}
