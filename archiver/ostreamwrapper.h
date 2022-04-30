#ifndef CPP_PILOT_HSE_OSTREAMWRAPPER_H
#define CPP_PILOT_HSE_OSTREAMWRAPPER_H

#include <cstdint>
#include <iostream>
#include <string>

class OStreamWrapper {
public:
    static const int BITS_PER_SEQ = 9;
    static const int BYTES_PER_PACKAGE = 9;

    OStreamWrapper(std::ostream& str);
    ~OStreamWrapper();

    void ClearBuf();
    void Put9bPrefix(uint16_t c);
    void Put9bSuffix(uint16_t c);
    std::ostream& Put9b(uint16_t c);
    size_t PutStringPrefix(std::string& s);
    void PutStringSuffix(std::string& s, size_t i);
    std::ostream& PutString(std::string& s);
    std::ostream& PutTheRest() const;

protected:
    std::ostream& str_;
    unsigned char* buf_ = new unsigned char[BYTES_PER_PACKAGE];
    uint8_t seq_num_;
    uint8_t in_seq_num_;
};

#endif  // CPP_PILOT_HSE_OSTREAMWRAPPER_H
