#include "istreamwrapper.h"

IStreamWrapper::IStreamWrapper(std::istream& str)
    : str_(str), seq_num_(0), in_seq_num_(0) {
    ClearBuf();
    Get();
}

IStreamWrapper::~IStreamWrapper() {
    delete[] buf_;
}

void IStreamWrapper::ClearBuf() {
    for (size_t i = 0; i < BYTES_PER_PACKAGE; ++i) {
        buf_[i] = 0;
    }
}

std::istream& IStreamWrapper::Get() {
    char new_byte;
    str_.get(new_byte);
    buf_[seq_num_] = new_byte;
    return str_;
}

void IStreamWrapper::Get9bPrefix(uint16_t& x) {
    x = 0;
    for (size_t i = in_seq_num_; i < CHAR_BIT; ++i) {
        int shift = CHAR_BIT + in_seq_num_ - 2 * i - 1;
        if (CHAR_BIT - i - shift - 1 < 0 || CHAR_BIT - i - shift - 1 > CHAR_BIT) {
            throw std::logic_error("Incorrect shift for bit mask");
        }
        if (shift < 0) {
            x |= ((buf_[seq_num_] & (1 << (CHAR_BIT - i - 1))) << -shift);
        } else {
            x |= ((buf_[seq_num_] & (1 << (CHAR_BIT - i - 1))) >> shift);
        }
    }
    if (seq_num_ == BYTES_PER_PACKAGE - 1) {
        seq_num_ = 0;
        ClearBuf();
    } else {
        ++seq_num_;
    }
    Get();
}

void IStreamWrapper::Get9bSuffix(uint16_t& x) {
    uint8_t remained = in_seq_num_ + 1;
    for (size_t i = 0; i < remained; ++i) {
        int shift = remained - 2 * i - 2;
        if (CHAR_BIT - i - shift - 1 < 0 || CHAR_BIT - i - shift - 1 > CHAR_BIT) {
            throw std::logic_error("Incorrect shift for bit mask");
        }
        if (shift < 0) {
            x |= ((buf_[seq_num_] & (1 << (CHAR_BIT - i - 1))) << -shift);
        } else {
            x |= ((buf_[seq_num_] & (1 << (CHAR_BIT - i - 1))) >> shift);
        }
    }
    if (remained == CHAR_BIT) {
        if (seq_num_ == BYTES_PER_PACKAGE - 1) {
            seq_num_ = 0;
            ClearBuf();
        } else {
            ++seq_num_;
        }
        in_seq_num_ = 0;
        Get();
    } else {
        in_seq_num_ = remained;
    }
}

std::istream& IStreamWrapper::Get9b(uint16_t& x) {
    Get9bPrefix(x);
    Get9bSuffix(x);
    return str_;
}

std::istream& IStreamWrapper::Get1b(char &c) {
    if (buf_[seq_num_] & (1 << (CHAR_BIT - in_seq_num_ - 1))) {
        c = '1';
    } else {
        c = '0';
    }
    if (in_seq_num_ == CHAR_BIT - 1) {
        in_seq_num_ = 0;
        if (seq_num_ == BYTES_PER_PACKAGE - 1) {
            seq_num_ = 0;
            ClearBuf();
        } else {
            ++seq_num_;
        }
        Get();
    } else {
        ++in_seq_num_;
    }
    return str_;
}

std::istream& IStreamWrapper::GetSymbol(uint16_t& symbol, std::shared_ptr<Node> current_node) {
    char bit;
    while (!current_node->GetIsTerm()) {
        Get1b(bit);
        if (bit == '0') {
            current_node = current_node->left_;
        } else {
            current_node = current_node->right_;
        }
    }
    symbol = current_node->GetSymbol();
    return str_;
}