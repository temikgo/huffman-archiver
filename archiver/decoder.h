#ifndef CPP_PILOT_HSE_DECODER_H
#define CPP_PILOT_HSE_DECODER_H

#include <cstdint>
#include <memory>
#include <string>

#include "istreamwrapper.h"
#include "node.h"

class Decoder {
public:

    Decoder(const char* archive_name);

    void Decode();
    void ReadTrieInfo(IStreamWrapper& str, std::vector<uint16_t>& symbols,
                      std::vector<uint16_t>& with_length_count);
    void AddNode(std::string& code, uint16_t symbol);
    void BuildTrie(IStreamWrapper& str, std::vector<uint16_t>& symbols,
                   std::vector<uint16_t>& with_length_count);
    bool Write(IStreamWrapper& str) const;

private:
    std::string archive_name_;
    std::shared_ptr<Node> trie_root_;
};

#endif  // CPP_PILOT_HSE_DECODER_H
