/**
 ******************************************************************************
 * @file     ConsoleReader.h 
 *
 * @brief    Non-blocking, byte-fed ASCII line reader. Used by CmdDispatcher
 *           to assemble one line at a time out of individual Serial bytes,
 *           without ever draining the Serial buffer itself - the caller
 *           (CmdDispatcher::decide()) owns byte consumption so this reader
 *           can safely share a single Serial stream with BinaryFrameReader.
 *
 * @authors  Veljko Sokolov, Vasilije Carapic 
 * @date     August 2026 
 ******************************************************************************
 */

#ifndef _CONSOLE_READER_H 
#define _CONSOLE_READER_H 

#include <Arduino.h>

/**
 * @class ConsoleReader
 * @brief Accumulates bytes fed one at a time via feedByte() into a line,
 *        terminated by '\r', '\n', or a '\r\n' pair (treated as one
 *        terminator).
 */
class ConsoleReader{ 
    public: 
        /**
         * @brief Construct a new Console Reader object with default parameters.
         */
        ConsoleReader() = default;  

        /**
         * @brief   Feeds exactly one byte into the line buffer. Does NOT
         *          touch any Stream - the caller reads the byte and passes
         *          it in, which is what makes it safe to interleave with
         *          BinaryFrameReader::feedByte() on the same physical UART.
         *
         * @param   c: the next raw byte received from the STM32.
         * @param   outLine: filled with the completed, trimmed line when
         *          this call returns true. Left untouched otherwise.
         * @retval  true if a full line just completed (outLine is valid).
         * @retval  false if the line is still incomplete.
         */
        bool feedByte(char c, String& outLine);

    private:
        String buffer_;                    /*!< stores read characters of the line */
        bool   CR_FLAG = false;            /*!< is \r detected or not */
        static const int MAX_LINE = 1000;  /*!< maximum allowed length of the line */
};

#endif