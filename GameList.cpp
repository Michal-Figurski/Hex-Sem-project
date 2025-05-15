//
// Created by Michał Figurski on 4/16/2025.
//

#include "GameList.h"

auto GameView::move(const sf::Vector2f vector) -> void {
    rect.move(vector);
    name.move(vector);
    player1Res.move(vector);
    player2Res.move(vector);
}

auto GameView::draw(sf::RenderWindow &window) const -> void {
    window.draw(rect);
    window.draw(name);
    window.draw(player1Res);
    window.draw(player2Res);
}

auto GameList::update(FileHandler &fileHandler, sf::Font &font, float screenWidth) -> void {
    // gameList.clear();
    gameViews.clear();
    gameFiles.clear();
    gameFiles = fileHandler.discoverSavedGames();
    for (auto &game: gameFiles) {
        auto [states, _] = FileHandler::loadGame(game, 61);
        auto stats = FileHandler::statsFromPreload(states);
        // gameList.emplace_back( game.generic_string(),stats, game);
        gameViews.emplace_back(game.filename().string(), stats,
                               std::filesystem::last_write_time(game).time_since_epoch().count(), font, screenWidth);
    }
}

auto GameList::resetPositions(const float screenWidth) -> void {
    int index = 0;
    for (auto &i: gameViews) {
        i.rect.setPosition({screenWidth / 4, static_cast<float>(index * 220.0)});
        i.name.setPosition({screenWidth / 2 - 100, static_cast<float>(index * 220.0 + 5)});
        i.player1Res.setPosition({screenWidth / 2 - 100, static_cast<float>(index * 220.0 + 100)});
        i.player2Res.setPosition({screenWidth / 2 + 100, static_cast<float>(index * 220.0 + 100)});
        index++;
    }
}
