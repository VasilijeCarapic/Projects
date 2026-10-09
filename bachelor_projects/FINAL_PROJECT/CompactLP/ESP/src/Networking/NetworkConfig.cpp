/**
 ******************************************************************************
 * @file    NetworkConfig.cpp
 *
 * @brief   Implementation of the NetworkConfig class declared in
 * 			NetworkConfig.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Networking/NetworkConfig.h"
#include <ESPmDNS.h>

NetworkConfig::NetworkConfig(ServerConfig* config)
{
    this->config_ = config;
}

void NetworkConfig::setupWiFi()
{
    if (config_->getMode() == "STA" || config_->getMode() == "AP+STA")
    {
        WiFi.begin(
            config_->getSSID().c_str(),
            config_->getPass().c_str()
        );

        int attempts = 30;

        while (WiFi.status() != WL_CONNECTED && attempts > 0) {
            delay(500);
            attempts--;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("SRV: WiFi connected");
            Serial.print("SRV: IP address: ");
            Serial.println(WiFi.localIP());
        }
        else {
            Serial.println("SRV: WiFi failed");
        }
    }
}

void NetworkConfig::setupMode()
{
    if (config_->getMode() == "STA")
    {
        WiFi.mode(WIFI_STA);
    }
    
    else if (config_->getMode() == "AP")
    {
        WiFi.mode(WIFI_AP);

        IPAddress ip;
        ip.fromString(config_->getIpAP());

        IPAddress gateway = ip;
        IPAddress subnet(255, 255, 255, 0);

        WiFi.softAPConfig(ip, gateway, subnet);
        WiFi.softAP(config_->getSSIDAP().c_str(), config_->getPassAP().c_str());
    }
    
    else if (config_->getMode() == "AP+STA")
    {
        WiFi.mode(WIFI_AP_STA);

        IPAddress ip;
        ip.fromString(config_->getIpAP());

        IPAddress gateway = ip;
        IPAddress subnet(255, 255, 255, 0);

        WiFi.softAPConfig(ip, gateway, subnet);
        WiFi.softAP(config_->getSSIDAP().c_str(), config_->getPassAP().c_str());
    }
}

void NetworkConfig::setupMDNS()
{
    if (MDNS.begin("esp_server")) {
        Serial.println("SRV: mDNS started: http://esp_server.local");
        MDNS.addService("http", "tcp", config_->getHttpPort());
        return;
    }
    Serial.println("mDNS failed to start");
}