/**
 ******************************************************************************
 * @file    configHelpers.c
 * @brief   Helper functions for command parsing and stream verification.
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */

#include <Commands/configHelpers.h>
#include <Commands/cmdResponse.h>
#include <Platform/platformStm32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int CFGHLP_ExtractParam(const char *cmd, const char *key, int default_val) {
    char *ptr = strstr(cmd, key);
    if (ptr != NULL) {
        return atoi(ptr + strlen(key));
    }
    return default_val;
}

int CFGHLP_ExtractSid(const char *cmd) {
    return CFGHLP_ExtractParam(cmd, "-sid=", -1);
}

Stream_Config_t* CFGHLP_GetStream(int sid) {
    if (sid < 0 || sid >= SSTREAM_CONNECTIONS_MAX_NUMBER || !streams[sid].in_use) {
        return NULL;
    }
    return &streams[sid];
}

Stream_Config_t* CFGHLP_StreamErrorCheck(const char *cmd, UART_HandleTypeDef *huart_esp, char *response, uint32_t responseSize) {
    int sid = CFGHLP_ExtractSid(cmd);
    Stream_Config_t *stream_ptr = CFGHLP_GetStream(sid);
    if (stream_ptr == NULL) {
        prvCONTROL_SendErrorResp(huart_esp, response, responseSize);
        return NULL;
    }
    return stream_ptr;
}

void CFGHLP_InitLinks(UART_HandleTypeDef *huart_pc) {
    STREAMCFG_InitStreams();
    STREAMCFG_InitSLink();
    STREAMCFG_InitEpLink();
    UART_Send(huart_pc, "System memory cleared before new acquisition starts!\r\n");
}
