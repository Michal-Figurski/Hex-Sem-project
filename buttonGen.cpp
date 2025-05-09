//
// Created by Michał Figurski on 5/6/2025.
//

#include "buttonGen.h"

#include <fileHandler.h>
#include <GameState.h>

auto Button::handleEvent(const sf::Event &event, const sf::Window &window) -> void {
    assert(event.is<sf::Event::MouseButtonPressed>());
    auto pos = sf::Mouse::getPosition(window);
    if (this->rect.getGlobalBounds().contains({
        static_cast<float>(pos.x), static_cast<float>(pos.y)
    }))
        action();
}

auto Button::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {
    target.draw(this->rect);
    target.draw(this->text);
}

auto Button::setString(const std::string &text) -> void {
    this->text.setString(text);
    auto pos = this->rect.getPosition();
    auto size = this->rect.getSize();
    this->text.setPosition(pos + sf::Vector2<float>(size.x / 2 - this->text.getLocalBounds().size.x / 2.0f,
                                                    size.y / 2 - this->text.getLocalBounds().size.y / 2.0f));
}

auto ButtonGen::menuButtons(const sf::Font &font, HEX::GameState &gameState,
                            sf::RenderWindow &window) -> std::vector<Button> {
    auto menuButtons = std::vector<Button>();
    menuButtons.push_back(Button("NEW GAME (1vs1)", {(screenWidth / 2) - 200, 200.0}, {400.0, 100.0}, sf::Color::Black,
                                 sf::Color::Red, font));
    menuButtons[0].action = [&gameState] {
        gameState.newGame(screenWidth, screenHeight);
        drawBoard = true;
        drawMenu = false;
        gameState.AI = false;
        shouldUpdateInGameAIButton = true;
    };

    menuButtons.push_back(Button("NEW GAME (1vsAI)", {(screenWidth / 2) - 200, 350.0}, {400.0, 100.0}, sf::Color::Black,
                                 sf::Color::Red, font));

    menuButtons[1].action = [&gameState] {
        gameState.newGame(screenWidth, screenHeight);
        drawBoard = true;
        drawMenu = false;
        gameState.AI = true;
        shouldUpdateInGameAIButton = true;
    };
    menuButtons.push_back(Button("HEXXAGON", {(screenWidth / 2) - 200, 0.0}, {400.0, 100.0}, sf::Color::Blue,
                                 sf::Color::Black, font));
    menuButtons.push_back(Button("LOAD", {(screenWidth / 2) - 200, 500.0}, {400.0, 100.0}, sf::Color::Black,
                                 sf::Color::Red,
                                 font));
    menuButtons[3].action = [] {
        drawLoadMenu = true;
        drawBoard = false;
        drawMenu = false;
        drawSaveMenu = false;
        shouldUpdateGameList = true;
    };
    menuButtons.push_back(Button("SAVE", {(screenWidth / 2) - 200, 650.0}, {400.0, 100.0}, sf::Color::Black,
                                 sf::Color::Red,
                                 font));
    menuButtons[4].action = [&gameState] {
        if (!gameState.isGameInProgress)
            return;
        drawBoard = false;
        drawMenu = false;
        drawLoadMenu = false;
        drawSaveMenu = true;
    };
    menuButtons.push_back(Button("BEST GAMES", {(screenWidth / 2) - 200, 800.0}, {400.0, 100.0}, sf::Color::Black,
                                 sf::Color::Red, font));
    menuButtons.push_back(Button("BACK", {screenWidth - 250, 200.0}, {250.0, 100.0}, sf::Color::Black, sf::Color::Red,
                                 font));
    menuButtons[6].action = [&gameState] {
        if (!gameState.isGameInProgress)
            return;
        drawBoard = true;
        drawMenu = false;
        drawLoadMenu = false;
    };
    menuButtons.push_back(Button("QUIT", {screenWidth - 250, 700.0}, {250.0, 100.0}, sf::Color::Black, sf::Color::Red,
                                 font));
    menuButtons[7].action = [&window] {
        window.close();
    };
    return menuButtons;
}

