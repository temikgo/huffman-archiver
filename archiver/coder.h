#ifndef CPP_PILOT_HSE_CODER_H
#define CPP_PILOT_HSE_CODER_H

#include <cstdint>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "node.h"
#include "ostreamwrapper.h"

class Coder {
public:
    using HeapNode = std::pair<std::shared_ptr<Node>, int>;

    Coder(size_t size, char* file_names[]);

    void Code();
    void BuildHeap(const std::string& file_name, std::priority_queue<HeapNode>& huffman_heap);
    void BuildTrie(const std::string& file_name);
    std::vector<SymbolCode> GetCodes() const;
    void Initialize(std::vector<SymbolCode>& codes,
                    std::vector<SymbolCode>& canonical_codes,
                    std::vector<uint16_t>& with_length_count,
                    std::vector<std::string>& symbol_code,
                    std::string& current_code) const;
    std::vector<SymbolCode> ToCanonicalForm(std::vector<SymbolCode>& codes,
                                            std::vector<uint16_t>& with_length_count,
                                            std::vector<std::string>& symbol_code) const;
    void WriteFile(OStreamWrapper& str, size_t file_number, std::vector<std::string>& symbol_code) const;
    void Write(OStreamWrapper& str, size_t file_number, std::vector<uint16_t>& with_length_count,
               std::vector<std::string>& symbol_code, std::vector<SymbolCode>& canonical_codes) const;

private:
    std::string archive_name_;
    std::vector<std::string> files_;
    std::shared_ptr<Node> trie_root_;
};

#endif  // CPP_PILOT_HSE_CODER_H
