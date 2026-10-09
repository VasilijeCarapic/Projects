/**
 ******************************************************************************
 * @file    LinkManager.cpp
 *
 * @brief   Implementation of the LinkManager class declared in LinkManager.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Networking/Links/LinkManager.h"

/**
 * @brief	Initializes the UDP stream link pool.
 */
LinkManager::LinkManager()
{
    udpStream_.Init();
}

/**
 * @brief	Selects the LinkPool matching a pool name.
 * @param	poolName: "eplink" | "slink".
 * @retval	pointer to the matching pool, or nullptr if unknown.
 */
LinkPool* LinkManager::selectPool(const String& poolName)
{
    if (poolName == "eplink") return &eplinkLinks_;
    if (poolName == "slink")  return &slinkLinks_;
    return nullptr;
}

/**
 * @brief	Executes a parsed link command against the UDP stream pool.
 * 			For "create", the sid is not taken from cmd (the GUI never
 * 			sends one for stream create) - the ESP allocates it itself via
 * 			StreamCreateAuto(), using the same 0-indexed "first free slot"
 * 			search the STM32's AllocateStream() uses, so both sides assign
 * 			matching sids.
 * @param	cmd: parsed link command with poolName == "stream".
 * @retval	Response line ("OK", "OK <sid>", "ERROR ...").
 */
String LinkManager::handleStreamCommand(const LinkCommand& cmd)
{
    if (cmd.action == "create") {
        String ipStr = cmd.ip.toString();
        uint32_t sid;

        if (!udpStream_.StreamCreateAuto(ipStr.c_str(), cmd.port, &sid)) {
            return "ERROR connect";
        }
        return "OK " + String(sid);
    }

    if (cmd.linkID < 0) return "ERROR noid";

    if (cmd.action == "start") {
        return udpStream_.StreamStart((uint32_t)cmd.linkID) ? "OK" : "ERROR notfound";
    }
    if (cmd.action == "pause") {
        return udpStream_.StreamPause((uint32_t)cmd.linkID) ? "OK" : "ERROR notfound";
    }
    if (cmd.action == "stop") {
        return udpStream_.StreamStop((uint32_t)cmd.linkID) ? "OK" : "ERROR notfound";
    }
    return "ERROR unknownaction";
}

/**
 * @brief	Executes a parsed link command against a TCP LinkPool.
 * @param	cmd: parsed link command with poolName == "eplink" or "slink".
 * @retval	Response line ("OK", "OK <id>", "ERROR ...").
 */
String LinkManager::handleTcpLinkCommand(const LinkCommand& cmd)
{
    LinkPool* pool = selectPool(cmd.poolName);
    if (pool == nullptr) return "ERROR unknownpool";

    if (cmd.action == "create") {
        int id = pool->getFreeLink();
        if (id < 0) return "ERROR nolinks";
        if (!pool->openLink(id, cmd.ip, cmd.port)) return "ERROR connect";
        return "OK " + String(id);
    }

    if (cmd.linkID < 0) return "ERROR noid";

    if (cmd.action == "stop") {
        pool->closeLink(cmd.linkID);
        return "OK";
    }
    else if (cmd.action == "send") {
        return pool->sendThroughLink(cmd.linkID, cmd.payload) ? "OK" : "ERROR notconnected";
    }
    return "ERROR unknownaction";
}

/**
 * @brief	Routes a parsed link-pool command to the appropriate backend
 * 			based on the pool name.
 * @param	cmd: parsed link command.
 * @retval	Response line ("OK", "ERROR ...").
 */
String LinkManager::handleLinkCommand(const LinkCommand& cmd)
{
    if (cmd.poolName == "stream") return handleStreamCommand(cmd);
    return handleTcpLinkCommand(cmd);
}

/**
 * @brief	Non-blocking poll of all active TCP links (eplink/slink);
 * 			forwards anything received to the STM32 over Serial. The UDP
 * 			stream pool is not polled here since it is fire-and-forget.
 */
void LinkManager::handleLinkPools()
{
    LinkPool* pools[] = { &eplinkLinks_, &slinkLinks_ };

    for (LinkPool* pool : pools) {
        for (int i = 0; i < pool->getMaxLinks(); i++) {
            if (!pool->isConnected(i)) continue;
            String data;
            pool->serviceIncoming(i, data);
            if (data.length() > 0) Serial.println(data);
        }
    }
}