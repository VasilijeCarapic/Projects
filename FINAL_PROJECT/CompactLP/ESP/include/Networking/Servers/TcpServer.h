/**
 ******************************************************************************
 * @file    Networking/Servers/TcpServer.h
 *
 * @brief   Owns the raw TCP passthrough server used by the OpenEPT GUI to
 * 			talk to the STM32 through the ESP8266. Accepts a single GUI
 * 			client at a time and relays data both ways (GUI -> Serial,
 * 			Serial -> GUI via sendToGui()).
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <WiFi.h>
#include "Networking/Servers/ServerConfig.h"
#include "Readers/ConsoleReader.h"


class CommandParser;
class LinkManager;

/**
 * @class	TcpServer
 * @brief	Manages the GUI<->STM32 raw TCP passthrough link: accepting a
 * 			single GUI client, forwarding GUI->STM32 data over Serial, and
 * 			forwarding STM32->GUI data (via sendToGui()) to the connected
 * 			client.
 */
class TcpServer {
public:
    /**
     * @brief	Stores the shared config dependency. The underlying
     * 			WiFiServer is not created here; call startTcpServer() to
     * 			bring it up.
     * @param	config: shared WiFi/server configuration (used for the TCP port).
     */
    TcpServer(ServerConfig* config);

    /** @brief Attaches parser/link-manager so GUI-originated stream/eplink/slink commands can be handled inline. */
    void attachLinkHandling(CommandParser* parser, LinkManager* links);

    /** @brief Stops and destroys the TCP server, if running. */
    void stopTcpServer();

    /** @brief (Re)creates and starts the TCP passthrough server on the configured port. */
    void startTcpServer();

    /**
     * @brief	Handles a single tcp tick: accepts a new GUI client if one
     * 			is pending, and forwards any data from an already-connected
     * 			GUI client to the STM32 over Serial.
     */
    void handleTcpServer();

    /**
     * @brief	STM32 -> ESP -> GUI: sends a line to the currently connected
     * 			GUI tcp client, if any.
     * @param	msg: message to forward to the GUI (a "\\r\\n" terminator is appended).
     */
    void sendToGui(const String& msg);


private:
    ServerConfig* config_;			/*!< Shared WiFi/server configuration (used for the TCP port) */

    WiFiServer* tcpServer_;			/*!< TCP server instance, created/destroyed by start/stopTcpServer() */
    WiFiClient tcpClient_;			/*!< Currently connected GUI TCP client (only one supported at a time) */

    CommandParser* cmdParser_ = nullptr; /*!< Pointer to command parser for GUI link commands */
    LinkManager* links_ = nullptr;       /*!< Pointer to link manager handling active links */

    ConsoleReader guiReader_;		/*!< Reserved line buffer for the GUI TCP stream (kept separate from the Serial buffer) */

    /** @brief Accepts new GUI tcp client, rejects extra ones. */
    void handleGuiTcpConnection();

    /** @brief GUI -> ESP -> STM32, raw passthrough. */
    void forwardGuiToStm();

    /** @brief removes device and tells us if the link command is found */
    bool stripDevice(String& lowerWithoutDevice, String& withoutDevice, const String& lower, const String& command); 
};

#endif