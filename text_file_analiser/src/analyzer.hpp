// src/analyzer.hpp
#pragma once
#include <string>
#include <fstream>
#include <optional>
#include <limits>
#include "hash_table.hpp"
#include "dyn_array.hpp"
#include "text_parser.hpp"

struct AnalyzerConfig {
    std::string input_path;
    std::optional<std::string> stopwords_path;
    std::optional<std::string> out_path;
    int topN = 20;
    int min_len = 1;
};

struct WordStat {
    std::string word;
    int count = 0;
};

struct FileStats {
    std::size_t lines = 0;
    std::size_t characters = 0;  // raw characters read
    std::size_t total_words = 0; // after filtering/normalization
    std::size_t unique_words = 0;
    double average_word_length = 0.0;
    std::string longest_word;
};

class TextFileAnalyzer {
public:
    bool load_stopwords(const std::string& path);
    bool analyze(const AnalyzerConfig& cfg);
    void report(std::ostream& out, const AnalyzerConfig& cfg) const;

private:
    HashTable<std::string, int> freq_;
    HashTable<std::string, bool> stop_;
    FileStats stats_;
};