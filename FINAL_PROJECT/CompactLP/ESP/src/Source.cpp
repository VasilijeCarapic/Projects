/**
 ******************************************************************************
 * @file    Source.cpp
 *
 * @brief   Main entry point of the ESP8266 middleware firmware. Wires
 * 			together the ServerConfig, CommandParser, Servers,
 * 			ConsoleReader and CmdDispatcher objects, brings up the default
 * 			WiFi configuration and web/TCP servers, synchronizes time via
 * 			NTP, and drives the main loop that services the servers and
 * 			dispatches commands received from the STM32 over Serial.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */

/* includes */
#include <Arduino.h>
#include "Networking/NetworkLayer.h"
#include "Parsers/CommandParser.h"
#include "Networking/Servers/ServerConfig.h"
#include "Common/Constants.h"
#include "Controllers/CommandDispatcher.h"
#include "Readers/ConsoleReader.h"
#include "Readers/BinaryFrameReader.h"

/*config parameters*/
ServerConfig config;					/*!< Active WiFi/server configuration (mode, SSID, ports, ...) */
CommandParser cmdParser(&config);		/*!< Parses lines from the STM32 and can update config via "info:" messages */
NetworkLayer  netLayer(&config, &cmdParser); /*!< Owns the HTTP log server and the GUI<->STM32 TCP passthrough link */
ConsoleReader consoleReader;			/*!< Non-blocking buffered reader for lines coming from the STM32 over Serial */
CmdDispatcher dispatcher(&netLayer, &cmdParser, &consoleReader); /*!< Chooses what to execute based on a given command  */

/**
 * @brief	Arduino setup(): runs once at boot.
 *
 * 			- Initializes Serial (UART link to the STM32) at BAUD_RATE.
 * 			- Sets the built-in LED pin as output and turns it off (HIGH).
 * 			- Applies a default configuration (STA mode, default
 * 			  SSID/password, default AP credentials/IP, default HTTP/TCP
 * 			  ports) to the ServerConfig instance.
 * 			- Brings up WiFi and the HTTP/TCP servers via
 * 			  Servers::applyConfig().
 * 			- Starts NTP time sync (pool.ntp.org / time.nist.gov) and sets
 * 			  the local timezone to Central European Time (CET/CEST) so
 * 			  that MsgLogger timestamps are correct.
 */
void setup()
{
    Serial.setRxBufferSize(2048); 

    Serial.begin(BAUD_RATE);
    delay(20);

    /*set initial diode state*/
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    /*default configuration*/
    config.setParameters("STA", DEFAULT_SSID, DEFAULT_PASS, "ESP_AP", "12345678", "192.168.4.1",
                          DEFAULT_HTTP_PORT, DEFAULT_TCP_PORT);

    // start system
    netLayer.applyConfig();

    // time setup
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    tzset();
}

/**
 * @brief	Arduino loop(): runs repeatedly for as long as the board is on.
 *
 * 			- Servers::handleServers(): services the HTTP server (log page)
 * 			  and the TCP server (GUI<->STM32 passthrough).
 * 			- CmdDispatcher::decide(): reads every available Serial byte
 * 			  exactly once and routes it to the ASCII or binary reader
 * 			  based on shared mode state, then dispatches the resulting
 * 			  action (forward to GUI, log, restart servers, clear log,
 * 			  device command, or forward a completed binary stream frame
 * 			  to the matching UDP link).
 */
void loop()
{
    // handles HTTP server, TCP server (GUI<->STM32 passthrough)
    netLayer.handleServers();

    // single byte-driven dispatch: ASCII control lines + binary stream frames
    dispatcher.decide();
}