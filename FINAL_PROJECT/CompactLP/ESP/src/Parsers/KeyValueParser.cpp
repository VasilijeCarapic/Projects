/**
 ******************************************************************************
 * @file    KeyValueParser.cpp
 *
 * @brief   Implementation of the KeyValueParser class declared in
 * 			KeyValueParser.h.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include "Parsers/KeyValueParser.h"

void KeyValueParser::parse(String msg)
{
    pair_count_ = 0;
    msg = normalize(msg);

    int start = 0, idx;
    while ((idx = msg.indexOf(';', start)) != -1) {
        String token = msg.substring(start, idx);
        token.trim();

        int eq = token.indexOf('=');
        if (eq != -1) {
            String k = token.substring(0, eq);
            String v = token.substring(eq + 1);

            k.trim();
            v.trim();
            k = stripDashes(k);

            if (pair_count_ < MAX_PAIRS) {
                pairs_[pair_count_].key   = k;
                pairs_[pair_count_].value = v;
                pair_count_++;
            }
        }

        start = idx + 1;
    }
}

String KeyValueParser::key(int i) const {
    if (i < 0 || i >= pair_count_) return "";
    return pairs_[i].key;
}

String KeyValueParser::value(int i) const {
    if (i < 0 || i >= pair_count_) return "";
    return pairs_[i].value;
}

String KeyValueParser::findValue(const String& keyName, const String& defaultVal) const {
    for (int i = 0; i < pair_count_; i++) {
        if (pairs_[i].key == keyName) return pairs_[i].value;
    }
    return defaultVal;
}

String KeyValueParser::normalize(String msg)
{
    msg.trim();

    String result = "";
    int start = 0;

    while (start < (int)msg.length()) {
        int spaceIdx = msg.indexOf(' ', start);
        String token;

        if (spaceIdx == -1) {
            token = msg.substring(start);
            start = msg.length();
        } else {
            token = msg.substring(start, spaceIdx);
            start = spaceIdx + 1;
        }

        token.trim();
        if (token.length() == 0) continue; // multiple spaces in a row

        result += token;

        if (token.charAt(token.length() - 1) != ';') {
            result += ';';
        }
    }

    return result;
}

String KeyValueParser::stripDashes(String key)
{
    while (key.length() > 0 && key.charAt(0) == '-') {
        key = key.substring(1);
    }
    return key;
}