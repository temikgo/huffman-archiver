#include "ostreamwrapper.h"

OStreamWrapper::OStreamWrapper(std::ostream& str)
    : str_(str), seq_num_(0), in_seq_num_(0) {
    ClearBuf();
}

OStreamWrapper::~OStreamWrapper() {
    delete[] buf_;
}

void OStreamWrapper::ClearBuf() {
    for (size_t i = 0; i < BYTES_PER_PACKAGE; ++i) {
        buf_[i] = 0;
    }
}

void OStreamWrapper::Put9bPrefix(uint16_t c) {
    for (int i = 0; i < CHAR_BIT - in_seq_num_; ++i) {
        int shift = CHAR_BIT - in_seq_num_ - 2 * i - 1;
        if (shift < 0) {
            if (i + shift < 0) {
                throw std::logic_error("Incorrect shift for bit mask");
            }
            buf_[seq_num_] |= (((1 << i) & c) >> -shift);
        } else {
            if (i + shift >= CHAR_BIT) {
                throw std::logic_error("Incorrect shift for bit mask");
            }
            buf_[seq_num_] |= (((1 << i) & c) << shift);
        }
    }
    if (seq_num_ == BYTES_PER_PACKAGE - 1) {
        seq_num_ = 0;
        str_.write(reinterpret_cast<char*>(buf_), BYTES_PER_PACKAGE);
        ClearBuf();
    } else {
        ++seq_num_;
    }
}

void OStreamWrapper::Put9bSuffix(uint16_t c) {
    uint8_t remained = in_seq_num_ + 1;
    for (int i = BITS_PER_SEQ - remained; i < BITS_PER_SEQ; ++i) {
        int shift = 2 * CHAR_BIT - remained - 2 * i;
        if (shift < 0) {
            if (i + shift < 0) {
                throw std::logic_error("Incorrect shift for bit mask");
            }
            buf_[seq_num_] |= (((1 << i) & c) >> -shift);
        } else {
            if (i + shift >= CHAR_BIT) {
                throw std::logic_error("Incorrect shift for bit mask");
            }
            buf_[seq_num_] |= (((1 << i) & c) << shift);
        }
    }
    if (remained == CHAR_BIT) {
        if (seq_num_ == BYTES_PER_PACKAGE - 1) {
            seq_num_ = 0;
            str_.write(reinterpret_cast<char*>(buf_), BYTES_PER_PACKAGE);
            ClearBuf();
        } else {
            ++seq_num_;
        }
        in_seq_num_ = 0;
    } else {
        in_seq_num_ = remained;
    }
}

std::ostream& OStreamWrapper::Put9b(uint16_t c) {
    Put9bPrefix(c);
    Put9bSuffix(c);
    return str_;
}

size_t OStreamWrapper::PutStringPrefix(std::string& s) {
    size_t i = 0;
    while (s.size() - i >= static_cast<size_t>(CHAR_BIT - in_seq_num_)) {
        for (size_t j = 0; j < static_cast<size_t>(CHAR_BIT - in_seq_num_); ++j) {
            if (s[i + j] == '1') {
                if (CHAR_BIT - j - in_seq_num_ - 1 < 0) {
                    throw std::logic_error("Incorrect shift for bit mask");
                }
                buf_[seq_num_] |= (1 << (CHAR_BIT - j - in_seq_num_ - 1));
            }
        }
        i += CHAR_BIT - in_seq_num_;
        in_seq_num_ = 0;
        if (seq_num_ == CHAR_BIT) {
            seq_num_ = 0;
            str_.write(reinterpret_cast<char*>(buf_), BYTES_PER_PACKAGE);
            ClearBuf();
        } else {
            ++seq_num_;
        }
    }
    return i;
}

void OStreamWrapper::PutStringSuffix(std::string& s, size_t i) {
    for (size_t j = 0; j < s.size() - i; ++j) {
        if (s[i + j] == '1') {
            if (CHAR_BIT - j - in_seq_num_ - 1 < 0) {
                throw std::logic_error("Incorrect shift for bit mask");
            }
            buf_[seq_num_] |= (1 << (CHAR_BIT - j - in_seq_num_ - 1));
        }
    }
    in_seq_num_ += s.size() - i;
}

std::ostream& OStreamWrapper::PutString(std::string& s) {
    size_t i = PutStringPrefix(s);
    PutStringSuffix(s, i);
    return str_;
}

std::ostream& OStreamWrapper::PutTheRest() const {
    str_.write(reinterpret_cast<char*>(buf_), seq_num_ + 1);
    str_.write("\n", 1);
    return str_;
}