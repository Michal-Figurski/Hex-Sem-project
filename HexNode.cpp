//
// Created by Michał Figurski on 4/15/2025.
//
#include <SFML/Graphics.hpp>
#include <vector>
#include "HexNode.h"

namespace HEX {
    std::string stateToString(const State &state) {
        switch (state) {
            case State::Empty:
                return "Empty";
            case State::Player1:
                return "Player1";
            case State::Player2:
                return "Player2";
            case State::Inactive:
                return "Inactive";
            default:
                throw std::invalid_argument("Invalid state");
        }
    }

    void HexNode::set_visited(const bool visited) {
        this->visited = visited;
    }

    [[nodiscard]] bool HexNode::isVisited() const {
        return visited;
    }

    auto HexNode::equal(const HEX::HexNode *other) const -> bool {
        return this->index == other->index;
    }

    HexNode::operator std::string() const {
        return std::to_string(this->index) + "column " + std::to_string(this->column) + "row " +
               std::to_string(this->row);
    }

    bool HexNode::operator==(const HexNode *node) const {
        return this->index == node->index;
    }
} // HEX
