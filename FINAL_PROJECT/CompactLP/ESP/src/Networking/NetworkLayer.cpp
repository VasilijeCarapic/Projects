/**
 ******************************************************************************
 * @file    Servers.cpp
 *
 * @brief   Implementation of the Servers coordinator class.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Networking/NetworkLayer.h"

NetworkLayer::NetworkLayer(ServerConfig* config, CommandParser* cmdParser)
    : config_(config)
    , cmdParser_(cmdParser)
    , http_(config)
    , tcp_(config)
    , links_()
    , network_(config)
{
    tcp_.attachLinkHandling(cmdParser_, &links_);
}

void NetworkLayer::applyConfig()
{
    http_.stopHttpServer();
    tcp_.stopTcpServer();

    network_.setupMode();
    network_.setupWiFi();

    http_.startHttpServer();
    tcp_.startTcpServer();

    network_.setupMDNS();
    config_->print();
}

void NetworkLayer::handleServers()
{
    http_.handleHttpServer();
    tcp_.handleTcpServer();
    links_.handleLinkPools();
}

void NetworkLayer::clearHttpServer() { http_.clearHttpServer(); }

void NetworkLayer::getMsg(const String& msg) { http_.getMsg(msg); }

void NetworkLayer::sendToGui(const String& msg) { tcp_.sendToGui(msg); }

bool NetworkLayer::sendStreamData(int sid, const char* data, size_t len) {
    return links_.sendStreamData(sid, (const uint8_t*)data, len);
}