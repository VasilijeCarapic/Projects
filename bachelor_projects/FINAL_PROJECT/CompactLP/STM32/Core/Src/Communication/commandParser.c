/**
 ******************************************************************************
 * @file    commandParser.c
 *
 * @brief   Top-level command router for lines received from either the PC
 *          or the ESP UART. CMDPARSE_SortCommand() classifies each line by its
 *          "<A> " / "<B>" prefix (single command vs. multi-line file
 *          transfer mode) and hands normal commands to the internal
 *          prvCMDPARSE_ProcessCommand(), which dispatches to DEVCTRL_Handle(),
 *          CONFCTRL_Handle() or TRANSFERCTRL_Handle() based on the first
 *          token.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include <Communication/commandParser.h>
#include <Platform/platformStm32.h>

typedef enum {
    MODE_NORMAL,  /*   NORMAL COMMANDS   */
    MODE_FILE     /* PARAGRAPH COMMANDS */
} ParserMode_t;

static ParserMode_t parser_mode  = MODE_NORMAL;
static uint32_t     file_lines   = 0;


/**
 * @brief   Dispatches a single normal-mode command line to the correct
 *          handler based on its first whitespace-delimited token.
 *
 *          Recognized first tokens:
 *            - "SRV:":      echoes the full line back to the PC.
 *            - "show":      forwarded to DEVCTRL_Handle() (LED/button).
 *            - "device" / "charger" / "stream" / "eplink" / "slink":
 *                           forwarded to CONFCTRL_Handle().
 *            - "transmit":  forwarded to TRANSFERCTRL_Handle().
 *            - anything else / empty: reports an "Unknown command" /
 *                           "Empty command" error to the PC.
 *
 * @param   cmd: command line (already stripped of any "<A> " prefix).
 *          Copied into a local buffer and tokenized in place, so the
 *          original string is not modified.
 * @param   huart_pc: UART handle for the PC link, forwarded to handlers
 *          and used for error replies.
 * @param   huart_esp: UART handle for the ESP link, forwarded to handlers.
 * @retval  None
 */
static void prvCMDPARSE_ProcessCommand(char *cmd, UART_HandleTypeDef *huart_pc, UART_HandleTypeDef *huart_esp)
{
    char buf[100];
    strncpy(buf, cmd, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *command = strtok(buf, " ");

    if (command == NULL)
    {
        UART_Send(huart_pc, "Error: Empty command. error_code:0101\r\n");
    }
    else if (strcmp(command, "SRV:") == 0)
	{
    	char srv_resp[120];
		snprintf(srv_resp, sizeof(srv_resp), "%s\r\n", cmd);
		UART_Send(huart_pc, srv_resp);
	}
    else if (strcmp(command, "show") == 0)
    {
    	DEVCTRL_Handle(cmd, &STM32_Platform);
    }
    else if (strcmp(command, "device") == 0  || (strcmp(command, "charger") == 0) || (strcmp(command, "stream") == 0)
    										 || (strcmp(command, "eplink") == 0)  || (strcmp(command, "slink") == 0))
    {
    	CONFCTRL_Handle(cmd, huart_pc, huart_esp);
    }
    else if (strcmp(command, "transmit") == 0)
    {
        TRANSFERCTRL_Handle(cmd, huart_pc, huart_esp);
    }
    else
    {
        char resp[80];
        snprintf(resp, sizeof(resp), "Error: Unknown command \"%s\". error_code:0102\r\n", command);
        UART_Send(huart_pc, resp);
    }
}

/**
 * @brief   Top-level entry point for every line received on either UART
 *          (called from prvParserTaskFunction() in parserTask.c).
 *
 *          Behaviour depends on the current parser_mode and on the
 *          message's prefix:
 *            - If in MODE_FILE: each line is treated as one line of a
 *              multi-line paragraph transfer and forwarded to the ESP,
 *              with a running line counter (file_lines) and a debug echo
 *              to the PC, until a line equal to "EOF" is received, which
 *              ends the transfer, reports the total forwarded line count
 *              to the PC, and returns to MODE_NORMAL.
 *            - If the message starts with "<A> ": the prefix is stripped
 *              and the remainder is treated as a single command, passed to
 *              prvCMDPARSE_ProcessCommand(). An empty remainder is reported as an
 *              error to the PC.
 *            - If the message starts with "<B>": switches parser_mode to
 *              MODE_FILE, resets file_lines to 0, and tells the PC that
 *              paragraph/file mode has started.
 *            - Otherwise: the whole message is treated as a normal
 *              command and passed directly to prvCMDPARSE_ProcessCommand().
 *
 * @param   message: NUL-terminated line received from the PC or ESP UART.
 * @param   huart_pc: UART handle for the PC link, used for replies/echoes
 *          and forwarded to the command handlers.
 * @param   huart_esp: UART handle for the ESP link, used to forward file
 *          lines and forwarded to the command handlers.
 * @retval  None
 */
void CMDPARSE_SortCommand(char *message, UART_HandleTypeDef *huart_pc, UART_HandleTypeDef *huart_esp)
{

	if (message == NULL || strlen(message) == 0)
	{
		UART_Send(huart_pc, "Error: Empty message. error_code:2000\r\n");
		return;
	}


	if (parser_mode == MODE_FILE)
	{
		if (strcmp(message, "EOF") == 0)
		{
			/* END OF FILE */
			parser_mode = MODE_NORMAL;

			char out[100];
			snprintf(out, sizeof(out), "File transfer complete. Lines forwarded to ESP: %lu\r\n", (unsigned long)file_lines);
			UART_Send(huart_pc, out);

			file_lines = 0;
		}
		else
		{
			/* NORMAL LINE OF FILE */
			char line[100];
			snprintf(line, sizeof(line), "%s\r\n", message);
			UART_Send(huart_esp, line);

			file_lines++;

			/* CHECK FOR PC - DELETE IT IF U DONT NEED IT */
			char dbg[60];
		    snprintf(dbg, sizeof(dbg), "[LINE %lu] %s\r\n", file_lines, message);
		    UART_Send(huart_pc, dbg);
		}

		return;
	}

	if (strncmp(message, "<A> ", 4) == 0)
	{
		char *actual_command = message + 4;

		if (strlen(actual_command) == 0)
		{
			UART_Send(huart_pc, "Error: <A> prefix used but no command given. error_code:2001\r\n");
			return;
		}

		prvCMDPARSE_ProcessCommand(actual_command, huart_pc, huart_esp);
	}

	else if (strncmp(message, "<B>", 3) == 0)
	{
		parser_mode = MODE_FILE;
		file_lines  = 0;

		UART_Send(huart_pc, "Paragraph line mode started. Send lines, finish with EOF.\r\n");

	}

	else{
		/* IF NO COMMAND TYPE GIVEN, WORK LIKE ASCI */
		prvCMDPARSE_ProcessCommand(message, huart_pc, huart_esp);
	}

}
