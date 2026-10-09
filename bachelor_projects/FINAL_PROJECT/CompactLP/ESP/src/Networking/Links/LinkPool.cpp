/**
 ******************************************************************************
 * @file    Links.cpp
 *
 * @brief   Implementation of the LinkPool class declared in Links.h.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Networking/Links/LinkPool.h"

LinkPool::LinkPool(int maxLinks): maxLinks_(maxLinks), clients_(new WiFiClient[maxLinks]) {}

LinkPool::~LinkPool() { delete[] clients_; }

bool LinkPool::openLink(int linkID, IPAddress ip, uint16_t port)
{
    if (linkID < 0 || linkID >= maxLinks_) return false;

    if (clients_[linkID].connected()) {
        clients_[linkID].stop();
    }

    /* Bound the connect wait so this call cannot stall loop() for the
     * core's default (milisecond) TCP connect timeout. */
    clients_[linkID].setTimeout(CONNECT_TIMEOUT_MS);
    clients_[linkID].connect(ip, port);

    return clients_[linkID].connected();
}

void LinkPool::closeLink(int linkID)
{
    if (linkID < 0 || linkID >= maxLinks_) return;

    if (clients_[linkID].connected()) { clients_[linkID].stop(); }
}

bool LinkPool::sendThroughLink(int linkID, const String& data)
{
    if (linkID < 0 || linkID >= maxLinks_) return false;
    if (!clients_[linkID].connected()) return false;

    clients_[linkID].print(data + "\r\n");
    return true;
}

int LinkPool::getFreeLink() const
{
    for (int i = 0; i < maxLinks_; i++) {
        if (!clients_[i].connected()) return i;
    }
    return -1;
}

bool LinkPool::isConnected(int linkID) const
{
    if (linkID < 0 || linkID >= maxLinks_) return false;
    return clients_[linkID].connected();
}

void LinkPool::serviceIncoming(int linkID, String& outData)
{
    outData = "";
    if (linkID < 0 || linkID >= maxLinks_) return;
    if (!clients_[linkID].connected()) return;

    while (clients_[linkID].available()) {
        outData += (char)clients_[linkID].read();
    }
}