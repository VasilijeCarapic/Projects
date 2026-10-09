/**
 ******************************************************************************
 * @file    TcpServer.cpp
 *
 * @brief   Implementation of the TcpServer class declared in TcpServer.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Networking/Servers/TcpServer.h"
#include "Parsers/CommandParser.h"
#include "Networking/Links/LinkManager.h"

TcpServer::TcpServer(ServerConfig* config)
{
    this->config_ = config;
    this->tcpServer_ = nullptr;
}

void TcpServer::attachLinkHandling(CommandParser* parser, LinkManager* links)
{
    cmdParser_ = parser;
    links_ = links;
}

void TcpServer::stopTcpServer()
{
    if (tcpServer_ != nullptr) {
        tcpServer_->stop();
        delete tcpServer_;
        tcpServer_ = nullptr;
    }
}

void TcpServer::startTcpServer()
{
    stopTcpServer();
    tcpServer_ = new WiFiServer(config_->getTcpPort());
    tcpServer_->begin();
    Serial.println("SRV: TCP Server for OpenEPT started on port: " + String(config_->getTcpPort()));
}

void TcpServer::handleGuiTcpConnection()
{
    if (!tcpClient_ || !tcpClient_.connected()) {
        if (tcpClient_) tcpClient_.stop();
        tcpClient_ = tcpServer_->accept();
        tcpClient_.setNoDelay(true);

        sendToGui("OK esp8266");

    } else {
        // GUI already connected, reject extra clients
        WiFiClient extraClient = tcpServer_->accept();
        extraClient.stop();
    }
}

bool TcpServer::stripDevice(String& lowerWithoutDevice, String& withoutDevice, const String& lower, const String& command){
    bool link_cmd_found = false; 
    
    if (lower.startsWith("device ")) {
        withoutDevice = command.substring(7);
        lowerWithoutDevice = lower.substring(7);
        
        link_cmd_found = (lowerWithoutDevice.startsWith("stream ") ||
         lowerWithoutDevice.startsWith("eplink ") ||
         lowerWithoutDevice.startsWith("slink "));
    }

    return link_cmd_found; 

}

void TcpServer::forwardGuiToStm()
{
    bool link_cmd_found = false;

    if (!tcpClient_ || !tcpClient_.connected()) return;

    String command = "";
    while (tcpClient_.available()) { command += (char)tcpClient_.read(); }
    command.trim();

    if (command.length() == 0) return;

    String lower = command;
    lower.toLowerCase();

    /*reset UDP streams after device hello to erase previous state */
    if (lower == "device hello" && links_ != nullptr) { links_->resetStreams(); }

    String withoutDevice = command;
    String lowerWithoutDevice = lower;

    link_cmd_found = stripDevice(lowerWithoutDevice, withoutDevice, lower, command); 

    if (cmdParser_ != nullptr && links_ != nullptr && link_cmd_found)
    {
        LinkCommand linkCmd = cmdParser_->parseLinkCommand(withoutDevice);

        links_->handleLinkCommand(linkCmd);

        Serial.println(command);

        return;
    }

    Serial.println(command); // everything else, unchanged
}

void TcpServer::sendToGui(const String& msg)
{
    if (tcpClient_ && tcpClient_.connected()) { tcpClient_.print(msg + "\r\n"); }
}

void TcpServer::handleTcpServer()
{
    if (tcpServer_ == nullptr) return;

    if (tcpServer_->hasClient()) { handleGuiTcpConnection(); }

    if (tcpClient_ && tcpClient_.connected()) { forwardGuiToStm(); }
}
