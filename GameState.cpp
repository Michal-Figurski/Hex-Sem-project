//
// Created by Michał Figurski on 4/15/2025.
//
#include "GameState.h"
#include "HexNode.h"
#include <map>
#include <notification.h>
#include <queue>
#include <TurnHandler.h>

namespace HEX {
    /// Gamestate(int, int) - constructor for custom sized boards (TODO)
    ///Gamestate() - constructor for default board (hexxagon.com)
    ///nodes - vector of all HexNode objects
    ///numberOfHexes - number of hexes
    ///size - unused
    ///longestRow


    GameState::GameState(const bool isAI, const float screenWidth, const float screenHeight) : nodes(std::vector<HEX::HexNode>()), AI(isAI), numberOfHexes(61), longestRow(9) {
        for (int i = 0; i < numberOfHexes; i++) {
            this->nodes.emplace_back(i, screenWidth, screenHeight);
        }
        int rowSize = 4;
        int index = 0;
        /*Generates column and rows
         *(mind that there are twice as many columns as hexes in the longset row, to account for those that are not aligned)
         *both rows and columns ordered from 0
         */
        for (int i = 0; i < 9; i++) {
            if (i > 4)
                rowSize--;
            else
                rowSize++;
            int firstColumnIndexInCurrRow = 9 - rowSize + 1;
            for (int j = 0; j < rowSize; j++) {
                this->nodes[index].row = i;
                this->nodes[index].column = firstColumnIndexInCurrRow - 1;
                index++;
                firstColumnIndexInCurrRow += 2;
            }
        }
        //build HexNodes' neighbour vectors
        this->buildGraph();
    }

    auto GameState::setSelected(const int nodeIndex) -> void {
        this->nodes[nodeIndex].hex.setOutlineColor(sf::Color::Cyan);
        this->nodes[nodeIndex].hex.setOutlineThickness(4);
    }

    auto GameState::clearSelected() -> void {
        for (auto &i: this->nodes) {
            i.hex.setOutlineThickness(0);
        }
    }

    auto GameState::isGameOver() -> bool {
        if (!isGameInProgress)
            return false;
        if (const auto move = TurnHandler::findBestMove(*this, this->turn); move.first==nullptr&&move.second==nullptr)
            return true;
        auto [player1Result, player2Result] = stats();
        if (player1Result == 0 || player2Result == 0) return true; //if any of the players doesn't have any hexes left
        for (const auto &i: this->nodes) {
            if (i.state == State::Empty)
                return false;
        }
        return true;
    }

    auto GameState::stats() -> std::pair<int, int> {
        int player1 = 0, player2 = 0;
        for (const auto &i: this->nodes) {
            if (i.state == State::Player1)
                player1++;
            if (i.state == State::Player2)
                player2++;
        }
        return std::make_pair(player1, player2);
    }

    auto GameState::changeTurn() -> void {
        if (turn == Turn::Player1)
            turn = Turn::Player2;
        else if (turn == Turn::Player2)
            turn = Turn::Player1;
    }

    auto GameState::startingPosition() -> void {
        for (auto &i: this->nodes) {
            i.state = State::Empty;
        }
        const auto inactiveFields = std::vector<int>{22, 29, 39};
        const auto P1Fields = std::vector<int>{0, 34, 56};
        const auto P2Fields = std::vector<int>{4, 26, 60};
        for (const auto i: inactiveFields)
            this->nodes[i].state = State::Inactive;
        for (const auto i: P1Fields)
            this->nodes[i].state = State::Player1;
        for (const auto i: P2Fields)
            this->nodes[i].state = State::Player2;
    }

