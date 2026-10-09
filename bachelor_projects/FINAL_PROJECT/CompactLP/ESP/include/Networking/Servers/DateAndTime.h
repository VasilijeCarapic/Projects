/**
 ******************************************************************************
 * @file    DateAndTime.h
 *
 * @brief   Displays time and date on Web Server whenever new message is accepted.
 *          This was done so that server would essentially be like an log of messages. 
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */


#ifndef _DATE_AND_TIME_H
#define _DATE_AND_TIME_H

#include <Arduino.h>
#include <time.h>

/**
* @class Date 
* @brief Handles date storage and formatting (dd/mm/yy).
*/ 
class Date{
    public:
        /**
        * @brief Sets current date value. 
        */
        void setDate();

        /**
        * @brief Gets the formatted date string.
        * @retval retuns formatted date string.  
        */
        String getDate();
    private:
        String date; /*!< Variable in which is current date is saved */
};

/**
 * @class Time
 * @brief Handles time storage and formatting (hh/mm/ss).
 */
class Time{
    public:
        /**
         * @brief Sets the current time value.
         */
        void setTime();
        
        /**
        * @brief  Gets the formatted time string.
        * @retval retuns formatted time string.  
        */
        String getTime();
    private:
        String currTime; /*!< Variable in which is current time is saved */
};

#endif
