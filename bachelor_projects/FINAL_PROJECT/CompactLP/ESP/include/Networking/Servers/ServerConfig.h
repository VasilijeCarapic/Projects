/**
 ******************************************************************************
 * @file     Networking/Servers/ServerConfig.h
 *
 * @brief    File for parameter configuration of the servers. 
 *
 * @authors  Veljko Sokolov, Vasilije Carapic 
 * @email    vasasvk2@gmail.com, veljkoSokolov26@gmail.com
 * @date     August 2026 
 ******************************************************************************
 */



#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <Arduino.h>   
#include "Common/Constants.h"



/**
* @class ServerConfig 
* @brief Configures parameters of the HTTP and TCP server 
*/ 
class ServerConfig {
public:

    /**
     * @brief	Default constructor. Initializes the configuration with
     * 			empty STA credentials, the default AP IP (192.168.4.1),
     * 			and the default HTTP/TCP ports. Actual values are typically applied
     * 			right after via setParameters().
     */
    ServerConfig()
    {
        cfg.mode = "";

        cfg.ssid = "";
        cfg.pass = "";

        cfg.ssidAP = "";
        cfg.passAP = "";
        cfg.ipAP = "192.168.4.1";

        cfg.httpPort = DEFAULT_HTTP_PORT;
        cfg.tcpPort  = DEFAULT_TCP_PORT;

    }

    // get functions 
    /** @brief	@retval current WiFi mode ("STA", "AP" or "AP+STA"). */
    const String& getMode()   const { return cfg.mode; }

    /** @brief	@retval configured station-mode SSID to connect to. */
    const String& getSSID()   const { return cfg.ssid; }
    
    /** @brief	@retval configured station-mode password. */
    const String& getPass()   const { return cfg.pass; }

    /** @brief	@retval configured access-point SSID to broadcast. */
    const String& getSSIDAP() const { return cfg.ssidAP; }
    
    /** @brief	@retval configured access-point password. */
    const String& getPassAP() const { return cfg.passAP; }
    
    /** @brief	@retval configured access-point IP address. */
    const String& getIpAP()   const { return cfg.ipAP; }

    /** @brief	@retval configured HTTP server port. */
    int getHttpPort() const { return cfg.httpPort; }
    
    /** @brief	@retval configured TCP (GUI passthrough) server port. */
    int getTcpPort() const { return cfg.tcpPort; }
    
    // set parameters
    /**
     * @brief	Overwrites all configuration fields at once. Typically
     * 			called with the firmware's default values at boot
     *          and again whenever an "info:" message is received from the STM32.
     * @param	mode: WiFi mode ("STA", "AP" or "AP+STA").
     * @param	ssid: station-mode SSID to connect to.
     * @param	pass: station-mode password.
     * @param	ssidAP: access-point SSID to broadcast.
     * @param	passAP: access-point password.
     * @param	ipAP: access-point IP address.
     * @param	httpPort: HTTP server port.
     * @param	tcpPort: TCP (GUI passthrough) server port.
     */
    void setParameters(const String& mode, const String& ssid, const String& pass, const String& ssidAP,
        const String& passAP, const String& ipAP, int httpPort, int tcpPort)
    {
        cfg.mode = mode;

        cfg.ssid = ssid;
        cfg.pass = pass;

        cfg.ssidAP = ssidAP;
        cfg.passAP = passAP;

        cfg.ipAP = ipAP;

        cfg.httpPort = httpPort;
        cfg.tcpPort  = tcpPort;
    }

    /**
     * @brief	Debug / show values on serial / terminal port.
     * 			Prints every configuration field, prefixed with "SRV: ",
     * 			to Serial. Called after applyConfig() which reconfigures the
     * 			servers so the active configuration is visible in the
     * 			debug console.
     */
    void print()
    {
        Serial.println("SRV: === SERVER CONFIG ===");
        Serial.println("SRV: Mode   : " + cfg.mode);
        Serial.println("SRV: SSID   : " + cfg.ssid);
        Serial.println("SRV: PASS   : " + cfg.pass);
        Serial.println("SRV: AP SSID: " + cfg.ssidAP);
        Serial.println("SRV: AP PASS: " + cfg.passAP);
        Serial.println("SRV: AP IP  : " + cfg.ipAP);
        Serial.println("SRV: HTTP PORT   : " + String(cfg.httpPort));
        Serial.println("SRV: TCP PORT    : " + String(cfg.tcpPort));
    }

private:

    /**
     * @brief	Data holder for all configuration fields.
     */
    typedef struct
    {
        String mode, ssid, pass, ssidAP, passAP, ipAP;	/*!< WiFi mode and STA/AP credentials + AP IP */

        int httpPort, tcpPort;							/*!< HTTP and TCP server ports */

    } Config;

    Config cfg;		/*!< Currently active configuration */
};

#endif