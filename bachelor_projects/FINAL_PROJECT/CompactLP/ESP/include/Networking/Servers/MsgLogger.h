/**
 ******************************************************************************
 * @file    MsgLogger.h
 *
 * @brief   Owns and drives the ESP8266's HTTP server (live message log
 * 			page) and TCP server (raw GUI<->STM32 passthrough link), and
 * 			handles WiFi mode setup + mDNS advertisement so the board can
 * 			be reached as "esp_server.local".
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */


#ifndef _MESSAGE_LOGGER_H
#define _MESSAGE_LOGGER_H

/*define needed libraries*/
#include <Arduino.h>

/* forward declaration to escape circular dependency */
class CommandParser;

/**
 * @brief Class which stores messages in a form of a linked list. 
 * When WebServer wants to show received messages to the servers(happens on every 300ms).
 * 
 */
class MsgLogger {
public:
    /**
     * @brief Stores message at the end of the linked list. 
     * 
     * @param msg Message which needs to be put in the list.
     */
    void putMsgInBuffer(const String& msg);

    /**
     * @brief deletes the full linked list which is all content that is shown onto the server 
     */
    void freeList();

    /**
     * @brief this function builds full log that is shown onto the server
     */
    String buildLog();
private:
    
    /**
     * @brief	Single node of the message log linked list.
     */
    struct msgBuffer {
        String msg, date, time, title;	/*!< Formatted message text and its timestamp components */
        msgBuffer* next;				/*!< Pointer to the next (later) log entry, or nullptr if this is the last one */
    };
 
    msgBuffer* head = nullptr;	/*!< First (oldest) entry in the log */
    msgBuffer* tail = nullptr;	/*!< Last (newest) entry in the log */
 
    int msgCount = 0;           /* Counter and limit for message number */
    const int MAX_MESSAGES = 30; 

    CommandParser* parser;		/*!< Unused reserved pointer (forward-declared to avoid circular include) */

};

#endif