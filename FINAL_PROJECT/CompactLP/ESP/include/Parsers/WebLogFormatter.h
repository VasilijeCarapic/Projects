/**
 ******************************************************************************
 * @file    WebLogFormatter.h
 *
 * @brief   Turns raw "echo:"/"check:" diagnostic lines from the STM32 into
 * 			human-readable text/HTML for display on the ESP's web log page.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef WEB_LOG_FORMATTER_H
#define WEB_LOG_FORMATTER_H

#include <Arduino.h>
#include "Parsers/KeyValueParser.h"

/**
 * @class	WebLogFormatter
 * @brief	Turns raw "echo:"/"check:" diagnostic lines from the STM32 into
 * 			human-readable text/HTML for display on the ESP's web log page
 * 			(see MsgLogger, HttpServer).
 */
class WebLogFormatter {
public:
    WebLogFormatter() = default;

    /**
     * @brief	Strips the leading "echo:" (5 characters) from msg and
     * 			trims the remaining whitespace.
     * @param	msg: raw message starting with "echo:".
     * @retval	the text to be logged.
     */
    String parseEcho(String msg);

    /**
     * @brief	Parses a "check:[T=..;p=..;]"-style message into a
     * 			human-readable, color-coded HTML summary.
     *
     * 			Strips the "check:" prefix and any '[' ']' brackets,
     * 			extracts key/value pairs, and expects at least two pairs
     * 			(temperature and pressure). Recognized keys "T"/"t" and
     * 			"P"/"p" are relabeled to "Temperatura"/"Pritisak"
     * 			respectively. Each value's numeric part is extracted and
     * 			used to colorize the corresponding label.
     *
     * @param	msg: raw message starting with "check:".
     * @retval	combined HTML string ("Temperatura je .. . Pritisak je ..")
     * 			with each part wrapped in a colored <span>, or an error
     * 			string if fewer than 2 key/value pairs were found.
     */
    String parseCheck(String msg);

private:
    KeyValueParser kv_;		/*!< Reused for parsing the "check:" pair list */

    /**
     * @brief	Extracts a floating point number from a string, ignoring
     * 			any non-numeric characters other than digits, '.' and '-'.
     * @param	s: input string, potentially containing units/labels (e.g. "23.5C").
     * @retval	parsed float value (0 if no numeric characters were present).
     */
    float extractNumber(const String& s);

    /**
     * @brief	Wraps label in an HTML <span style='color:...'> based on
     * 			which threshold range value falls into: below low = blue,
     * 			below mid = green, below high = amber, otherwise red.
     * @param	label: text to colorize.
     * @param	value: numeric value used to pick the color band.
     * @param	low: upper bound of the blue ("low") range.
     * @param	mid: upper bound of the green ("ok") range.
     * @param	high: upper bound of the amber ("warning") range; above
     * 			this is red ("critical").
     * @retval	HTML string with the label wrapped in a colored <span>.
     */
    String colorizeText(String label, float value, float low, float mid, float high);
};

#endif