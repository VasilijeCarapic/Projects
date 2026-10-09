/**
 ******************************************************************************
 * @file    ServerParamsParser.h
 *
 * @brief   Parses "info:mode=..;ssid=..;pass=..;..." messages and applies
 * 			the extracted values to a ServerConfig instance.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef SERVER_PARAMS_PARSER_H
#define SERVER_PARAMS_PARSER_H

#include <Arduino.h>
#include "Networking/Servers/ServerConfig.h"
#include "Parsers/KeyValueParser.h"

/**
 * @class	ServerParamsParser
 * @brief	Parses "info:mode=..;ssid=..;pass=..;..." messages and applies
 * 			the extracted values to a ::ServerConfig instance.
 */
class ServerParamsParser {
public:
    /**
     * @brief	Stores the ServerConfig instance that apply() will update.
     * @param	config: shared WiFi/server configuration to update.
     */
    explicit ServerParamsParser(ServerConfig* config) : config_(config) {}

    /**
     * @brief	Parses an "info:key=val;..." message and applies the
     * 			extracted values to the ServerConfig instance passed at
     * 			construction. Any key not recognized is ignored. Ports
     * 			default to 80 (HTTP) and 8080 (TCP) if not present.
     * @param	msg: raw message starting with "info:".
     */
    void apply(String msg);

private:
    ServerConfig* config_;	/*!< Server configuration updated by apply() */
    KeyValueParser kv_;		/*!< Reused for parsing the "info:" pair list */
};

#endif