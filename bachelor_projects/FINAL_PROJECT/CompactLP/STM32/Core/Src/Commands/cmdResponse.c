#include <Commands/cmdResponse.h>

/**
 * @brief builds OK [msg] \r\n OK response
 */
void prvCONTROL_PrepareOkResp(char* resp, char* msg, uint32_t respSize, uint32_t msgSize){
    char* respPos = resp;
    uint32_t okLen = strlen(CONTROL_RESPONSE_OK_STATUS_MSG), totalLen;

    totalLen = okLen + 1 + msgSize + 2 + 1;

    if(resp == NULL || respSize == 0){ return; }
    if(totalLen > respSize) { return; }

    memcpy(respPos, CONTROL_RESPONSE_OK_STATUS_MSG, okLen);
    respPos += okLen;

    memcpy(respPos, " ", 1);
    respPos += 1;

    if(msg != NULL && msgSize > 0){
        memcpy(respPos, msg, msgSize);
        respPos += msgSize;
    }

    memcpy(respPos, "\r\n", 2);
    respPos += 2;

    *respPos = '\0';
}

void prvCONTROL_PrepareErrorResp(char* resp, uint32_t respSize){
	char* respPos = resp;
	uint32_t errLen = strlen(CONTROL_RESPONSE_ERROR_STATUS_MSG), totalLen;

	/* Error + " " + "1" + \r\n + '\0' */
	totalLen = errLen + 1 + 1  + 2 + 1;

	if(resp == NULL || respSize == 0){ return; }
	if(totalLen > respSize) { return; }

	/* put ERROR in respPos*/
	memcpy(respPos, CONTROL_RESPONSE_ERROR_STATUS_MSG, errLen);
	respPos += errLen;

	memcpy(respPos, " ", 1);
	respPos += 1;

	memcpy(respPos, "1", 1);
	respPos += 1;

	memcpy(respPos, "\r\n", 2);
	respPos += 2;

	*respPos = '\0';
}


void prvCONTROL_SendOkResp(UART_HandleTypeDef *huart_esp, char* response, uint32_t respSize, const char* msg, uint32_t msgSize){
    prvCONTROL_PrepareOkResp(response, (char*)msg, respSize, msgSize);
    UART_Send(huart_esp, response);
}

void prvCONTROL_SendOkRespHP(UART_HandleTypeDef *huart_esp, char* response, uint32_t respSize, const char* msg, uint32_t msgSize){
    prvCONTROL_PrepareOkResp(response, (char*)msg, respSize, msgSize);
    UART_SendHP(huart_esp, response);
}

void prvCONTROL_SendErrorResp(UART_HandleTypeDef *huart_esp, char* response, uint32_t respSize){
    prvCONTROL_PrepareErrorResp(response, respSize);
    UART_Send(huart_esp, response);
}
