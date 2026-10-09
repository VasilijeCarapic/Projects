/**
 ******************************************************************************
 * @file    LinkManager.h
 *
 * @brief   Owns the UDP stream link pool and the two LinkPool instances
 * 			(eplink/slink) used for STM32-initiated outbound TCP connections,
 * 			executes parsed link commands (create/start/pause/stop/send)
 * 			against the right backend, and polls all active TCP links for
 * 			incoming data every loop() tick.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef LINK_MANAGER_H
#define LINK_MANAGER_H

#include <Arduino.h>
#include "Networking/Links/LinkPool.h"
#include "Networking/Links/UdpStreamLink.h"
#include "Common/Constants.h"
#include "Common/StructTypes.h"

/**
 * @class	LinkManager
 * @brief	Dispatches parsed stream/eplink/slink commands to the matching
 * 			backend (UdpStreamLink for "stream", LinkPool for "eplink"/
 * 			"slink") and services incoming data on all active TCP links,
 * 			forwarding anything received to the STM32 over Serial.
 */
class LinkManager {
public:
    /**
     * @brief	Initializes the UDP stream link pool.
     */
    LinkManager();

    /**
     * @brief	Executes a parsed link-pool command (create/start/pause/
     * 			stop/send) against the appropriate backend and returns the
     * 			result line.
     * @param	cmd: parsed link command.
     * @retval	Response line ("OK 2", "ERROR connect", ...). Caller decides
     * 			what to do with it (Serial for STM32-originated commands via
     * 			NetworkLayer::handleLinkCommand, or nothing for GUI-originated
     * 			commands where STM32's own OK is the real confirmation).
     */
    String handleLinkCommand(const LinkCommand& cmd);

    /**
     * @brief	Non-blocking poll of all active TCP links (eplink/slink);
     * 			forwards anything received to the STM32 over Serial. The UDP
     * 			stream pool is not polled here since it is fire-and-forget.
     * 			Safe to call every loop() tick.
     */
    void handleLinkPools();

    /**
     * @brief	Sends sample data for a given stream ID over its UDP link.
     * @param	sid: stream ID of the UDP link.
     * @param	data: data to be sent through the link.
     * @retval	true if data was successfully sent through the link.
     * @retval	false if the link doesn't exist, isn't streaming, or the
     * 			send failed.
     */

    bool sendStreamData(int sid, const uint8_t* data, size_t len) {
        return udpStream_.SendSample((uint32_t)sid, data, len);
    }
    /**
     * @brief	Fully resets the UDP stream pool (all slots freed, sids
     * 			cleared). Must be called on "device hello" to stay in sync
     * 			with the STM32's InitStreams(), which is also called on
     * 			"device hello" and is the only point where the STM32 frees
     * 			stream slots.
     */
    void resetStreams() { udpStream_.Init(); }

private:
    UdpStreamLink udpStream_;                              /*!< UDP stream links (sample data) */
    LinkPool eplinkLinks_{ELINK_CONNECTIONS_MAX_NUMBER};    /*!< Elink connections (TCP) */
    LinkPool slinkLinks_{SLINK_CONNECTIONS_MAX_NUMBER};     /*!< Slink connections (TCP) */

    /**
     * @brief	Selects the LinkPool matching a pool name.
     * @param	poolName: "eplink" | "slink".
     * @retval	pointer to the matching pool, or nullptr if unknown.
     */
    LinkPool* selectPool(const String& poolName);

    /**
     * @brief	Executes a parsed link command against the UDP stream pool
     * 			(create/start/pause/stop).
     * @param	cmd: parsed link command with poolName == "stream".
     * @retval	Response line ("OK", "OK <sid>", "ERROR ...").
     */
    String handleStreamCommand(const LinkCommand& cmd);

    /**
     * @brief	Executes a parsed link command against a TCP LinkPool
     * 			(create/stop/send), for "eplink"/"slink" pools.
     * @param	cmd: parsed link command with poolName == "eplink" or "slink".
     * @retval	Response line ("OK", "OK <id>", "ERROR ...").
     */
    String handleTcpLinkCommand(const LinkCommand& cmd);
};

#endif