#ifndef CPP_PILOT_HSE_NODE_H
#define CPP_PILOT_HSE_NODE_H

#include <cstdint>
#include <memory>
#include <vector>

using SymbolCode = std::pair<std::string, uint16_t>;

class Node {
public:
    Node() = default;
    explicit Node(uint16_t symbol);
    Node(std::shared_ptr<Node> left, std::shared_ptr<Node> right);

    bool GetIsTerm() const;
    uint16_t GetSymbol() const;

public:
    std::shared_ptr<Node> left_ = nullptr;
    std::shared_ptr<Node> right_ = nullptr;

private:
    bool is_term_ = false;
    uint16_t symbol_;
};


#endif  // CPP_PILOT_HSE_NODE_H
