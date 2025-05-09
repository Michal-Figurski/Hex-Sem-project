//
// Created by Michał Figurski on 4/16/2025.
//

#ifndef GAMELIST_H
#define GAMELIST_H
#include <string>
#include "GameState.h"
#include "FileHandler.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <ctime>
#include <algorithm>

class Game {
public:
    //time_t date;
    std::string name;
    std::pair<int, int> result;
    std::filesystem::path path;
};

class GameView {
public:
    sf::RectangleShape rect = sf::RectangleShape({1920.0 / 2, 200});
    sf::Text *name;
    sf::Text *player1Res;
    sf::Text *player2Res;
    //sf::Text *date;

    GameView(std::string name, const std::pair<int, int> result, time_t date, const sf::Font &font,
             const float screenWidth) {
        rect.setSize(sf::Vector2f(screenWidth / 2, 200));
        this->name = new sf::Text(font, name, 30);
        this->name->setFillColor(sf::Color::Red);
        this->player1Res = new sf::Text(font, "Player1: " + std::to_string(result.first), 20);
        this->player1Res->setFillColor(sf::Color::Red);
        this->player2Res = new sf::Text(font, "Player2: " + std::to_string(result.second), 20);
        this->player2Res->setFillColor(sf::Color::Red);
        this->rect.setFillColor(sf::Color::Black);
    }

    auto move(const sf::Vector2f vector) -> void;

    auto draw(sf::RenderWindow &window) const -> void;
};

class GameList {
public:
    // std::vector<Game> gameList = std::vector<Game>();
    std::vector<std::filesystem::path> gameFiles = std::vector<std::filesystem::path>();
    std::vector<GameView> gameViews = std::vector<GameView>();

    auto update(FileHandler &fileHandler, sf::Font &font, float screenWidth) -> void;

    auto resetPositions(float screenWidth) -> void;
};


#endif //GAMELIST_H
