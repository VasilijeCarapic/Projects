/**
 ******************************************************************************
 * @file    device_control.cpp
 *
 * @brief   Implementation of the platform-independent device command
 * 			handler declared in Device_control.h. All hardware access goes
 * 			through the ::Platform_t function pointer table passed in by
 * 			the caller, so this file contains no board-specific code.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */
#include "Platform/Device_control.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief	Sends response string back to the host
 * 		    through the platform's uart_send() callback.
 * 			Safe to call even if platform or its uart_send member is NULL
 * 			(no-op in that case).
 * @param	p: platform instance to send through.
 * @param	msg: null-terminated message to send.
 */
static void Send(const Platform_t *p, const char *msg)
{
    if (p != NULL && p->uart_send != NULL)
        p->uart_send(msg);
}

/**
 * @brief	Executes a "diode" action (on/off/toggle) on the platform LED.
 * @param	action: "on", "off", "toggle".
 * @param	platform: platform instance providing led_on/led_off/led_toggle.
 */
static void Device_Diode(const char *action, const Platform_t *platform)
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
 * @brief	Reports the current status of the LED or the button back to the
 * 			host.
 * @param	what: which LED or button.
 * @param	platform: platform instance providing led_read()/btn_read()
 * 			and uart_send() (used indirectly via Send()).
 */
static void Device_Status(const char *what, const Platform_t *platform)
{
	if (strcmp(what, "diode") == 0)
	{
		if (platform->led_read())
		{
			Send(platform, "LED is ON.\r\n");
		}
		else
		{
			Send(platform, "LED is OFF.\r\n");
		}
	}
	else if (strcmp(what, "button") == 0)
	{
		if (platform->btn_read())
		{
			Send(platform, "BUTTON is PRESSED.\r\n");
		}
		else
		{
			Send(platform, "BUTTON is NOT PRESSED.\r\n");
		}
	}
}

/**
 * @brief	Parses and executes a single "device ..." command line.
 *
 * 			Expected format: "device <sub-command> [<arg>]" where
 * 			<sub-command> is one of:
 * 				- "diode <on|off|toggle>"  -> drives the LED (see Device_Diode())
 * 				- "status <diode|button>"  -> reports current state (see Device_Status())
 *
 * 			On any missing/invalid sub-command or argument, an error message
 * 			with a numeric error_code is sent back via platform->uart_send().
 *
 * @param	message: null-terminated command line, e.g. "device diode on".
 * 			The leading "device" word is skipped (assumed already matched
 * 			by the caller before routing here).
 * @param	platform: pointer to the concrete ::Platform_t implementation
 * 			used to act on the hardware and to send responses.
 */
void Device_Handle(const char *message, const Platform_t *platform)
{

    char buf[100];
    strncpy(buf, message, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    strtok(buf, " ");           /* SKIPPING FIRST WORD "device" BECAUSE WE KNOW IT IS FOR DEVICE */
    char *sub_command = strtok(NULL, " ");

    if (sub_command == NULL)
    {
        Send(platform, "Error: No sub-command given. Available options: diode, status. error_code:0100\r\n");
        return;
    }
    else if (strcmp(sub_command, "diode") == 0)
    {
    	char *action = strtok(NULL, " ");

    	if (action == NULL)
    	{
    		Send(platform, "Error: No argument given. Available options: on, off, toggle, status. error_code:0100\r\n");
            return;
    	}
    	if ((strcmp(action, "on") == 0) || (strcmp(action, "off") == 0)  ||  (strcmp(action, "toggle") == 0))
		{
			Device_Diode(action, platform);
		}
    	else
    	{
    		char err[80];
			snprintf(err, sizeof(err), "Error: Unknown action \"%s\". error_code:0102\r\n", action);
			Send(platform, err);
    	}
    }
    else if (strcmp(sub_command, "status") == 0)
    {
        char *what = strtok(NULL, " ");

        if (what == NULL)
        {
            Send(platform, "Error: Specify quest. Use: status diode / button. error_code:0101 \r\n");
            return;
        }
        else if ((strcmp(what, "diode") == 0) || (strcmp(what, "button") == 0))
        {
            Device_Status(what, platform);
        }
        else
        {
            Send(platform, "Error: Specify device. Use: status diode / button. error_code:0102 \r\n");
        }
    }
    else
    {
        Send(platform, "Error: No sub-command given. Available options: on, off, toggle, status. error_code:0103\r\n");
    }
}