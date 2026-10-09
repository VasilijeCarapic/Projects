/**
 ******************************************************************************
 * @file    CommandParser.cpp
 *
 * @brief   Implementation of the CommandParser orchestrator declared in
 * 			CommandParser.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Parsers/CommandParser.h"
#include "Platform/Device_control.h"
#include "Platform/Platform_esp8266.h"

CommandParser::ParseResult CommandParser::parseMessage(String msg)
{
    msg.trim();

    String lower = msg;
    lower.toLowerCase();

    if (lower.startsWith("error")) return {ACTION_GUI_FORWARD, msg};

    if (lower.startsWith("ok")) return {ACTION_GUI_FORWARD, msg};

    if (lower.startsWith("echo:"))  return {ACTION_LOG, webLog_.parseEcho(msg)};

    if (lower.startsWith("check:")) return {ACTION_LOG, webLog_.parseCheck(msg)};

    if (lower.startsWith("info:")) { serverParams_.apply(msg); return {ACTION_SERVER_RESTART, ""}; }

    if (lower == "clear") return {ACTION_SERVER_CLEAR, ""};

    if (lower.startsWith("show")) {
        Device_Handle(lower.c_str(), &ESP8266_Platform);
        return {ACTION_NONE, ""};
    }

    return {ACTION_LOG, "Invalid message: [" + msg + "]"};
}

LinkCommand CommandParser::parseLinkCommand(String msg)
{
    return linkParser_.parse(msg);
}