//
// Created by Michał Figurski on 4/15/2025.
//

#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include "GameState.h"
#include "notification.h"
#include <filesystem>
#include <fstream>
#include <iostream>


class FileHandler {
    std::filesystem::path const savePath = std::filesystem::path("./saves");

public:
    FileHandler();

    auto saveGame(const HEX::GameState &gameState, const std::string &name) const -> void;

    static auto statsFromPreload(const std::vector<HEX::State> &nodes) -> std::pair<int, int>;

    static auto isSaveFileName(const std::filesystem::path &filename);

    static auto checkIfValidFilename(const std::string &filename) -> bool;

    [[nodiscard]] auto checkIfSaveFileExists(const std::string &filename) const -> bool;

    auto discoverSavedGames() -> std::vector<std::filesystem::path>;

    static auto loadGame(const std::filesystem::path &filename,
                         int expectedSize) -> std::pair<std::vector<HEX::State>, HEX::Turn>;

    static auto getSavedGames();
};


#endif //FILEHANDLER_H
