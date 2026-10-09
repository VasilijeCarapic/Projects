/**
 ******************************************************************************
 * @file    configurationControl.c
 * @brief   Main command router for ESP/GUI configuration.
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include <Communication/streamConfig.h>
#include <Commands/configHelpers.h>
#include <Commands/cmdResponse.h>
#include <Commands/streamCommands.h>
#include <Commands/deviceCommands.h>
#include <Commands/configurationControl.h>
#include <stdio.h>
#include <string.h>
#include <Utils/usefulFunctions.h>


void CONFCTRL_Handle(const char *cmd, UART_HandleTypeDef *huart_pc, UART_HandleTypeDef *huart_esp){
    char debug_msg[MAX_LEN_DEBUG_MSG];
    snprintf(debug_msg, sizeof(debug_msg), "GUI SALJE: [%s]\r\n", cmd);
    UART_Send(huart_pc, debug_msg);

    char response[MAX_LEN_RESP_MSG];

    // 1. Osnovne device komande
    if (strstr(cmd, "device hello")) {
        CFGHLP_InitLinks(huart_pc);
        uint32_t n = (uint32_t)strlen(device_config.device_name);
        prvCONTROL_SendOkResp(huart_esp, response, sizeof(response), device_config.device_name, n);
        return;
    }
    if (strstr(cmd, "device setname")) {
        char *name_ptr = strstr(cmd, "-name=");
        if (name_ptr != NULL) {
            strncpy(device_config.device_name, name_ptr + 6, sizeof(device_config.device_name) - 1);
            device_config.device_name[sizeof(device_config.device_name) - 1] = '\0';
        }
        prvCONTROL_SendOkResp(huart_esp, response, sizeof(response), NULL, 0);
        return;
    }

    // 2. Rutiranje po prefiksu / kategoriji komande
    if (strncmp(cmd, "device slink", 12) == 0) {
        STREAMCMD_HandleSlinkCmds(cmd, huart_pc, huart_esp, response, sizeof(response));
    }
    else if (strncmp(cmd, "device eplink", 13) == 0) {
        STREAMCMD_HandleEplinkCmds(cmd, huart_pc, huart_esp, response, sizeof(response));
    }
    else if (strncmp(cmd, "device stream", 13) == 0) {
        STREAMCMD_HandleStreamControlCmds(cmd, huart_esp, response, sizeof(response));
    }
    else if (strstr(cmd, "device adc")) {
        STREAMCMD_HandleAdcCmds(cmd, huart_esp, response, sizeof(response));
    }
    else if (strncmp(cmd, "charger", 7) == 0) {
        DEVCMD_HandleChargerCmds(cmd, huart_esp, response, sizeof(response));
    }
    else if (strstr(cmd, "device dac") || strstr(cmd, "device load") || strstr(cmd, "device bat") || strstr(cmd, "device ppath")) {
        DEVCMD_HandleDacAndSwitchesCmds(cmd, huart_esp, response, sizeof(response));
    }
    else if (strstr(cmd, "device wave") || strstr(cmd, "device uvoltage") || strstr(cmd, "device ovoltage") ||
             strstr(cmd, "device ocurrent") || strstr(cmd, "device latch") || strstr(cmd, "device rgb")) {
        DEVCMD_HandleWaveAndMiscCmds(cmd, huart_esp, response, sizeof(response));
    }
    else {
        UART_Send(huart_pc, "Error: Else block discovered :(. error_code:9999 \r\n");
        prvCONTROL_SendErrorResp(huart_esp, response, sizeof(response));
    }
}
