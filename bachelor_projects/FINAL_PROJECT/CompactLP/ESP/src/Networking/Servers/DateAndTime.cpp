/**
 ******************************************************************************
 * @file    DateAndTime.cpp
 *
 * @brief   Implementation of the Date and Time helper classes declared in
 * 			DateAndTime.h. 
 *
 * @authors	Veljko Sokolov, Vasilije Carapic
 ******************************************************************************
 */
#include "Networking/Servers/DateAndTime.h"

/**
 * @brief	Reads the current local time via getLocalTime() and formats it
 * 			as "dd/mm/yyyy" into the internal date member.
 * 			Falls back to "00/00/0000" if the time could not be obtained
 */
void Date::setDate() {
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo)) {
        date = "00/00/0000";
        return;
    }

    char buffer[11]; // dd/mm/yyyy
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", &timeinfo);

    date = String(buffer);
}

/**
 * @brief	Returns the date string previously computed by setDate().
 * @retval	formatted date, e.g. "22/08/2026".
 */
String Date::getDate() {
    return date;
}

/**
 * @brief	Reads the current local time via getLocalTime() and formats it
 * 			as "hh:mm:ss" into the internal currTime member.
 * 			Falls back to "00:00:00" if the time could not be obtained.
 */
void Time::setTime() {
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo)) {
        currTime = "00:00:00";
        return;
    }

    char buffer[9]; // hh:mm:ss
    strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);

    currTime = String(buffer);
}

/**
 * @brief	Returns the time string previously computed by setTime().
 * @retval	formatted time, e.g. "14:32:07".
 */
String Time::getTime() {
    return currTime;
}