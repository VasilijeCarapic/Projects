/**
 ******************************************************************************
 * @file    LinkCommandParser.cpp
 *
 * @brief   Implementation of the LinkCommandParser class declared in
 * 			LinkCommandParser.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Parsers/LinkCommandParser.h"


/**
 * @brief   Parses a "poolName action [-key=value ...]" command line into a
 *          LinkCommand (e.g. "stream create -ip=192.168.8.164 -port=11223",
 *          "eplink stop -sid=0"). Splits off poolName and action by the
 *          first two spaces, then hands the remaining "-key=value" args to
 *          KeyValueParser to fill ip/port/linkID/payload.
 * @param   msg: raw line, already known to start with a pool name
 *          ("stream" | "eplink" | "slink").
 * @retval  filled LinkCommand.
 */
LinkCommand LinkCommandParser::parse(String msg)
{
    LinkCommand cmd;
    cmd.linkID = -1;
    cmd.port = 0;

    msg.trim();

    int sp1 = msg.indexOf(' ');
    cmd.poolName = msg.substring(0, sp1);
    msg = msg.substring(sp1 + 1);

    int sp2 = msg.indexOf(' ');
    if (sp2 == -1) { cmd.action = msg; return cmd; }
    cmd.action = msg.substring(0, sp2);

    String args = msg.substring(sp2 + 1);
    kv_.parse(args);

    for (int i = 0; i < kv_.count(); i++) {
        String key = kv_.key(i);
        String val = kv_.value(i);

        if      (key == "ip")     { IPAddress ip; ip.fromString(val); cmd.ip = ip; }
        else if (key == "port")   cmd.port = (uint16_t)val.toInt();
        else if (key == "id" || key == "sid") cmd.linkID = val.toInt();
        else if (key == "value")  cmd.payload = val;
    }

    return cmd;
}

