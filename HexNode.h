#ifndef HEXGRAPH_H
#define HEXGRAPH_H
#include <SFML/Graphics.hpp>
#include <vector>

namespace HEX {
    enum class State {
        Inactive = 0,
        Empty = 1,
        Player1 = 2,
        Player2 = 3
    };

    auto stateToString(const State &state) -> std::string;

    class HexNode {
    public:
        int row{}, column{};
        int index{};
        std::vector<HEX::HexNode *> neighbors;
        sf::CircleShape hex = sf::CircleShape(40, 6);
        State state{};

        HexNode(int count, float screenWidth, float screenHeight) {
            hex = sf::CircleShape(screenHeight / 27, 6);
            this->visited = false;
            this->index = count;
            this->neighbors = std::vector<HEX::HexNode *>();
            this->state = State::Empty;
            hex.setOutlineColor(sf::Color::Red);
        }

        void set_visited(bool visited);

        [[nodiscard]] bool isVisited() const;

        auto equal(const HEX::HexNode *other) const -> bool;

        explicit operator std::string() const;

        bool operator==(const HexNode *node) const;

    private:
        bool visited{};
    };
} // HEX

#endif //HEXGRAPH_H
