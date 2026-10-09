/**
 ******************************************************************************
 * @file    WebLogFormatter.cpp
 *
 * @brief   Implementation of the WebLogFormatter class declared in
 * 			WebLogFormatter.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Parsers/WebLogFormatter.h"
#include "Common/Constants.h"

String WebLogFormatter::parseEcho(String msg)
{
    msg.trim();

    String result = msg.substring(5); // strip "echo:"
    result.trim();

    return result;
}

String WebLogFormatter::parseCheck(String msg)
{
    String s = msg.substring(6); // strip "check:"

    s.replace("[", "");
    s.replace("]", "");

    kv_.parse(s);

    if (kv_.count() < 2)
        return "invalid format for check command! Correct Format: check [T = arg1; p = arg2;];";

    String key1 = kv_.key(0);
    String key2 = kv_.key(1);
    String val1 = kv_.value(0);
    String val2 = kv_.value(1);

    float num1 = extractNumber(val1);
    float num2 = extractNumber(val2);

    if (key1 == "T" || key1 == "t") key1 = "Temperatura";
    if (key2 == "P" || key2 == "p") key2 = "Pritisak";

    String msg1 = key1 + " je " + val1;
    String msg2 = key2 + " je " + val2;

    String out = "";
    out += colorizeText(msg1, num1, TEMP_LOW, TEMP_MEDIUM, TEMP_HIGH);
    out += ". ";
    out += colorizeText(msg2, num2, PRESSURE_LOW, PRESSURE_MEDIUM, PRESSURE_HIGH);
    return out;
}

float WebLogFormatter::extractNumber(const String& s)
{
    String cleaned = "";

    for (char c : s)
        if (isDigit(c) || c == '.' || c == '-') { cleaned += c; }

    return cleaned.toFloat();
}

String WebLogFormatter::colorizeText(String label, float value, float low, float mid, float high)
{
    String color;

    if (value < low) color = "#2196F3";       // blue (cool / low)
    else if (value < mid) color = "#4CAF50";  // green (ok)
    else if (value < high) color = "#FFC107"; // amber (warning)
    else color = "#F44336";                   // red (critical)

    return "<span style='color:" + color + "'>" + label + "</span>";
}