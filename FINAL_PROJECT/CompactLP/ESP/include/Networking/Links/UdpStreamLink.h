/**
 ******************************************************************************
 * @file    UdpStreamLink.h
 *
 * @brief   This file is responsible for managing the lifecycle, creation, maintenance, 
 *          teardown, and stopping of UDP connections.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */


#ifndef _UDP_STREAM_LINK_H 
#define _UDP_STREAM_LINK_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include "Common/Constants.h"
#include "Common/StructTypes.h"


class UdpStreamLink{
public: 
    UdpStreamLink() = default;
    
    /**
     * @brief Initialize values at the beggining after system boot.
     * 
     */
    void Init(); 


    bool StreamCreate(uint32_t sid, const char* ip, uint16_t port);

    /**
     * @brief   Creates a UDP stream link without a pre-assigned sid: finds
     *          the first free slot (0-indexed, matching the STM32's
     *          AllocateStream()), uses that slot's index as the sid, opens
     *          the UDP socket, and immediately starts streaming (since the
     *          GUI never sends a separate "stream start" for an id it
     *          doesn't know in advance).
     *
     * @param   ip: target IP address for the stream.
     * @param   port: target UDP port for the stream.
     * @param   outSid: filled with the assigned sid on success.
     * @retval  true if a free slot was found and the stream was created
     *          and started successfully.
     * @retval  false if the pool is full, the IP is malformed, or start
     *          failed (slot is rolled back to unused in that case).
     */
    bool StreamCreateAuto(const char* ip, uint16_t port, uint32_t* outSid);

    /**
     * @brief Starts UDP stream. This action is triggered when in GUI start button is pressed(play).
     * 
     * @param  sid  what stream do we start 
     * @return true if stream is successfully started
     * @return false if stream could not be started 
     */
    bool StreamStart(uint32_t sid); 

    /**
     * @brief In GUI a pause means if we hit pause at 0.14 seconds 
     * let's say streaming will be paused at 0.14 seconds after we click start again it will be started 
     * again 
     * 
     * @param  sid id that we want to pause stream for 
     * @return true if stream is successfully paused 
     * @return false if it is not 
     */
    bool StreamPause(uint32_t sid);
    
    /**
     * @brief   Pauses streaming for the given sid without releasing its
     *          slot. The slot/sid/socket remain reserved so config get/set
     *          commands still resolve after stop, mirroring the STM32
     *          side which only clears in_use on a full reset
     *          ("device hello" / InitStreams()), never on "stream stop".
     * 
     * @param  sid id that we want to stop stream for 
     * @return true if stream is successfully stopped 
     * @return false if it is not 
     */
    bool StreamStop(uint32_t sid); 

    /**
     * @brief 
     * 
     * @param sid stream ID
     * @param data data to be send throught package
     * @param size size of the data 
     * @return true if data is successfully sent
     * @return false if data couldn't be sent 
     */
    bool SendSample(uint32_t sid, const uint8_t* data, size_t size); 

    bool IsSlotTaken(uint32_t sid); 
private: 
    udp_stream_link_t udp_streams[SSTREAM_CONNECTIONS_MAX_NUMBER];

    /**
     * @brief find slot by sid
     * 
     * @param sid 
     * @return int that sid by slot 
     */
    int findSlotBySid(uint32_t sid); 

    /**
     * @brief find which sid is free 
     * 
     * @return int: return that sid 
     */
    int findFreeSlot();
};


#endif