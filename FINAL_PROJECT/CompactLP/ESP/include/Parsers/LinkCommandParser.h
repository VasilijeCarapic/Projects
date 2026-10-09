/**
 ******************************************************************************
 * @file    LinkCommandParser.h
 *
 * @brief   Parses "stream|eplink|slink create|start|stop|send [args]"
 * 			lines into a ::LinkCommand, and extracts the "-sid=" tag the
 * 			STM32 attaches to its own stream sample lines.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef LINK_COMMAND_PARSER_H
#define LINK_COMMAND_PARSER_H

#include <Arduino.h>
#include "Common/StructTypes.h"
#include "Parsers/KeyValueParser.h"

/**
 * @class	LinkCommandParser
 * @brief	Parses "stream|eplink|slink create|start|stop|send [args]"
 * 			lines into a ::LinkCommand, and extracts the "-sid=" stream id
 * 			tag from STM32-originated lines like "OK -sid=0 ...".
 */
class LinkCommandParser {
public:
    LinkCommandParser() = default;

    /**
     * @brief	Parses a link-pool command line into a ::LinkCommand.
     * @param	msg: raw line, already known to start with a pool name
     * 			("stream" | "eplink" | "slink").
     * @retval	filled ::LinkCommand.
     */
    LinkCommand parse(String msg);

private:
    KeyValueParser kv_;		/*!< Reused for parsing each command's args */
};

#endif