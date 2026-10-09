/**
 ******************************************************************************
 * @file    ServerParamsParser.cpp
 *
 * @brief   Implementation of the ServerParamsParser class declared in
 * 			ServerParamsParser.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Parsers/ServerParamsParser.h"

void ServerParamsParser::apply(String msg)
{
    String mode, ssid, pass, ssidAP, passAP, ipAP;
    int httpPort = 80, tcpPort = 8080;

    kv_.parse(msg.substring(5)); // strip "info:"

    for (int i = 0; i < kv_.count(); i++) {
        String key = kv_.key(i);
        String val = kv_.value(i);

        if      (key == "mode")      mode   = val;
        else if (key == "http_port") httpPort = val.toInt();
        else if (key == "tcp_port")  tcpPort  = val.toInt();
        else if (key == "ssid")      ssid   = val;
        else if (key == "pass")      pass   = val;
        else if (key == "ssidAP")    ssidAP = val;
        else if (key == "passAP")    passAP = val;
        else if (key == "ipAP")      ipAP   = val;
    }

    config_->setParameters(mode, ssid, pass, ssidAP, passAP, ipAP, httpPort, tcpPort);
}