    auto GameState::defaultHexPositions(const float windowWidth, const float windowHeight) -> void {
        auto *gameState = this;
        const sf::Angle angle = sf::degrees(30);
        for (auto &i: gameState->nodes) {
            const float startX = windowWidth / 3 + i.column * windowWidth / 50 * 1.3;
            const float startY = windowHeight / 5 + i.row * windowHeight / 16.5 * 1.3;
            const float x_rotated = ((startX - windowWidth / 2.0) * std::cos(angle.asRadians()) - (
                                         (windowHeight / 2.0 - startY) * std::sin(angle.asRadians())) + windowWidth /
                                     2.0);
            const float y_rotated = windowHeight / 2.0 - (
                                        (windowHeight / 2.0 - startY) * std::cos(angle.asRadians()) + (
                                            (startX - windowWidth / 2.0) * std::sin(angle.asRadians())));
            // i.hex.setPosition({((static_cast<float>(600+i.column*38), static_cast<float>(200+i.row*65)});
            i.hex.setOrigin(sf::Vector2f(40, 40));
            i.hex.setPosition({-100 + x_rotated, y_rotated});
            i.hex.setRotation(sf::degrees(0));
            i.hex.rotate(angle);
        }
    }

    auto GameState::dist(const HexNode &src, const HexNode &dest) -> int {
        const int res = bfs(src, dest);
        clearVisited();
        return res;
    }

    auto GameState::newGame(const float windowWidth, const float windowHeight) -> void {
        this->startingPosition();
        this->defaultHexPositions(windowWidth, windowHeight);
        this->clearSelected();
        this->turn = Turn::Player1;
        this->isGameInProgress = true;
        this->isGameOverVar = false;
        this->moves = 0;
    }

    [[nodiscard]] Turn GameState::turn1() const {
        return turn;
    }

    void GameState::set_turn(Turn turn) {
        this->turn = turn;
    }

    auto GameState::buildGraph() -> void {
        for (auto &i: nodes) {
            for (auto &j: nodes) {
                if (isNeighbour(i, j)) {
                    i.neighbors.push_back(&j);
                }
            }
        }
    }

    auto GameState::bfs(const HexNode &src, const HexNode &dest) -> int {
        std::queue<std::pair<const HexNode *, int> > queue; // int: HexNode.index int: depth
        queue.emplace(&src, 0);
        while (!queue.empty()) {
            const HexNode *node = queue.front().first;
            const int depth = queue.front().second;
            if (this->nodes[node->index].isVisited()) {
                queue.pop();
                continue;
            }
            if (node == dest) return depth;
            this->nodes[node->index].set_visited(true);
            for (auto &neighbour: this->nodes[node->index].neighbors) {
                if (!neighbour->isVisited())
                    queue.emplace(neighbour, depth + 1);
            }
            queue.pop();
        }
        return -1;
    }

    auto GameState::clearVisited() -> void {
        for (auto &i: nodes) {
            i.set_visited(false);
        }
    }

    auto GameState::isNeighbour(const HEX::HexNode &a, const HEX::HexNode &b) -> bool {
        if (a.index == b.index)
            return false;
        return std::abs(a.column - b.column) <= 2 && std::abs(a.row - b.row) <= 1;
    }

    auto GameState::importPositions(const std::vector<State> &states) -> std::optional<notification> {
        const auto inactiveFields = std::vector{22, 29, 39};
        if (states.size() != 61)
            return notification(notificationTypes::Error, "Wrong amount of hexes");
        for (const auto &i: inactiveFields)
            if (states[i] != State::Inactive) {
                return notification(notificationTypes::Error, "Invalid state of inactive hex");
            }
        for (int i = 0; i < states.size(); i++) {
            nodes[i].state = states[i];
        }
        this->moves = 0;
        return std::nullopt;
    }

    auto GameState::evaluateResult() -> void {
        assert(this->isGameOver());
        auto [Player1Result, Player2Result] = this->stats();
        if (Player1Result > Player2Result) {
            std::cout << "Player1 wins";
            this->set_turn(HEX::Turn::Player1Won);
        } else if (Player1Result < Player2Result) {
            std::cout << "Player2 wins";
            this->set_turn(HEX::Turn::Player2Won);
        } else {
            std::cout << "Remis";
            this->set_turn(HEX::Turn::Draw);
        }
    }
}
