text_parser.hpp
#pragma once
#include <istream>
#include <string>
#include "utils.hpp"

class TextParser {
public:
    // Feed an input stream, call on_token(word) for each normalized word.
    template <typename F>
    void parse(std::istream& in, F on_token) {
        std::string token;
        token.reserve(32);

        char c;
        while (in.get(c)) {
            // Build tokens allowing internal ' and - when surrounded by alnum
            if (is_word_char(c) || c == '\'' || c == '-') {
                token.push_back(c);
            } else {
                if (!token.empty()) {
                    auto norm = normalize_token(token);
                    if (!norm.empty()) on_token(norm);
                    token.clear();
                }
            }
        }
        if (!token.empty()) {
            auto norm = normalize_token(token);
            if (!norm.empty()) on_token(norm);
        }
    }
};