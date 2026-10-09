/**
 ******************************************************************************
 * @file    deviceControl.c
 *
 * @brief   Platform-independent handling of "device" (LED/button) commands.
 *          Parses "show device diode ..." / "show device status ..."
 *          command lines and drives the abstract ::Platform_t interface
 *          (led_on/off/toggle/read, btn_read, uart_send) so the same logic
 *          works regardless of the concrete platform backend in use.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include <Platform/deviceControl.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief   Sends a message through the platform's uart_send callback, if
 *          both the platform pointer and the callback are valid.
 * @param   p: platform instance to send through.
 * @param   msg: NUL-terminated message to transmit.
 * @retval  None
 */
static void prvDEVCTRL_Send(const Platform_t *p, const char *msg)
{
    if (p != NULL && p->uart_send != NULL)
        p->uart_send(msg);
}

/**
 * @brief   Executes an LED action ("on" / "off" / "toggle") on the given
 *          platform.
 * @param   action: one of "on", "off", "toggle" (already validated by the
 *          caller).
 * @param   platform: platform instance whose led_on/led_off/led_toggle
 *          callback is invoked.
 * @retval  None
 */
static void prvDEVCTRL_Diode(const char *action, const Platform_t *platform)
{
	if (strcmp(action, "on") == 0){
		platform->led_on();
	}
	else if (strcmp(action, "off") == 0){
		platform->led_off();
	}
	else if (strcmp(action, "toggle") == 0){
		platform->led_toggle();
	}
}

/**
 * @brief   Reports the current status of the LED or the button back over
 *          UART.
 * @param   what: "diode" to report LED state, "button" to report button
 *          state (already validated by the caller).
 * @param   platform: platform instance used to read state (led_read /
 *          btn_read) and to send the resulting text (via prvDEVCTRL_Send()).
 * @retval  None
 */
static void prvDEVCTRL_Status(const char *what, const Platform_t *platform)
{
	if (strcmp(what, "diode") == 0)
	{
		if (platform->led_read())
		{
			prvDEVCTRL_Send(platform, "LED is ON.\r\n");
		}
		else
		{
			prvDEVCTRL_Send(platform, "LED is OFF.\r\n");
		}
	}
	else if (strcmp(what, "button") == 0)
	{
		if (platform->btn_read())
		{
			prvDEVCTRL_Send(platform, "BUTTON is PRESSED.\r\n");
		}
		else
		{
			prvDEVCTRL_Send(platform, "BUTTON is NOT PRESSED.\r\n");
		}
	}
}

/**
 * @brief   Entry point for "device" commands routed here from
 *          commandParser.c (the "show" command).
 *
 *          Tokenizes the message on spaces, skips the leading "device"
 *          keyword and dispatches on the following sub-command:
 *            - (none):        reports "No sub-command given" error.
 *            - diode <action>: on/off/toggle via prvDEVCTRL_Diode(), or an
 *                              "Unknown action" error for anything else.
 *            - status <what>:  diode/button via prvDEVCTRL_Status(), or a
 *                              "Specify device" error for anything else.
 *            - anything else:  reports "No sub-command given" error.
 *
 * @param   message: full raw command line, e.g. "show device diode on".
 *          Copied into a local buffer and tokenized in place, so the
 *          original string is not modified.
 * @param   platform: platform instance (LED/button/UART callbacks) used to
 *          execute the command and reply.
 * @retval  None
 */
void DEVCTRL_Handle(const char *message, const Platform_t *platform)
{

    char buf[100];
    strncpy(buf, message, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    strtok(buf, " ");           /* SKIPPING FIRST WORD "device" BECOUSE WE KNOW IT IS FOR DEVICE */
    char *sub_command = strtok(NULL, " ");

    if (sub_command == NULL)
    {
        prvDEVCTRL_Send(platform, "Error: No sub-command given. Available options: diode, status. error_code:0100\r\n");
        return;
    }
    else if (strcmp(sub_command, "diode") == 0)
    {
    	char *action = strtok(NULL, " ");

    	if (action == NULL)
    	{
    		prvDEVCTRL_Send(platform, "Error: No argument given. Available options: on, off, toggle, status. error_code:0100\r\n");
            return;
    	}
    	if ((strcmp(action, "on") == 0) || (strcmp(action, "off") == 0)  ||  (strcmp(action, "toggle") == 0))
		{
			prvDEVCTRL_Diode(action, platform);
		}
    	else
    	{
    		char err[80];
			snprintf(err, sizeof(err), "Error: Unknown action \"%s\". error_code:0102\r\n", action);
			prvDEVCTRL_Send(platform, err);
    	}
    }
    else if (strcmp(sub_command, "status") == 0)
    {
        char *what = strtok(NULL, " ");

        if (what == NULL)
        {
            prvDEVCTRL_Send(platform, "Error: Specify quest. Use: status diode / button. error_code:0101 \r\n");
            return;
        }
        else if ((strcmp(what, "diode") == 0) || (strcmp(what, "button") == 0))
        {
            prvDEVCTRL_Status(what, platform);
        }
        else
        {
            prvDEVCTRL_Send(platform, "Error: Specify device. Use: status diode / button. error_code:0102 \r\n");
        }
    }
    else
    {
        prvDEVCTRL_Send(platform, "Error: No sub-command given. Available options: on, off, toggle, status. error_code:0103\r\n");
    }
}
