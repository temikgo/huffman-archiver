#ifndef CPP_PILOT_HSE_ISTREAMWRAPPER_H
#define CPP_PILOT_HSE_ISTREAMWRAPPER_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "node.h"

class IStreamWrapper {
public:
    static const int BYTES_PER_PACKAGE = 9;

    IStreamWrapper(std::istream& str);
    ~IStreamWrapper();

    void ClearBuf();
    std::istream& Get();
    void Get9bPrefix(uint16_t& x);
    void Get9bSuffix(uint16_t& x);
    std::istream& Get9b(uint16_t& x);
    std::istream& Get1b(char& c);
    std::istream& GetSymbol(uint16_t &symbol, std::shared_ptr<Node> current_node);

protected:
    std::istream& str_;
    unsigned char* buf_ = new unsigned char[BYTES_PER_PACKAGE];
    uint8_t seq_num_;
    uint8_t in_seq_num_;
};

#endif  // CPP_PILOT_HSE_ISTREAMWRAPPER_H
