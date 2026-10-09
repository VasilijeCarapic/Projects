/**
 ******************************************************************************
 * @file    BinaryFrameReader.h
 *
 * @brief   Byte-fed state machine for sync-byte-framed binary stream
 *          packets (0xAA, sid, len_lo, len_hi, payload). Fed one byte at a
 *          time via feedByte() by CmdDispatcher::decide(), which owns
 *          Serial byte consumption so this reader can safely share the
 *          same UART with ConsoleReader.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef BINARY_FRAME_READER_H
#define BINARY_FRAME_READER_H

#include <Arduino.h>
#include "Common/Constants.h"

/**
 * @class BinaryFrameReader
 * @brief Parses one byte at a time through sync -> sid -> len -> payload.
 *        Resets itself to WAIT_SYNC whenever a frame completes or the
 *        declared length is invalid, so it's always ready for the next frame.
 */
class BinaryFrameReader {
public:
    BinaryFrameReader() = default;

    bool waitingForSync() const { return state_ == WAIT_SYNC; }

    /**
     * @brief   Feeds exactly one byte into the frame state machine.
     * @param   byte: next raw byte received from the STM32.
     * @retval  true if a complete frame just finished (call sid()/
     *          payload()/payloadLen() to read it out).
     * @retval  false if the frame is still incomplete.
     */
    bool feedByte(uint8_t byte);

    uint8_t sid() const { return sid_; }
    const uint8_t* payload() const { return buf_; }
    uint16_t payloadLen() const { return len_; }

private:
    enum State { WAIT_SYNC, WAIT_SID, WAIT_LEN_LO, WAIT_LEN_HI, WAIT_PAYLOAD };

    State    state_ = WAIT_SYNC;
    uint8_t  sid_ = 0;
    uint16_t len_ = 0;
    uint16_t received_ = 0;
    uint8_t  buf_[MAX_BINARY_DATA_LENGTH] = {0};
};

#endif