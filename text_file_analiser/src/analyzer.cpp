#include "analyzer.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>

bool TextFileAnalyzer::load_stopwords(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;
    std::string w;
    while (std::getline(in, w)) {
        auto norm = normalize_token(w);
        if (!norm.empty()) stop_.put(norm, true);
    }
    return true;
}

bool TextFileAnalyzer::analyze(const AnalyzerConfig& cfg) {
    freq_ = HashTable<std::string,int>();  // reset
    stats_ = FileStats();

    std::ifstream in(cfg.input_path, std::ios::binary);
    if (!in) {
        std::cerr << "Error: cannot open input file: " << cfg.input_path << "\n";
        return false;
    }

    // Count lines & characters as we go; parse with TextParser.
    TextParser parser;

    // We’ll read the file twice: once raw to count lines/chars (streaming),
    // then reopen for parsing (keeps parser simple while staying O(n)).
    {
        char c;
        while (in.get(c)) {
            ++stats_.characters;
            if (c == '\n') ++stats_.lines;
        }
    }
    in.clear(); in.seekg(0);

    auto on_token = [&](const std::string& word) {
        if ((int)word.size() < cfg.min_len) return;
        if (stop_.contains(word)) return;
        freq_.increment(word, 1);
        ++stats_.total_words;
        if (word.size() > stats_.longest_word.size()) stats_.longest_word = word;
    };

    parser.parse(in, on_token);

    // finalize stats
    stats_.unique_words = freq_.size();
    // average word length
    std::size_t sum_len = 0;
    freq_.for_each([&](const std::string& w, int c){
        sum_len += w.size() * static_cast<std::size_t>(c);
    });
    stats_.average_word_length = stats_.total_words ? (double)sum_len / (double)stats_.total_words : 0.0;

    return true;
}

void TextFileAnalyzer::report(std::ostream& out, const AnalyzerConfig& cfg) const {
    out << "== Text File Analyzer Report ==\n";
    out << "Input file      : " << cfg.input_path << "\n";
    if (cfg.stopwords_path) out << "Stopwords       : " << *cfg.stopwords_path << "\n";
    out << "Min word length : " << cfg.min_len << "\n";
    out << "Top N           : " << cfg.topN << "\n\n";

    out << "-- File Statistics --\n";
    out << "Lines                 : " << stats_.lines << "\n";
    out << "Characters (raw)      : " << stats_.characters << "\n";
    out << "Total words (counted) : " << stats_.total_words << "\n";
    out << "Unique words          : " << stats_.unique_words << "\n";
    out << "Average word length   : " << stats_.average_word_length << "\n";
    out << "Longest word          : " << (stats_.longest_word.empty() ? "-" : stats_.longest_word) << "\n\n";

    // Collect & sort topN
    DynamicArray<WordStat> arr;
    arr.reserve(stats_.unique_words ? stats_.unique_words : 8);

    freq_.for_each([&](const std::string& w, int c){
        arr.emplace_back(WordStat{w, c});
    });

    std::sort(arr.begin(), arr.end(), [](const WordStat& a, const WordStat& b){
        if (a.count != b.count) return a.count > b.count; // desc by freq
        return a.word < b.word;                           // asc by word
    });

    out << "-- Top " << cfg.topN << " Words --\n";
    int shown = 0;
    for (auto& ws : arr) {
        if (shown >= cfg.topN) break;
        out << (shown + 1) << ". " << ws.word << " : " << ws.count << "\n";
        ++shown;
    }

    out << "\n(End of report)\n";
}