#include <algorithm>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "coder.h"
#include "node.h"
#include "ostreamwrapper.h"

static const int MAX_CHAR_NUMBER = 258;

Coder::Coder(size_t size, char* file_names[]):
    archive_name_(file_names[2]) {
    files_.resize(size - 3);
    for (size_t i = 0; i < size - 3; ++i) {
        files_[i] = file_names[i + 3];
    }
}

bool operator <(const Coder::HeapNode& a, const Coder::HeapNode& b) {
    return a.second > b.second;
}

void Coder::BuildHeap(const std::string& file_name, std::priority_queue<HeapNode>& huffman_heap) {
    std::fstream fin(file_name, std::ios_base::binary | std::ios_base::in);
    if (!fin.is_open()) {
        throw std::runtime_error("File could not be opened for reading");
    }
    std::vector<int> symbol_count(MAX_CHAR_NUMBER + 1);
    for (const char c : file_name) {
        if (static_cast<uint16_t>(c) > MAX_CHAR_NUMBER) {
            throw std::out_of_range("Vector index is out of range (incorrect symbol)");
        }
        ++symbol_count[static_cast<uint16_t>(c)];
    }
    char symbol;
    while (fin.get(symbol)) {
        if (static_cast<uint16_t>(symbol) > MAX_CHAR_NUMBER) {
            throw std::out_of_range("Vector index is out of range (incorrect symbol)");
        }
        ++symbol_count[static_cast<uint16_t>(symbol)];
    }
    for (size_t i = MAX_CHAR_NUMBER - 2; i <= MAX_CHAR_NUMBER; ++i) {
        ++symbol_count[i];
    }
    for (uint16_t i = 0; i <= MAX_CHAR_NUMBER; ++i) {
        if (symbol_count[i]) {
            huffman_heap.push({std::shared_ptr<Node>(new Node(i)), symbol_count[i]});
        }
    }
    fin.close();
}

void Coder::BuildTrie(const std::string& file_name) {
    std::priority_queue<HeapNode> huffman_heap;
    BuildHeap(file_name, huffman_heap);
    while (huffman_heap.size() > 1) {
        std::shared_ptr<Node> left_node = huffman_heap.top().first;
        int left_count = huffman_heap.top().second;
        huffman_heap.pop();
        std::shared_ptr<Node> right_node = huffman_heap.top().first;
        int right_count = huffman_heap.top().second;
        huffman_heap.pop();
        huffman_heap.push({std::shared_ptr<Node>(new Node(left_node, right_node)), left_count + right_count});
    }
    trie_root_ = huffman_heap.top().first;
}

void TrieDfs(const std::shared_ptr<Node> node_ptr, std::string& current_code, std::vector<SymbolCode>& codes) {
    if (node_ptr->GetIsTerm()) {
        codes.push_back({current_code, node_ptr->GetSymbol()});
        return;
    }
    current_code += '0';
    TrieDfs(node_ptr->left_, current_code, codes);
    current_code.pop_back();
    current_code += '1';
    TrieDfs(node_ptr->right_, current_code, codes);
    current_code.pop_back();
}

std::vector<SymbolCode> Coder::GetCodes() const {
    std::vector<SymbolCode> codes;
    std::string current_code;
    TrieDfs(trie_root_, current_code, codes);
    return codes;
}

bool LengthCharCmp(const SymbolCode& a, const SymbolCode& b) {
    if (a.first.size() == b.first.size()) {
        return a.second < b.second;
    }
    return a.first.size() < b.first.size();
}

