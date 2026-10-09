/**
 ******************************************************************************
 * @file    CommandDispatcher.h
 *
 * @brief   Owns the single byte-driven read loop for the UART link to the
 *          STM32. Every available Serial byte is consumed exactly once and
 *          routed to either the ASCII line reader or the binary frame
 *          reader based on shared mode state, then dispatched to the
 *          appropriate NetworkLayer action.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef _CMD_DISPATCHER_H
#define _CMD_DISPATCHER_H

#include "Networking/NetworkLayer.h"
#include "Parsers/CommandParser.h"
#include "Readers/ConsoleReader.h"
#include "Readers/BinaryFrameReader.h"
#include "Common/StructTypes.h"

/**
 * @class CmdDispatcher
 * @brief Single entry point for everything arriving on the STM32 UART link.
 *        Replaces the old separate decideFlow()/decideBinaryFlow() pair,
 *        which independently drained the same Serial buffer with two
 *        greedy while(available()) loops and could eat each other's bytes
 *        (a control-channel line could be silently swallowed by the binary
 *        frame reader mid-buffer, or vice versa).
 */
class CmdDispatcher{
    public: 
        CmdDispatcher(NetworkLayer* netLayer, CommandParser* cmdParser, ConsoleReader* consoleReader);

        /**
         * @brief	Reads every byte currently available on Serial, exactly
         * 			once each, and routes it to either the ASCII line
         * 			reader or the binary frame reader depending on mode_:
         * 			- MODE_IDLE: the next byte decides the mode. 0xAA ->
         * 			  MODE_BINARY (byte is fed to the binary reader as its
         * 			  sync byte); anything else -> MODE_ASCII (byte is fed
         * 			  to the line reader).
         * 			- MODE_ASCII: every byte goes to ConsoleReader::feedByte()
         * 			  until a line completes, then processLine() runs and
         * 			  mode_ resets to MODE_IDLE.
         * 			- MODE_BINARY: every byte goes to
         * 			  BinaryFrameReader::feedByte() until a frame completes,
         * 			  then the payload is forwarded via
         * 			  NetworkLayer::sendStreamData() and mode_ resets to
         * 			  MODE_IDLE.
         *
         * 			Intended to be called once per loop() iteration.
         */
        void decide();

    private: 
        /**
         * @brief	Classifies a completed ASCII line via CommandParser and
         * 			dispatches the resulting action:
         * 				- ACTION_GUI_FORWARD:     forward to GUI over TCP
         * 				- ACTION_SERVER_RESTART:  re-apply WiFi/server config
         * 				- ACTION_SERVER_CLEAR:    clear the HTTP log
         * 				- ACTION_NONE:            nothing further to do
         * 				- default (ACTION_LOG):   append result.output to the web log
         * @param	line: a completed, trimmed line from ConsoleReader.
         */
        void processLine(const String& line);

        enum Mode { MODE_IDLE, MODE_ASCII, MODE_BINARY };
        Mode mode_ = MODE_IDLE; /*!< which reader currently owns incoming bytes */

        NetworkLayer* netLayer_; 
        CommandParser* cmdParser; 
        ConsoleReader* consoleReader; 
        BinaryFrameReader binReader_;	/*!< Detects/buffers binary stream frames on Serial */
};

#endif