auto ButtonGen::inGameButtons(const sf::Font &font, HEX::GameState &gameState) -> std::vector<Button> {
    auto inGameButtons = std::vector<Button>();
    inGameButtons.push_back(Button("SAVE", {screenWidth - 400, screenHeight / 3}, {400.0, 100.0}, sf::Color::Black,
                                   sf::Color::Red, font));
    inGameButtons[0].action = [] {
        drawBoard = false;
        drawMenu = false;
        drawLoadMenu = false;
        drawSaveMenu = true;
        userInput = "";
    };
    inGameButtons.push_back(Button("LOAD", {screenWidth - 400, screenHeight / 3 + 200}, {400.0, 100.0},
                                   sf::Color::Black, sf::Color::Red, font));
    inGameButtons[1].action = [] {
        drawLoadMenu = true;
        drawBoard = false;
        drawMenu = false;
        drawSaveMenu = false;
        shouldUpdateGameList = true;
    };
    inGameButtons.push_back(Button("MENU", {screenWidth - 400, screenHeight / 3 - 200}, {400.0, 100.0},
                                   sf::Color::Black, sf::Color::Red, font));
    inGameButtons[2].action = [] {
        drawBoard = false;
        drawMenu = true;
        drawResult = false;
        drawLoadMenu = false;
        drawSaveMenu = false;
    };
    //Turn Indicator
    inGameButtons.push_back(Button("", {screenWidth - 400, 0.0}, {400, 100}, sf::Color::White, sf::Color::Red, font));
    inGameButtons.push_back(Button("TURN ON AI", {screenWidth - 400, screenHeight - 100}, {400, 100}, sf::Color::White,
                                   sf::Color::Red, font));
    inGameButtons[4].action = [&gameState] {
        gameState.AI = !gameState.AI;
        shouldUpdateInGameAIButton = true;
        lastPlayerMove = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                .count();
    };
    return inGameButtons;
}

auto ButtonGen::saveButtons(const sf::Font &font, std::vector<std::pair<HEX::notification, sf::Text> > &notifications,
                            const HEX::GameState &gameState) -> std::vector<Button> {
    auto saveButtons = std::vector<Button>();
    saveButtons.push_back(Button("Filename: ", sf::Vector2<float>(screenWidth / 2 - 250, 250), {500, 50},
                                 sf::Color::Black, sf::Color::White, font));

    saveButtons.push_back(Button("SAVE", {screenWidth / 2 - 300, 800}, {200.0, 70.0}, sf::Color::Black, sf::Color::Red,
                                 font));
    saveButtons[1].action = [&gameState,  &notifications, &font] {
        if (!gameState.isGameInProgress) {
            notifications.push_back(HEX::notification::notificationToStringPair({HEX::notificationTypes::Error, "No game in progress to save"}, font));
            return;
        }
        if (FileHandler::checkIfValidFilename(userInput)) {
            shouldSaveGame = true;
            drawSaveMenu = false;
            drawBoard = true;
        } else {
            notifications.push_back(HEX::notification::notificationToStringPair(
                HEX::notification(HEX::notificationTypes::Warning,
                                  "Filename can not be empty"), font));
        }
    };
    saveButtons.push_back(Button("BACK", {screenWidth / 2 + 100, 800}, {200.0, 70.0}, sf::Color::Black, sf::Color::Red,
                                 font));
    saveButtons[2].action = [&gameState] {
        if (gameState.isGameInProgress) {
            drawSaveMenu = false;
            drawBoard = true;
            drawMenu = false;
            drawLoadMenu = false;
            drawSaveMenu = false;
        }
    };

    return saveButtons;
}

auto ButtonGen::resultButtons(const sf::Font &font, HEX::GameState &gameState) -> std::vector<Button> {
    auto resultButtons = std::vector<Button>();
    //GameOver result
    resultButtons.push_back(Button("", {screenWidth / 2 - 200, 500}, {400.0, 70.0}, sf::Color::White, sf::Color::Black,
                                   font));

    resultButtons.push_back(Button("SAVE", {screenWidth / 2 - 450, 700}, {200.0, 70.0}, sf::Color::Red,
                                   sf::Color::Black,
                                   font));
    resultButtons[1].action = [&gameState] {
        drawBoard = false;
        drawMenu = false;
        drawLoadMenu = false;
        drawSaveMenu = true;
        drawResult = false;
    };
    resultButtons.push_back(Button("BACK TO BOARD", {screenWidth / 2 - 200, 700}, {400.0, 70.0}, sf::Color::Red,
                                   sf::Color::Black,
                                   font));
    resultButtons[2].action = [&gameState] {
        if (!gameState.isGameInProgress)
            return;
        drawBoard = true;
        drawMenu = false;
        drawLoadMenu = false;
        drawResult = false;
    };
    resultButtons.push_back(Button("MENU", {screenWidth / 2 + 250, 700}, {200.0, 70.0}, sf::Color::Red,
                                   sf::Color::Black,
                                   font));
    resultButtons[3].action = [&gameState] {
        drawResult = false;
        drawMenu = true;
    };
    return resultButtons;
}
