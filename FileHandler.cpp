//
// Created by Michał Figurski on 4/15/2025.
//

#include "FileHandler.h"

FileHandler::FileHandler() {
    if (!std::filesystem::exists(savePath)) {
        std::filesystem::create_directory(savePath);
    }
}

auto FileHandler::saveGame(const HEX::GameState &gameState, const std::string &name) const -> void {
    std::ofstream ofs;
    auto const currentPath = std::filesystem::path(savePath.generic_string() + '/' + name + ".sav");
    ofs.open(currentPath, std::ofstream::out | std::ofstream::trunc);
    char turn;
    switch (gameState.turn1()) {
        case HEX::Turn::Null:
            turn = 0;
            break;
        case HEX::Turn::Player1:
            turn = 2;
            break;
        case HEX::Turn::Player2:
            turn = 3;
            break;
        case HEX::Turn::Player1Won:
            turn = 4;
            break;
        case HEX::Turn::Player2Won:
            turn = 5;
            break;
        case HEX::Turn::Draw:
            turn = 6;
            break;
        default:
            throw std::invalid_argument("gamestate corrupted (turn)");
    }
    ofs << static_cast<char>(turn + '0');
    char state;
    for (auto &i: gameState.nodes) {
        switch (i.state) {
            case HEX::State::Inactive:
                state = 0;
                break;
            case HEX::State::Empty:
                state = 1;
                break;
            case HEX::State::Player1:
                state = 2;
                break;
            case HEX::State::Player2:
                state = 3;
                break;
            default:
                throw std::invalid_argument("gamestate corrupted (state)");
        }
        ofs << static_cast<char>(state + '0');
    }
    ofs.close();
}

auto FileHandler::statsFromPreload(const std::vector<HEX::State> &nodes) -> std::pair<int, int> {
    int player1 = 0, player2 = 0;
    for (auto &i: nodes) {
        if (i == HEX::State::Player1)
            player1++;
        if (i == HEX::State::Player2)
            player2++;
    }
    return std::make_pair(player1, player2);
}

auto FileHandler::isSaveFileName(const std::filesystem::path &filename) {
    return filename.extension() == ".sav";
}

auto FileHandler::checkIfValidFilename(const std::string &filename) -> bool {
    if (filename.empty())
        return false;
    for (auto &i: filename) {
        if (!std::isalpha(i) && !std::isdigit(i))
            return false;
    }
    return true;
}

auto FileHandler::checkIfSaveFileExists(const std::string &filename) const -> bool {
    auto path = savePath;
    path /= filename + ".sav";
    return std::filesystem::exists(path);
}

auto FileHandler::discoverSavedGames() -> std::vector<std::filesystem::path> {
    std::vector<std::filesystem::path> result;
    for (const auto &entry: std::filesystem::directory_iterator(savePath))
        if (isSaveFileName(entry.path()))
            result.push_back(entry.path());
    return result;
}

auto FileHandler::loadGame(const std::filesystem::path &filename,
                           const int expectedSize) -> std::pair<std::vector<HEX::State>, HEX::Turn> {
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::in);
    if (ifs.bad() || ifs.eof())
        throw std::ios_base::failure("Savefile not readable");
    std::vector<HEX::State> states;
    char temp;
    ifs >> temp;
    auto turn = static_cast<HEX::Turn>(temp - '0');
    for (auto i = 0; i < expectedSize; i++) {
        ifs >> temp;
        states.push_back(static_cast<HEX::State>(temp - '0'));
    }
    ifs.close();
    return std::make_pair(states, turn);
}

auto FileHandler::getSavedGames() {
    return;
}