void Coder::Initialize(std::vector<SymbolCode>& codes,
                       std::vector<SymbolCode>& canonical_codes,
                       std::vector<uint16_t>& with_length_count,
                       std::vector<std::string>& symbol_code,
                       std::string& current_code) const {
    std::sort(codes.begin(), codes.end(), LengthCharCmp);
    for (size_t i = 0; i < codes[0].first.size(); ++i) {
        current_code += '0';
    }
    canonical_codes[0] = {current_code, codes[0].second};
    for (size_t i = 1; i < codes[0].first.size(); ++i) {
        with_length_count.push_back(0);
    }
    with_length_count.push_back(1);
    if (static_cast<size_t>(codes[0].second) >= symbol_code.size()) {
        throw std::out_of_range("Vector index is out of range (incorrect symbol)");
    }
    symbol_code[codes[0].second] = current_code;
}

std::vector<SymbolCode> Coder::ToCanonicalForm(std::vector<SymbolCode>& codes,
                                               std::vector<uint16_t>& with_length_count,
                                               std::vector<std::string>& symbol_code) const {
    std::vector<SymbolCode> canonical_codes(codes.size());
    std::string current_code;
    Initialize(codes, canonical_codes, with_length_count, symbol_code, current_code);
    for (size_t i = 1; i < codes.size(); ++i) {
        size_t j = current_code.size();
        while (current_code[--j] == '1') {
            current_code[j] = '0';
            if (!j) {
                throw std::logic_error("Canonical from build error");
            }
        }
        current_code[j] = '1';
        for (j = codes[i - 1].first.size(); j < codes[i].first.size(); ++j) {
            current_code += '0';
            with_length_count.push_back(0);
        }
        ++with_length_count.back();
        canonical_codes[i] = {current_code, codes[i].second};
        if (static_cast<size_t>(codes[i].second) >= symbol_code.size()) {
            throw std::out_of_range("Vector index is out of range (incorrect symbol)");
        }
        symbol_code[codes[i].second] = current_code;
    }
    return canonical_codes;
}

void Coder::WriteFile(OStreamWrapper& str, size_t file_number, std::vector<std::string>& symbol_code) const {
    std::fstream fin(files_[file_number], std::ios_base::binary | std::ios_base::in);
    if (!fin.is_open()) {
        throw std::runtime_error("File could not be opened for reading");
    }
    char symbol;
    while (fin.get(symbol)) {
        if (static_cast<size_t>(symbol) >= symbol_code.size()) {
            throw std::out_of_range("Vector index is out of range (incorrect symbol)");
        }
        str.PutString(symbol_code[symbol]);
    }
    fin.close();
}

void Coder::Write(OStreamWrapper& str, size_t file_number, std::vector<uint16_t>& with_length_count,
                  std::vector<std::string>& symbol_code, std::vector<SymbolCode>& canonical_codes) const {
    str.Put9b(canonical_codes.size());
    for (const SymbolCode& p : canonical_codes) {
        str.Put9b(p.second);
    }
    for (const uint16_t number : with_length_count) {
        str.Put9b(number);
    }
    for (const char c : files_[file_number]) {
        str.PutString(symbol_code[c]);
    }
    str.PutString(symbol_code[MAX_CHAR_NUMBER - 2]);
    WriteFile(str, file_number, symbol_code);
    if (file_number < files_.size() - 1) {
        str.PutString(symbol_code[MAX_CHAR_NUMBER - 1]);
    } else {
        str.PutString(symbol_code[MAX_CHAR_NUMBER]);
    }
}

void Coder::Code() {
    std::fstream fout(archive_name_, std::ios_base::binary | std::ios_base::out);
    if (!fout.is_open()) {
        throw std::runtime_error("File could not be opened for writing");
    }
    OStreamWrapper ostream9(fout);
    for (size_t i = 0; i < files_.size(); ++i) {
        BuildTrie(files_[i]);
        std::vector<SymbolCode> symbol_codes = GetCodes();
        std::vector<uint16_t> with_length_count;
        std::vector<std::string> symbol_code(MAX_CHAR_NUMBER + 1);
        std::vector<SymbolCode> canonical_codes =
                ToCanonicalForm(symbol_codes, with_length_count, symbol_code);
        Write(ostream9, i, with_length_count, symbol_code, canonical_codes);
    }
    ostream9.PutTheRest();
    fout.close();
}