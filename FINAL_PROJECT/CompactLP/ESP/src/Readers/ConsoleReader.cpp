/**
 ******************************************************************************
 * @file    ConsoleReader.cpp
 *
 * @brief   Implementation of the non-blocking, byte-fed line reader
 * 			declared in ConsoleReader.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */
#include "Readers/ConsoleReader.h"

/**
 * @brief	Feeds one byte into the line buffer. Handles \r\n pairs
 *          seamlessly by ignoring the duplicate terminator, and resets the
 *          buffer if line length exceeds MAX_LINE to prevent overflow.
 *
 * @param	c: next raw byte from the source stream.
 * @param	outLine: filled with the trimmed line when a terminator is hit.
 * @retval	true if outLine was just completed, false otherwise.
 */
bool ConsoleReader::feedByte(char c, String& outLine)
{
    if (c == '\n')
    {
        if (CR_FLAG) { CR_FLAG = false; return false; } // drugi deo \r\n para - ignorisi
        outLine = buffer_;
        buffer_ = "";
        outLine.trim();
        return true;
    }

    if (c == '\r')
    {
        CR_FLAG = true;
        outLine = buffer_;
        buffer_ = "";
        outLine.trim();
        return true;                    // vracamo liniju vec na sam '\r'
    }

    CR_FLAG = false;
    buffer_ += c;

    if (buffer_.length() >= MAX_LINE) buffer_ = "";

    return false;
}