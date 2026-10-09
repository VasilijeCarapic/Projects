#include <Commands/configHelpers.h>
#include <Commands/cmdResponse.h>
#include <Commands/streamCommands.h>
#include <Commands/configurationControl.h>
#include <stdio.h>
#include <string.h>
#include <Utils/usefulFunctions.h>


void STREAMCMD_HandleSlinkCmds(const char *cmd, UART_HandleTypeDef *huart_pc, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz) {
    char valBuf[32];
    int n;

    if (strstr(cmd, "slink create")) {
        int id = STREAMCFG_AllocateSlink();
        if (id == -1) {
            UART_Send(huart_pc, "Error: No slink available. error_code:9001 \r\n");
            prvCONTROL_SendErrorResp(huart_esp, resp, respSz);
            return;
        }
        n = snprintf(valBuf, sizeof(valBuf), "%d", id);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "slink send")) {
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
}

void STREAMCMD_HandleEplinkCmds(const char *cmd, UART_HandleTypeDef *huart_pc, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz) {
    char valBuf[32];
    int n;

    if (strstr(cmd, "eplink create")) {
        int id = STREAMCFG_AllocateEplink();
        if (id == -1) {
            UART_Send(huart_pc, "Error: No elink available. error_code:9002 \r\n");
            prvCONTROL_SendErrorResp(huart_esp, resp, respSz);
            return;
        }
        n = snprintf(valBuf, sizeof(valBuf), "%d", id);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
}

void STREAMCMD_HandleStreamControlCmds(const char *cmd, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz) {
    char valBuf[32];
    int n;

    if (strstr(cmd, "stream create")) {
        int sid = STREAMCFG_AllocateStream();
        if (sid == -1) {
            prvCONTROL_SendErrorResp(huart_esp, resp, respSz);
            return;
        }
        n = snprintf(valBuf, sizeof(valBuf), "%d", sid);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "stream start")) {
        Stream_Config_t* s = CFGHLP_StreamErrorCheck(cmd, huart_esp, resp, respSz);
        if (s == NULL) return;
        s->is_streaming = 1;
        prvCONTROL_SendOkRespHP(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "stream stop")) {
        Stream_Config_t* s = CFGHLP_StreamErrorCheck(cmd, huart_esp, resp, respSz);
        if (s == NULL) return;
        s->is_streaming = 0;
        prvCONTROL_SendOkRespHP(huart_esp, resp, respSz, "OK", 2);
    }
}

void STREAMCMD_HandleAdcCmds(const char *cmd, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz) {
    Stream_Config_t* s = CFGHLP_StreamErrorCheck(cmd, huart_esp, resp, respSz);
    if (s == NULL) return;

    char valBuf[32];
    int n;

    if (strstr(cmd, "chresolution set")) {
        s->adc_resolution = CFGHLP_ExtractParam(cmd, "-value=", s->adc_resolution);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "chresolution get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", s->adc_resolution);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "chclkdiv set")) {
        s->adc_clk_div = CFGHLP_ExtractParam(cmd, "-value=", s->adc_clk_div);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "chclkdiv get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", s->adc_clk_div);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "chstime set")) {
        s->adc_stime = CFGHLP_ExtractParam(cmd, "-value=", s->adc_stime);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "chstime get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", s->adc_stime);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "chavrratio set")) {
        s->adc_chavrratio = CFGHLP_ExtractParam(cmd, "-value=", s->adc_chavrratio);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "chavrratio get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", s->adc_chavrratio);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "speriod set")) {
        s->adc_period    = CFGHLP_ExtractParam(cmd, "-period=", s->adc_period);
        s->adc_prescaler = CFGHLP_ExtractParam(cmd, "-prescaler=", s->adc_prescaler);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "speriod get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%lu", (unsigned long)s->adc_period);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "voffset set")) {
        s->adc_voffset = CFGHLP_ExtractParam(cmd, "-value=", s->adc_voffset);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "voffset get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", s->adc_voffset);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "coffset set")) {
        s->adc_coffset = CFGHLP_ExtractParam(cmd, "-value=", s->adc_coffset);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "coffset get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", s->adc_coffset);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "samplesno set")) {
        s->adc_samplesno = CFGHLP_ExtractParam(cmd, "-value=", s->adc_samplesno);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "clk get") || strstr(cmd, "value get")) {
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "0", 1);
    }
}
