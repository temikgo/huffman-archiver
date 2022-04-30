#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

#include "decoder.h"
#include "istreamwrapper.h"
#include "node.h"

static const int MAX_CHAR_NUMBER = 258;

Decoder::Decoder(const char* archive_name)
    : archive_name_(archive_name) {
}

void Decoder::ReadTrieInfo(IStreamWrapper& str, std::vector<uint16_t>& symbols,
                           std::vector<uint16_t>& with_length_count) {
    uint16_t number_symbols;
    str.Get9b(number_symbols);
    symbols.resize(number_symbols);
    for (size_t i = 0; i < symbols.size(); ++i) {
        str.Get9b(symbols[i]);
    }
    uint16_t total_count = 0;
    while (total_count < number_symbols) {
        with_length_count.push_back(0);
        str.Get9b(with_length_count.back());
        total_count += with_length_count.back();
    }
}

void Decoder::AddNode(std::string& code, uint16_t symbol) {
    std::shared_ptr<Node> current_node = trie_root_;
    for (size_t i = 0; i < code.size(); ++i) {
        if (code[i] == '0') {
            if (current_node->left_ == nullptr) {
                if (i == code.size() - 1) {
                    current_node->left_ = std::shared_ptr<Node>(new Node(symbol));
                } else {
                    current_node->left_ = std::shared_ptr<Node>(new Node());
                }
            }
            current_node = current_node->left_;
        } else {
            if (current_node->right_ == nullptr) {
                if (i == code.size() - 1) {
                    current_node->right_ = std::shared_ptr<Node>(new Node(symbol));
                } else {
                    current_node->right_ = std::shared_ptr<Node>(new Node());
                }
            }
            current_node = current_node->right_;
        }
    }
}

void Decoder::BuildTrie(IStreamWrapper& str, std::vector<uint16_t>& symbols,
                        std::vector<uint16_t>& with_length_count) {
    trie_root_ = std::shared_ptr<Node>(new Node());
    size_t current_symbol = 0;
    std::string current_code;
    for (size_t current_length = 1; current_length <= with_length_count.size(); ++current_length) {
        current_code += '0';
        for (size_t i = 0; i < with_length_count[current_length - 1]; ++i) {
            if (static_cast<size_t>(current_symbol) >= symbols.size()) {
                throw std::out_of_range("Vector index is out of range (incorrect symbol)");
            }
            AddNode(current_code, symbols[current_symbol]);
            ++current_symbol;
            if (current_symbol < symbols.size()) {
                size_t j = current_code.size();
                while (current_code[--j] == '1') {
                    current_code[j] = '0';
                    if (!j) {
                        throw std::logic_error("Canonical from build error");
                    }
                }
                current_code[j] = '1';
            }
        }
    }
}

bool Decoder::Write(IStreamWrapper& str) const {
    uint16_t symbol = 0;
    while (symbol < MAX_CHAR_NUMBER - 1) {
        std::string file_name;
        str.GetSymbol(symbol, trie_root_);
        while (symbol != MAX_CHAR_NUMBER - 2) {
            file_name += symbol;
            str.GetSymbol(symbol, trie_root_);
        }
        std::fstream fout(file_name, std::ios_base::binary | std::ios_base::out);
        if (!fout.is_open()) {
            throw std::runtime_error("File could not be opened for writing");
        }
        str.GetSymbol(symbol, trie_root_);
        while (symbol < MAX_CHAR_NUMBER - 1) {
            fout.write(reinterpret_cast<char*>(&symbol), 1);
            str.GetSymbol(symbol, trie_root_);
        }
        fout.close();
    }
    return symbol == MAX_CHAR_NUMBER;
}

void Decoder::Decode() {
    std::fstream fin(archive_name_, std::ios_base::binary | std::ios_base::in);
    if (!fin.is_open()) {
        throw std::runtime_error("File could not be opened for reading");
    }
    IStreamWrapper istream9(fin);
    bool is_archive_ended = false;
    do {
        std::vector<uint16_t> symbols;
        std::vector<uint16_t> with_length_count;
        ReadTrieInfo(istream9, symbols, with_length_count);
        BuildTrie(istream9, symbols, with_length_count);
        is_archive_ended = Write(istream9);
    } while (!is_archive_ended);
}