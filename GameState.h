//
// Created by Michał Figurski on 4/9/2025.
//

#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <HexNode.h>
#include <iostream>
#include <map>
#include <notification.h>
#include <queue>

namespace HEX {
    enum class Turn {
        Null = 0,
        Player1 = 2,
        Player2 = 3,
        Player1Won = 4,
        Player2Won = 5,
        Draw = 6
    };

    class Maps {
    public:
        static std::map<HEX::State, sf::Color> color() {
            std::map<HEX::State, sf::Color> map;
            map[HEX::State::Empty] = sf::Color::White;
            map[HEX::State::Player1] = sf::Color::Blue;
            map[HEX::State::Player2] = sf::Color::Green;
            map[HEX::State::Inactive] = sf::Color::Black;
            return map;
        }

        static Turn stateToTurn(State state) {
            if (state == State::Player1) return Turn::Player1;
            if (state == State::Player2) return Turn::Player2;
            return Turn::Null;
        }
    };

    /// Gamestate(int, int) - constructor for custom sized boards (TODO)
    ///Gamestate() - constructor for default board (hexxagon.com)
    ///nodes - vector of all HexNode objects
    ///numberOfHexes - number of hexes
    ///size - unused
    ///longestRow
    class GameState {
    public:
        std::vector<HEX::HexNode> nodes;
        bool isGameInProgress = false;
        bool isGameOverVar = false;
        bool AI = false;
        int moves = 0;
        //length of the longest row
        //constructors

        GameState(bool isAI, float screenWidth, float screenHeight); //default constructor;

        auto setSelected(int nodeIndex) -> void; //make a hex selected
        auto clearSelected() -> void; //clear all hexes' selected
        auto isGameOver() -> bool; //are game over conditions met
        auto stats() -> std::pair<int, int>; //how many hexes each player has
        auto changeTurn() -> void; //next player's turn
        auto startingPosition() -> void; //default position of hexes and inactive fields
        auto defaultHexPositions(float windowWidth, float windowHeight) -> void; //hexes' position on the screen
        auto newGame(float windowWidth, float windowHeight) -> void; //start a new game
        auto importPositions(const std::vector<State> &states) -> std::optional<notification>;

        auto evaluateResult() -> void;

        /*
         * * dist() -> int - returns distance between two HexNodes
        */
        auto dist(const HexNode &src, const HexNode &dest) -> int;

        [[nodiscard]] Turn turn1() const;

        void set_turn(Turn turn);

    private:
        int numberOfHexes = 0;
        int longestRow = 0;
        Turn turn = Turn::Null;

        /*
         * buildGraph() - ?util for constructors? builds neighbour vectors for each HexNode
         */
        auto buildGraph() -> void;

        /*
        *  bfs() - util for dist() returns distance between two HexNodes !!Must clearVisited() after every call!!
        */
        auto bfs(const HexNode &src, const HexNode &dest) -> int;

        /*
         *  clearVisited() - util for dist() clears visited field for every HexNode
         */
        auto clearVisited() -> void;

        /*
        * isNeighbour() - util for buildGraph() (nodes' neighbour vectors should be used to reference neighbours after construction)
        */
        static auto isNeighbour(const HEX::HexNode &a, const HEX::HexNode &b) -> bool;
    };
}
#endif //GAMESTATE_H
