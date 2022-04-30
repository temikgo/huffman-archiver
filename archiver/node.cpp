#include "node.h"

Node::Node(uint16_t symbol)
    : is_term_(true), symbol_(symbol) {
}

Node::Node(std::shared_ptr<Node> left, std::shared_ptr<Node> right)
    : left_(left), right_(right) {
}

bool Node::GetIsTerm() const {
    return is_term_;
}

uint16_t Node::GetSymbol() const {
    return symbol_;
}
