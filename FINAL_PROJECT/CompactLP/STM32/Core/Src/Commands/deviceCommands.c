#include <Commands/configHelpers.h>
#include <Commands/cmdResponse.h>
#include <Commands/deviceCommands.h>
#include <Commands/configurationControl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Utils/usefulFunctions.h>

void DEVCMD_HandleChargerCmds(const char *cmd, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz) {
    char valBuf[32];
    int n;

    if (strstr(cmd, "charging enable")) {
        device_config.charger_enabled = 1;
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "charging disable")) {
        device_config.charger_enabled = 0;
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "charging get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", device_config.charger_enabled);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "charging current set")) {
        device_config.charger_current = CFGHLP_ExtractParam(cmd, "-value=", device_config.charger_current);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "charging current get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", device_config.charger_current);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "charging termcurrent set")) {
        device_config.charger_termcurrent = CFGHLP_ExtractParam(cmd, "-value=", device_config.charger_termcurrent);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "charging termcurrent get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", device_config.charger_termcurrent);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "charging termvoltage set")) {
        char *val_ptr = strstr(cmd, "-value=");
        if (val_ptr != NULL) {
            device_config.charger_termvoltage = atof(val_ptr + 7);
        }
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "charging termvoltage get")) {
        int v_int = (int)device_config.charger_termvoltage;
        int v_frac = (int)(device_config.charger_termvoltage * 100) % 100;
        n = snprintf(valBuf, sizeof(valBuf), "%d.%02d", v_int, v_frac);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "reg read")) {
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK: 0x00", 8);
    }
}

void DEVCMD_HandleDacAndSwitchesCmds(const char *cmd, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz) {
    char valBuf[32];
    int n;

    // DAC
    if (strstr(cmd, "dac enable set")) {
        device_config.dac_enabled = CFGHLP_ExtractParam(cmd, "-value=", device_config.dac_enabled);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "dac enable get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", device_config.dac_enabled);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    else if (strstr(cmd, "dac value set")) {
        device_config.dac_value = CFGHLP_ExtractParam(cmd, "-value=", device_config.dac_value);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "dac value get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", device_config.dac_value);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    // LOAD
    else if (strstr(cmd, "load enable")) {
        device_config.load_enabled = 1;
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "load disable")) {
        device_config.load_enabled = 0;
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "load get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", device_config.load_enabled);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    // BAT
    else if (strstr(cmd, "bat enable")) {
        device_config.bat_enabled = 1;
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "bat disable")) {
        device_config.bat_enabled = 0;
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "bat get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", device_config.bat_enabled);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
    // PPATH
    else if (strstr(cmd, "ppath enable")) {
        device_config.ppath_enabled = 1;
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "ppath disable")) {
        device_config.ppath_enabled = 0;
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "ppath get")) {
        n = snprintf(valBuf, sizeof(valBuf), "%d", device_config.ppath_enabled);
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, valBuf, (uint32_t)n);
    }
}

void DEVCMD_HandleWaveAndMiscCmds(const char *cmd, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz) {
    if (strstr(cmd, "wave chunk add") || strstr(cmd, "wave state set") ||
        strstr(cmd, "wave clear") || strstr(cmd, "latch trigger") ||
        strstr(cmd, "rgb setcolor")) {
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "OK", 2);
    }
    else if (strstr(cmd, "uvoltage get") || strstr(cmd, "ovoltage get") || strstr(cmd, "ocurrent get")) {
        prvCONTROL_SendOkResp(huart_esp, resp, respSz, "0", 1);
    }
}
