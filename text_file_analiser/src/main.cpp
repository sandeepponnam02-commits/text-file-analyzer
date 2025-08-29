// src/main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "analyzer.hpp"

static void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog << " <input.txt> [--top N] [--min-length M] [--stopwords FILE] [--out FILE]\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    AnalyzerConfig cfg;
    cfg.input_path = argv[1];

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--top" && i + 1 < argc) {
            cfg.topN = std::atoi(argv[++i]);
            if (cfg.topN <= 0) cfg.topN = 20;
        } else if (arg == "--min-length" && i + 1 < argc) {
            cfg.min_len = std::atoi(argv[++i]);
            if (cfg.min_len < 1) cfg.min_len = 1;
        } else if (arg == "--stopwords" && i + 1 < argc) {
            cfg.stopwords_path = argv[++i];
        } else if (arg == "--out" && i + 1 < argc) {
            cfg.out_path = argv[++i];
        } else {
            std::cerr << "Unknown or incomplete option: " << arg << "\n";
            print_usage(argv[0]);
            return 1;
        }
    }

    TextFileAnalyzer analyzer;

    if (cfg.stopwords_path) {
        if (!analyzer.load_stopwords(*cfg.stopwords_path)) {
            std::cerr << "Warning: could not load stopwords from " << *cfg.stopwords_path << "\n";
        }
    }

    if (!analyzer.analyze(cfg)) {
        return 1;
    }

    if (cfg.out_path) {
        std::ofstream out(*cfg.out_path);
        if (!out) {
            std::cerr << "Error: cannot open output file: " << *cfg.out_path << "\n";
            return 1;
        }
        analyzer.report(out, cfg);
        std::cout << "Report written to: " << *cfg.out_path << "\n";
    } else {
        analyzer.report(std::cout, cfg);
    }

    return 0;
}