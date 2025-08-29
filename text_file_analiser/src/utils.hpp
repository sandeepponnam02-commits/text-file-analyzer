
#pragma once
#include <cctype>
#include <string>
#include <algorithm>

// ASCII-lowercase (simple & fast)
inline char to_lower_ascii(char c) {
    if (c >= 'A' && c <= 'Z') return char(c - 'A' + 'a');
    return c;
}

// Consider letters/digits as core; allow internal ' and - between alnum chars
inline bool is_word_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

inline bool is_space(char c) {
    return std::isspace(static_cast<unsigned char>(c)) != 0;
}

// Normalize word: lowercase; strip leading/trailing non-alnum; keep internal ' or - (e.g., don't, state-of-the-art)
inline std::string normalize_token(const std::string& raw) {
    if (raw.empty()) return {};
    // find first/last alnum
    size_t i = 0, j = raw.size();
    while (i < j && !is_word_char(raw[i])) ++i;
    while (j > i && !is_word_char(raw[j-1])) --j;
    if (i >= j) return {};
    std::string s = raw.substr(i, j - i);
    // collapse leading/trailing apostrophes/hyphens again (if trimming exposed them)
    while (!s.empty() && !is_word_char(s.front())) s.erase(s.begin());
    while (!s.empty() && !is_word_char(s.back())) s.pop_back();
    // lowercase ASCII
    for (char& c : s) c = to_lower_ascii(c);
    return s;
}