/**
 ******************************************************************************
 * @file    Servers.h
 *
 * @brief   Thin coordinator that owns HttpServer, TcpServer, LinkManager and
 * 			NetworkConfig, and wires them together: applyConfig() brings
 * 			everything up, handleServers() services all of them every
 * 			loop() tick.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef SERVERS_H
#define SERVERS_H

#include "Networking/Servers/ServerConfig.h"
#include "Parsers/CommandParser.h"
#include "Networking/Servers/HttpServer.h"
#include "Networking/Servers/TcpServer.h"
#include "Networking/Links/LinkManager.h"
#include "Networking/NetworkConfig.h"


/**
 * @class	NetworkLayer
 * @brief	Coordinates the HTTP server, TCP passthrough server, link pool
 * 			manager and network/WiFi setup. Holds one instance of each and
 * 			delegates to them.
 */
class NetworkLayer {
public:
    NetworkLayer(ServerConfig* config, CommandParser* cmdParser);

    /** @brief Stops/reconfigures/restarts everything: WiFi, mDNS, HTTP, TCP. */
    void applyConfig();

    /** @brief Services HTTP, TCP and link pools. Call every loop(). */
    void handleServers();

    /** @brief Clears the web log. */
    void clearHttpServer();

    /** @brief Appends msg to the web log. */
    void getMsg(const String& msg);

    /** @brief Forwards msg to the connected GUI over TCP. */
    void sendToGui(const String& msg);

    /** @brief Send data to the opened stream. */
    bool sendStreamData(int sid, const char* data, size_t len);

private:
    ServerConfig* config_;
    CommandParser* cmdParser_;

    HttpServer    http_;
    TcpServer     tcp_;
    LinkManager   links_;
    NetworkConfig network_;
};

#endif