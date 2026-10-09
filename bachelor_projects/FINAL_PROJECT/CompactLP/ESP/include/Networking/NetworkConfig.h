/**
 ******************************************************************************
 * @file    NetworkConfig.h
 *
 * @brief   Handles WiFi mode setup (STA/AP/AP+STA) and mDNS advertisement,
 * 			so the board can be reached as "esp_server.local" once connected.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#include <WiFi.h>
#include "Networking/Servers/ServerConfig.h"

/**
 * @class	NetworkConfig
 * @brief	Brings up WiFi (station/AP/AP+STA) according to a ServerConfig,
 * 			and starts the mDNS responder advertising the HTTP service.
 */
class NetworkConfig {
public:
    /**
     * @brief	Stores the shared config dependency used to decide the WiFi
     * 			mode/credentials and the mDNS-advertised HTTP port.
     * @param	config: shared WiFi/server configuration.
     */
    NetworkConfig(ServerConfig* config);

    /**
     * @brief	Connects the esp8266 to WiFi in station mode, if the
     * 			configured mode requires it. Blocks briefly (polling with
     * 			delay) waiting for a STA connection.
     */
    void setupWiFi();

    /**
     * @brief	Sets in which mode the esp will work (STA / AP / AP+STA),
     * 			configuring and starting the soft AP where applicable.
     */
    void setupMode();

    /**
     * @brief	For STA or STA+AP mode the server becomes reachable via
     * 			http://esp_server.local:port/test.
     * 			Starts the mDNS responder and advertises the HTTP service.
     */
    void setupMDNS();

private:
    ServerConfig* config_;		/*!< Shared WiFi/server configuration (mode, SSID, ports, ...) */
};

#endif