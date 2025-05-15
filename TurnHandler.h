//
// Created by Michał Figurski on 4/13/2025.
//

#ifndef TURNHANDLER_H
#define TURNHANDLER_H
#include <GameState.h>
#include <notification.h>
#include <optional>
#include <set>

namespace HEX {
    class TurnHandler {
    public:
        static auto getTurn(const GameState &gameState) -> Turn;

        static auto convertNeighbors(const HexNode &node) -> void;

        static auto clone(const HexNode &src, HexNode &dst) -> void;

        static auto move(HexNode &src, HexNode &dst) -> void;

        static auto makeMove(GameState &gameState, HexNode &src, HexNode &dst,
                             bool AI = false) -> std::optional<notification>;

        static auto turnToString(HEX::Turn turn1) -> std::string;

        static auto evalMove(const HexNode &node, State srcState) -> int;

        static auto possibleMoves(HexNode &node); //set pair[HexNode dest, int move or clone]
        static auto findBestMove(GameState &gameState, Turn turn) -> std::pair<HexNode *, HexNode *>;
    };
}
#endif //TURNHANDLER_H
