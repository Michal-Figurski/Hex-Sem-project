#include "TurnHandler.h"

#include <iostream>
#include <set>

auto HEX::TurnHandler::getTurn(const GameState &gameState) -> Turn {
    return gameState.turn1();
}

void HEX::TurnHandler::convertNeighbors(const HexNode &node) {
    for (auto &i: node.neighbors) {
        if (i->state != State::Empty && i->state != State::Inactive) {
            i->state = node.state;
        }
    }
}

auto HEX::TurnHandler::clone(const HexNode &src, HexNode &dst) -> void {
    dst.state = src.state;
}

auto HEX::TurnHandler::move(HexNode &src, HexNode &dst) -> void {
    dst.state = src.state;
    src.state = State::Empty;
}

auto HEX::TurnHandler::turnToString(const HEX::Turn turn1) -> std::string {
    std::string turn;
    switch (turn1) {
        case HEX::Turn::Null:
            turn = "NULL";
            break;
        case HEX::Turn::Player1:
            turn = "Player1";
            break;
        case HEX::Turn::Player2:
            turn = "Player2";
            break;
        case HEX::Turn::Player1Won:
            turn = "Player1 Won";
            break;
        case HEX::Turn::Player2Won:
            turn = "Player2 Won";
            break;
        case HEX::Turn::Draw:
            turn = "Draw";
            break;
    }
    return turn;
}

auto HEX::TurnHandler::evalMove(const HexNode &node, const State srcState) -> int {
    int res = 0;
    for (const auto &i: node.neighbors) {
        if (srcState != i->state && i->state != State::Empty && i->state != State::Inactive) {
            res++;
        }
    }
    return res;
}

//1 - clone
//0 - move
//TODO inefficient af
auto HEX::TurnHandler::possibleMoves(HexNode &node) {
    //Stackoverflow
    auto cmp = [](const std::pair<HexNode *, int> a, const std::pair<HexNode *, int> b) {
        return a.first < b.first;
    };
    std::set<std::pair<HexNode *, int>, decltype(cmp)> resSet;
    //end
    for (auto &i: node.neighbors) {
        if (i->state == State::Empty) {
            resSet.insert({i, 1});
        }
    }
    for (const auto &i: node.neighbors) {
        for (auto &j: i->neighbors) {
            if (j->state == State::Empty) {
                resSet.insert({j, 0});
            }
        }
    }
    return resSet;
}

std::pair<HEX::HexNode *, HEX::HexNode *> HEX::TurnHandler::findBestMove(GameState &gameState, Turn turn) {
    //always assume computer is Player2
    State state;
    if (turn==Turn::Player1)
        state = State::Player1;
    else
        state = State::Player2;
    std::pair<HEX::HexNode *, HEX::HexNode *> bestMove=std::make_pair(nullptr, nullptr);
    auto bestMoveEval = -1;
    for (auto &node: gameState.nodes) {
        if (node.state == state)
            for (auto [dest, isClone]: possibleMoves(node)) {
                auto currMoveEval = evalMove(*dest, state) + isClone;
                if (currMoveEval > bestMoveEval) {
                    bestMoveEval = currMoveEval;
                    bestMove = std::make_pair(&node, dest);
                }
            }
    }
    return bestMove;
}


std::optional<HEX::notification> HEX::TurnHandler::makeMove(GameState &gameState, HexNode &src, HexNode &dst,
                                                            const bool AI) {
    if (getTurn(gameState) == Turn::Player1Won || getTurn(gameState) == Turn::Player2Won || getTurn(gameState)
        == Turn::Draw)
        return notification(notificationTypes::Info, "Game Over");
    if (!AI && gameState.AI && getTurn(gameState) == Turn::Player2) {
        return notification(notificationTypes::Error,
                            "Player2 is AI");
    }
    if (src.equal(&dst))
        return notification(notificationTypes::Error,
                            "starting field and destination can not be the same");
    if (src.state == State::Empty || src.state == State::Inactive)
        return notification(
            notificationTypes::Error, "starting field is either empty or inactive");
    if (getTurn(gameState) != HEX::Maps::stateToTurn(src.state))
        return notification(
            notificationTypes::Error, "not your hex");
    if (dst.state != State::Empty)
        return notification(notificationTypes::Error,
                            "destination field isn't empty or is inactive");
    const int distance = gameState.dist(src, dst);
    if (distance > 2) return notification(notificationTypes::Error, "fields are too far apart");
    if (distance == 1) clone(src, dst);
    if (distance == 2) move(src, dst);
    convertNeighbors(dst);
    gameState.changeTurn();
    gameState.moves++;
    return std::nullopt;
}
