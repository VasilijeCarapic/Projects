/**
 ******************************************************************************
 * @file    StructTypes.h
 *
 * @brief   Shared data types used to describe a parsed link-pool command
 * 			(stream/eplink/slink create|stop|send), exchanged between
 * 			CommandParser and LinkManager, and UdpStreamLink.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef STRUCT_TYPES_H
#define STRUCT_TYPES_H

#include <WiFiUdp.h>
#include <Arduino.h>
#include <IPAddress.h>


/**
 * @brief	Parsed representation of a "eplink/slink ..." command.
 */
struct LinkCommand {
    String poolName;   /*!< "stream" | "eplink" | "slink" */
    String action;     /*!< "create" | "start" | "stop" | "send" */
    int    linkID;     /*!< target link id, -1 if not applicable (e.g. create) */
    IPAddress ip;      /*!< target ip, valid only for "create" */
    uint16_t  port;    /*!< target port, valid only for "create" */
    String payload;    /*!< raw value = payload, valid only for "send" */
};

/**
 * @brief Parsed representation of the udp streaming 
 * 
 */

 typedef struct {
    bool is_streaming; /*<! is data currently being sent through the udp */ 
    bool slot_taken;   /*<! is slot taken */ 


    uint32_t sid;      /*<! stream sid */
    uint16_t port;     /*<! stream port */


    IPAddress ip;

    WiFiUDP     udp;

 } udp_stream_link_t; 

#endif