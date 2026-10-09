/**
 ******************************************************************************
 * @file    KeyValueParser.h
 *
 * @brief   Generic "key=value;key2=value2;..." mini-parser shared by every
 * 			message format that uses key/value pairs (info:, check:, and
 * 			stream/eplink/slink link commands).
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef KEY_VALUE_PARSER_H
#define KEY_VALUE_PARSER_H

#include <Arduino.h>
#include "Common/Constants.h"

/**
 * @class	KeyValueParser
 * @brief	Splits a "key=value" string (space and/or ';'-separated, keys
 * 			optionally CLI-style with a leading '-') into key/value pairs
 * 			and provides indexed/lookup access to the result.
 */
class KeyValueParser {
public:
    KeyValueParser() = default;

    /**
     * @brief	Parses msg into key/value pairs, replacing whatever was
     * 			previously stored. Leading '-' characters on keys are
     * 			stripped (so "-sid=0" and "sid=0" both yield key "sid").
     * @param	msg: string containing key=value pairs, space and/or
     * 			';'-separated.
     */
    void parse(String msg);

    /**
     * @brief	Number of valid pairs parsed by the last parse() call.
     * @retval	pair count.
     */
    int count() const { return pair_count_; }

    /**
     * @brief	Key of the i-th pair (leading '-' already stripped).
     * @param	i: pair index, 0-based.
     * @retval	the key, or "" if i is out of range.
     */
    String key(int i) const;

    /**
     * @brief	Value of the i-th pair.
     * @param	i: pair index, 0-based.
     * @retval	the value, or "" if i is out of range.
     */
    String value(int i) const;

    /**
     * @brief	Convenience lookup: value of the first pair whose key
     * 			matches keyName.
     * @param	keyName: key to search for, without a leading '-'.
     * @param	defaultVal: returned if keyName is not found.
     * @retval	the matching value, or defaultVal if not found.
     */
    String findValue(const String& keyName, const String& defaultVal = "") const;

private:
    /**
     * @brief	Single key/value pair parsed out of a "key=value;" list.
     */
    struct KeyValue { String key; String value; };

    KeyValue pairs_[MAX_PAIRS];	/*!< Buffer holding pairs parsed by parse() */
    int pair_count_ = 0;			/*!< Number of valid entries currently in pairs_ */

    /**
     * @brief	Normalizes a "key=value key2=value2 ..." (or already
     * 			";"-terminated) string so that every pair ends with a ';'.
     * 			Splits on whitespace; appends ';' to each token only if it
     * 			doesn't already end with one. Lets the same parse() logic
     * 			handle both the old space+";"-per-pair format (info:
     * 			messages) and the plain space-separated format used by
     * 			STM32 link commands (e.g. "sid=1 value=2").
     * @param	msg: raw key=value string, any mix of ' ' and/or ';' separators.
     * @retval	normalized string where every pair is terminated by ';'.
     */
    static String normalize(String msg);

    /**
     * @brief	Strips leading '-' characters from a key (GUI sends
     * 			CLI-style keys like "-ip=", "-port=", "-sid=").
     * @param	key: raw key, possibly with leading dashes.
     * @retval	key with leading dashes removed.
     */
    static String stripDashes(String key);
};

#endif