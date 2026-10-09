/**
 ******************************************************************************
 * @file    LinkPool.h
 *
 * @brief   A file whose role is to be made so that LinkManager.h sends / creates/ stops links 
 *          which are created via TCP. 
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */



#ifndef _LINKS_H 
#define _LINKS_H 


#include "Common/Constants.h"
#include <WiFi.h>

/**
 * @brief Per-link connection state.
 */
enum class LinkState {
    IDLE,        /*!< No connection attempted / slot free */
    CONNECTED    /*!< Socket connected and usable */
};


/**
 * @class LinkPool
 * @brief Fixed-size pool of WiFiClient sockets, indexed by linkID, with a
 *        bounded-timeout connect so the caller's blocking window is known
 *        and small instead of the multi-second lwIP default.
 */
class LinkPool {
public: 
    /**
     * @brief Construct a new Link Pool object
     * 
     * @param maxLinks Number of allowed TCP connections
     */
    explicit LinkPool(int maxLinks);

    /**
     * @brief Destroy the Link Pool object and clean up memory
     */
    ~LinkPool();

    /**
     * @brief Opens a new link instance
     * 
     * @param linkID Index of the client socket (0 to maxLinks - 1)
     * @param ip IP address of the target server
     * @param port Port of the target TCP server
     * @retval is link successfully opened 
     */
    bool openLink(int linkID, IPAddress ip, uint16_t port);

    /**
     * @brief Closes a link instance.
     * @param linkID Index of the link to close.
     */
    void closeLink(int linkID);

    /**
     * @brief Sends data over a specific link.
     * @param linkID Index of the link to send through.
     * @param data   Data payload to send (a "\r\n" terminator is appended).
     * @retval        true if the link was connected and the send was attempted.
     */
    bool sendThroughLink(int linkID, const String& data);

    /**
     * @brief Finds the first free (not connected) slot.
     * @retval index of a free slot, or -1 if the pool is full.
     */
    int getFreeLink() const;

    /**
     * @brief Checks whether a given link is currently connected.
     * @param linkID Index of the link to check.
     * @retval        true if connected, false otherwise (including out of range).
     */
    bool isConnected(int linkID) const;

    /**
     * @brief Non-blocking poll for data received on a given link.
     *        Reads whatever is already buffered (no waiting) - safe to call
     *        every loop() tick for every active link.
     * @param linkID  Index of the link to poll.
     * @param outData Filled with the bytes read (empty if none available).
     */
    void serviceIncoming(int linkID, String& outData);

    /**
     * @brief Get the Maximum number of the Links 
     * 
     * @return int
     */
    int getMaxLinks() const { return maxLinks_; }

private: 
    int maxLinks_;
    WiFiClient* clients_; 
};

#endif /* _LINKS_H */