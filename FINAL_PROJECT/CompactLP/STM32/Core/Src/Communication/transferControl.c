/**
 ******************************************************************************
 * @file    transferControl.c
 *
 * @brief   Handles "transmit" commands. Parses sub-commands and forwards
 *          payloads to PC and/or ESP UARTs.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include <Communication/transferControl.h>


/**
 * @brief   Parses and dispatches "transmit" sub-commands (check, behavior,
 *          value, clear, infomsg1-3).
 *
 * @param   message: Raw input command string.
 * @param   huart_pc: UART handle for PC replies.
 * @param   huart_esp: UART handle for ESP commands.
 * @retval  None
 */
void TRANSFERCTRL_Handle(const char *message, UART_HandleTypeDef *huart_pc, UART_HandleTypeDef *huart_esp)
{
    char buf[100];
    strncpy(buf, message, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    strtok(buf, " ");
    char *sub_command = strtok(NULL, " ");


    if (sub_command == NULL)
    {
        UART_Send(huart_pc, "Error: No sub-command given. Available options: check, value. error_code:1000\r\n");
    }
    else if (strcmp(sub_command, "check") == 0)
    {
        UTILFN_GenerateData(huart_esp);
    }
    else if (strncmp(sub_command, "behavior", 8) == 0){

        const char *behavior_pos = strstr(message, "behavior ");

        if (behavior_pos == NULL)
        {
            UART_Send(huart_pc, "Error: No instructions given after 'behavior'. error_code:1101\r\n");
            return;
        }

        /* Skip "behavior " keyword */
        const char *instructions = behavior_pos + 9;

        if (strlen(instructions) == 0)
        {
            UART_Send(huart_pc, "Error: No instructions given after 'behavior'. error_code:1102\r\n");
            return;
        }

        char out_esp[150];
        snprintf(out_esp, sizeof(out_esp), "%s\r\n", instructions);
        UART_Send(huart_esp, out_esp);

        char out_pc[150];
        snprintf(out_pc, sizeof(out_pc), "Behavior command sent to ESP: [%s]\r\n", instructions);
        UART_Send(huart_pc, out_pc);
    }

    else if (strncmp(sub_command, "value", 5) == 0)
    {
        const char *equal = strchr(message, '=');
        if (equal == NULL){
            UART_Send(huart_pc, "Error: Invalid value format. error_code:1002\r\n");
            return;
        }

        const char *info = equal + 1;
        while (*info == ' '){
            info++;
        }

        if (*info != '"'){
            UART_Send(huart_pc, "Error: Missing opening quote (\"). error_code:1003\r\n");
            return;
        }
        const char *first_quote = info;

        const char *last_quote = strchr(first_quote + 1, '"');
        if (last_quote == NULL)
        {
            UART_Send(huart_pc, "Error: Missing closing quote (\"). error_code:1004\r\n");
            return;
        }

        size_t len = (size_t)(last_quote - (first_quote+1));
        char text[100];

        if (len == 0){
            UART_Send(huart_pc, "Error: Value is empty. error_code:1005\r\n");
            return;
        }
        if (len >= sizeof(text)){
            UART_Send(huart_pc, "Error: Value is too long (max 100 chars). error_code:1006\r\n");
            return;
        }

        strncpy(text, first_quote+1, len);
        text[len] = '\0';

        char out_pc[150];
        char out_esp[150];

        snprintf(out_pc, sizeof(out_pc), "Transfered successfully message: %s \r\n", text);
        UART_Send(huart_pc, out_pc);

        snprintf(out_esp, sizeof(out_esp), "%s \r\n", text);
        UART_Send(huart_esp, out_esp);
    }
    else if (strncmp(sub_command, "clear", 5) == 0)
    {
        UART_Send(huart_esp, "clear\r\n");
    }

    else if (strncmp(sub_command, "infomsg1", 8) == 0)
    {
    	UART_Send(huart_esp, "info: mode=STA;tcp_port=13450;http_port=80;ssid=HUAWEI_B535_11C6;pass=6aLhEDnaG2F;\r\n");
        //UART_Send(huart_pc, "info: mode = STA; tcp_port = 13450; http_port = 80; ssid = HUAWEI_B535_11C6; pass = 6aLhEDnaG2F; \r\n");
    }

    else if (strncmp(sub_command, "infomsg2", 8) == 0)
    {
    	UART_Send(huart_esp, "info: mode=AP;tcp_port=5000;http_port=8081;ssidAP=ESP_NET;passAP=23456789;ipAP=192.168.10.30;\r\n");
        //UART_Send(huart_pc, "info: mode = AP; tcp_port = 5000; http_port = 8081; ssidAP = ESP_NET; passAP = 23456789; ipAP = 192.168.10.30; \r\n");
    }

    else if (strncmp(sub_command, "infomsg3", 8) == 0)
    {
    	UART_Send(huart_esp, "info: mode=AP+STA;tcp_port=3000;http_port=8082;ssid=HUAWEI_B535_11C6;pass=6aLhEDnaG2F;ssidAP=ESP_NET;passAP=12345678;ipAP=192.168.50.5;\r\n");
        //UART_Send(huart_pc, "info: mode = AP+STA; tcp_port = 3000; http_port = 8082; ssid = HUAWEI_B535_11C6; pass = 6aLhEDnaG2F; ssidAP = ESP_NET; passAP = 12345678; ipAP = 192.168.50.5; \r\n");
    }
    else{
        UART_Send(huart_pc, "Error: Invalid sub-command. Available options: check, value. error_code:1009\r\n");
        return;
    }
}
