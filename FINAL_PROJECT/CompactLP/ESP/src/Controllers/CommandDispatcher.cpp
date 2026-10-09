/**
 ******************************************************************************
 * @file    CommandDispatcher.cpp
 *
 * @brief   Implementation of the single byte-driven UART dispatch loop
 * 			declared in CommandDispatcher.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Controllers/CommandDispatcher.h"

CmdDispatcher::CmdDispatcher(NetworkLayer* netLayer, CommandParser *cmdParser, ConsoleReader* consoleReader)
{
    this -> netLayer_     = netLayer;
    this -> cmdParser     = cmdParser;
    this -> consoleReader = consoleReader;
}

void CmdDispatcher::decide()
{
    while (Serial.available()) {
        uint8_t b = Serial.read();

        if (mode_ == MODE_IDLE) {
            if (b == '\r' || b == '\n') continue;

            mode_ = (b == 0xAA) ? MODE_BINARY : MODE_ASCII;
        }

        if (mode_ == MODE_BINARY) {
            if (binReader_.feedByte(b)) {
                netLayer_->getMsg("BINFRAME sid=" + String(binReader_.sid()) + " len=" + String(binReader_.payloadLen()));
                netLayer_->sendStreamData(binReader_.sid(), (const char*)binReader_.payload(), binReader_.payloadLen());
                mode_ = MODE_IDLE;
            } 
            else if (binReader_.waitingForSync()) {
                mode_ = MODE_IDLE;
            }

        } else { // MODE_ASCII
            String line;
            if (consoleReader->feedByte((char)b, line)) {
                mode_ = MODE_IDLE;
                if (line.length() > 0) processLine(line);
            }
        }
    }
}

void CmdDispatcher::processLine(const String& line)
{
    CommandParser::ParseResult result = cmdParser -> parseMessage(line);

    switch (result.action) {
        case CommandParser::ACTION_GUI_FORWARD: /* STM32 -> ESP -> GUI */
            netLayer_ -> sendToGui(result.output);
            break;

        case CommandParser::ACTION_SERVER_RESTART:
            netLayer_ -> applyConfig();
            break;

        case CommandParser::ACTION_SERVER_CLEAR:
            netLayer_ -> clearHttpServer();
            break;

        case CommandParser::ACTION_NONE:
            break;

        default:
            netLayer_ -> getMsg(result.output);
            break;
    }
}