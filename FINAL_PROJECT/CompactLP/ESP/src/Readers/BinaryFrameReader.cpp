/**
 ******************************************************************************
 * @file    BinaryFrameReader.cpp
 *
 * @brief   Implementation of the byte-fed binary frame state machine
 *          declared in BinaryFrameReader.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Readers/BinaryFrameReader.h"

/**
 * @brief   Advances the frame state machine by exactly one byte.
 *          Resets to WAIT_SYNC on completion or on an invalid declared
 *          length, so the reader is immediately ready for the next frame.
 *
 * @param   byte: next raw byte from the source stream.
 * @retval  true if this byte completed a full frame, false otherwise.
 */
bool BinaryFrameReader::feedByte(uint8_t byte)
{
    switch (state_) {
        case WAIT_SYNC:
            if (byte == 0xAA) state_ = WAIT_SID;
            break;

        case WAIT_SID:
            sid_ = byte;
            state_ = WAIT_LEN_LO;
            break;

        case WAIT_LEN_LO:
            len_ = byte;
            state_ = WAIT_LEN_HI;
            break;

        case WAIT_LEN_HI:
            len_ = (len_ | (byte << 8));

            /* reset if length exceeds buffer size or is bogus */
            if (len_ > MAX_BINARY_DATA_LENGTH || len_ == 0) {
                state_ = WAIT_SYNC;
                break;
            }

            state_ = WAIT_PAYLOAD;
            received_ = 0;
            break;

        case WAIT_PAYLOAD:
            buf_[received_++] = byte;
            if (received_ >= len_) {
                state_ = WAIT_SYNC; // ready for the next frame
                return true;
            }
            break;

        default:
            state_ = WAIT_SYNC;
            break;
    }
    return false;
}