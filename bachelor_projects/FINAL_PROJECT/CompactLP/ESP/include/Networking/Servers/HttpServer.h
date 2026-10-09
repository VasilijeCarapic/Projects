
/**
 ******************************************************************************
 * @file    DateAndTime.h
 *
 * @brief   Displays time and date on Web Server + message whenever new message is accepted.
 *          This was done so that server would essentially be like an log of messages. 
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <WebServer.h>
#include "Networking/Servers/MsgLogger.h"
#include "Networking/Servers/ServerConfig.h"

class HttpServer{
    public: 

        HttpServer(ServerConfig* config); 

        /** @brief Stops and destroys the HTTP server, if running. */
        void stopHttpServer();

        /** @brief (Re)creates and starts the HTTP server, registering the "/test" and "/log" routes. */
        void startHttpServer();

        /** @brief Handles a single http tick (serves pending requests, updates mDNS). */
        void handleHttpServer();

        /** @brief clears http server content */
        void clearHttpServer();   

        /** @brief put msg in MsgLogger */
        void getMsg(const String& msg); 
    private: 
        ServerConfig* config_;			/*!< Shared WiFi/server configuration (mode, SSID, ports, ...) */

        WebServer* server_;	    /*!< HTTP server instance, created/destroyed by start/stopHttpServer() */

        MsgLogger logger;				/*!< In-memory log shown on the "/log" and "/test" pages */

        /** @brief What will be written on the output page (auto-refreshing log view). */
        void handlePage();
        
        
};

#endif