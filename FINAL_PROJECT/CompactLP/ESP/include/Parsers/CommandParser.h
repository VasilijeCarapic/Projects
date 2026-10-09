/**
 ******************************************************************************
 * @file    CommandParser.h
 *
 * @brief   Classifies a line received from the STM32 and decides what the
 * 			caller (CmdDispatcher) should do with it, delegating parsing
 * 			work to LinkCommandParser, WebLogFormatter, and
 * 			ServerParamsParser.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <Arduino.h>
#include "Networking/Servers/ServerConfig.h"
#include "Common/StructTypes.h"
#include "Parsers/LinkCommandParser.h"
#include "Parsers/WebLogFormatter.h"
#include "Parsers/ServerParamsParser.h"

/**
 * @class	CommandParser
 * @brief	Orchestrator: classifies a line received from the STM32 and
 * 			decides what the caller (CmdDispatcher) should do with it,
 * 			delegating the actual parsing work to ::LinkCommandParser
 * 			(stream/eplink/slink + sid extraction), ::WebLogFormatter
 * 			(echo:/check:), and ::ServerParamsParser (info:).
 */
class CommandParser {

public:

    /**
     * @brief	Action that the caller (CmdDispatcher) should take after parsing
     * 			a line.
     */
    enum parseAction {
    	ACTION_NONE,			/*!< Nothing to do, message already handled internally (e.g. "show" commands) */
    	ACTION_LOG,				/*!< Append output to the web log (MsgLogger) */
    	ACTION_SERVER_RESTART,	/*!< Reconfigure and restart HTTP/TCP servers (new "info:" params received) */
    	ACTION_SERVER_CLEAR,	/*!< Clear the web log ("clear" command received) */
    	ACTION_GUI_FORWARD,		/*!< Forward the raw message straight to the GUI over TCP */
        ACTION_LINK_CMD        /*!< Stream/eplink/slink create|start|stop|send */
    };

    /**
     * @brief	Result of parsing a single line: what to do, and with which text.
     */
    struct ParseResult { parseAction action; String output; int sid = -1; };

    CommandParser() : serverParams_(nullptr) {}
    CommandParser(ServerConfig* config) : serverParams_(config) {}
    ~CommandParser() = default;

    /**
     * @brief	Parses one line of text and determines the resulting action.
     *
     * 			Recognized prefixes/formats (case-insensitive):
     * 				- "error..."             -> ACTION_GUI_FORWARD (as-is)
     * 				- "ok..." with "-sid="   -> ACTION_STREAM_FORWARD (sid extracted)
     * 				- "ok..." (no sid)       -> ACTION_GUI_FORWARD (as-is)
     * 				- "echo:<text>"          -> ACTION_LOG (text after "echo:")
     * 				- "check:[T=..;p=..;]"   -> ACTION_LOG (colorized T/P summary)
     * 				- "info:key=val;..."     -> updates ServerConfig, ACTION_SERVER_RESTART
     * 				- "clear"                -> ACTION_SERVER_CLEAR
     * 				- "show..."              -> forwarded to Device_Handle(), ACTION_NONE
     * 				- anything else          -> ACTION_LOG ("Invalid message!")
     *
     * @param	msg: raw line received from the STM32.
     * @retval	::ParseResult with the action to perform and its payload.
     */
    ParseResult parseMessage(String msg);

    /**
     * @brief	Parses a "stream|eplink|slink create|start|stop|send [args]"
     * 			line. Delegates to ::LinkCommandParser.
     * @param	msg: raw line, already known to start with one of the pool names.
     * @retval	filled LinkCommand.
     */
    LinkCommand parseLinkCommand(String msg);

private:
    LinkCommandParser  linkParser_;	/*!< Handles stream/eplink/slink command parsing + sid extraction */
    WebLogFormatter    webLog_;		/*!< Handles echo:/check: -> web log text/HTML formatting */
    ServerParamsParser serverParams_;	/*!< Handles info: -> ServerConfig */
};

#endif