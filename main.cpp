#include "fileHandler.h"
#include <SFML/Window.hpp>
#include "HexNode.h"
#include "GameState.h"
#include <iostream>
#include <map>
#include <vector>
#include <chrono>

#include "TurnHandler.h"
#include "notification.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "GameList.h"
#include "main.h"
#include <buttonGen.h>

void printVector(const std::vector<HEX::notification> &vec) {
    for (const auto &i: vec) {
        std::cout << i.message.getString().toAnsiString() << " " << i.timestamp << "\n";
    }
    std::cout << std::endl;
}

void setUpBackground(sf::RectangleShape &background, sf::Color fill, sf::Vector2f pos, sf::Color outline) {
    background.setFillColor(fill);
    background.setPosition(pos);
    background.setOutlineColor(outline);
}

struct NotificationManager {
    std::vector<HEX::notification> list;
    int64_t lastCheck = 0;

    void update() {
        auto now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        if (difftime(now, lastCheck) > 1) {
            HEX::notification::clearNotifications(now, list);
            lastCheck = now;
        }
    }

    void draw(sf::RenderWindow &window) {
        int index = 0;
        for (auto &notification: list) {
            notification.message.setPosition({0, static_cast<float>(index++) * 40});
            notification.draw(window);
        }
    }
};

struct SaveMenuComponents {
    sf::RectangleShape background;
    sf::Text userInputDisplay;
    sf::Text cursor;
    sf::Clock cursorClock;
    bool showCursor = true;
    std::vector<Button> buttons;

    // Pass 'font' by reference to the constructor initialization list
    SaveMenuComponents(sf::Font &font, std::vector<HEX::notification> &notifications, HEX::GameState &gameState)
        : userInputDisplay(font),
          cursor(font)
    {
        background.setSize({500, 200});
        background.setFillColor(sf::Color::Black);
        background.setPosition({screenWidth / 2.0f - 250.0f, 300.0f});

        userInputDisplay.setFont(font);
        userInputDisplay.setPosition({screenWidth / 2.0f - 200.0f, 400.0f});
        userInputDisplay.setCharacterSize(24);

        cursor.setFont(font);
        cursor.setCharacterSize(24);
        cursor.setString("|");
        cursorClock.start();

        buttons = ButtonGen::saveButtons(font, notifications, gameState);
    }

    void handleTextEvents(const sf::Event *event, NotificationManager &nm) {
        if (const auto *ev = event->getIf<sf::Event::KeyPressed>()) {
            if (!((ev->code >= sf::Keyboard::Key::A && ev->code <= sf::Keyboard::Key::Z) || ev->code ==
                  sf::Keyboard::Key::Backspace)) {
                nm.list.emplace_back(HEX::notificationTypes::Warning, "Filename can only contain latin letters");
            }
            if (ev->code >= sf::Keyboard::Key::A && ev->code <= sf::Keyboard::Key::Z) {
                if (userInput.size() < 26) {
                    userInput += static_cast<char>(static_cast<char>(ev->code) + 97);
                } else {
                    nm.list.emplace_back(HEX::notificationTypes::Info, "Filename can not be longer than 26 characters");
                }
            }
            if (ev->code == sf::Keyboard::Key::Backspace && !userInput.empty()) {
                userInput.pop_back();
            }
        }
    }

    void updateCursor() {
        if (cursorClock.getElapsedTime() > sf::milliseconds(500)) {
            showCursor = !showCursor;
            cursorClock.restart();
        }
    }

    void draw(sf::RenderWindow &window) {
        userInputDisplay.setString(userInput);
        window.draw(background);
        window.draw(userInputDisplay);
        if (showCursor) {
            auto pos = userInputDisplay.getPosition();
            cursor.setPosition({pos.x + userInputDisplay.getGlobalBounds().size.x, pos.y});
            window.draw(cursor);
        }
        for (auto &btn: buttons) btn.draw(window);
    }
};


void handleInput(sf::RenderWindow &window, HEX::GameState &gameState, GameList &gameList,
                 SaveMenuComponents &saveMenu, NotificationManager &nm, Button &loadBackButton,
                 std::vector<Button> &inGameButtons, std::vector<Button> &saveButtons,
                 std::vector<Button> &menuButtons, std::vector<Button> &resultButtons,
                 std::pair<HEX::HexNode *, HEX::HexNode *> &moveToMake) {
    while (std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        if (event->is<sf::Event::MouseButtonReleased>()) {
            ignoreMouse = false;
        }

        if (drawLoadMenu) {
            if (const auto *ev = event->getIf<sf::Event::MouseWheelScrolled>()) {
                if (ev->delta < 0) loadListMoveDown = true;
                if (ev->delta > 0) loadListMoveUp = true;
            }
        }

        if (drawSaveMenu) {
            saveMenu.handleTextEvents(&(*event), nm);
        }

        if (!ignoreMouse && event->is<sf::Event::MouseButtonPressed>()) {
            ignoreMouse = true;
            auto pos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos{static_cast<float>(pos.x), static_cast<float>(pos.y)};

            if (drawLoadMenu) {
                int index = 0;
                for (auto &view: gameList.gameViews) {
                    if (view.rect.getGlobalBounds().contains(mousePos)) {
                        gameToLoad = index;
                        shouldLoadGame = true;
                        drawLoadMenu = false;
                        drawBoard = true;
                        drawMenu = false;
                    }
                    index++;
                }
                if (loadBackButton.rect.getGlobalBounds().contains(mousePos)) {
                    drawLoadMenu = false;
                    drawSaveMenu = false;
                    bool inProgress = gameState.isGameInProgress;
                    drawBoard = inProgress;
                    drawMenu = !inProgress;
                }
            } else if (drawBoard) {
                HEX::HexNode *node = nullptr;
                for (auto &i: gameState.nodes) {
                    if (i.hex.getGlobalBounds().contains(mousePos)) {
                        node = &i;
                    }
                }
                if (node != nullptr) {
                    if (lastSelected == nullptr) {
                        lastSelected = node;
                        gameState.setSelected(lastSelected->index);
                    } else {
                        moveToMake = std::make_pair(lastSelected, node);
                        shouldMakeMove = true;
                        gameState.clearSelected();
                        lastSelected = nullptr;
                    }
                }
                for (auto &btn: inGameButtons) btn.handleEvent(event.value(), window);
            } else if (drawSaveMenu) {
                for (auto &btn: saveButtons) btn.handleEvent(event.value(), window);
            } else if (drawMenu) {
                for (auto &btn: menuButtons) btn.handleEvent(event.value(), window);
            } else if (drawResult) {
                for (auto &btn: resultButtons) btn.handleEvent(event.value(), window);
            }
        }
    }
}

void updateGameLogic(HEX::GameState &gameState, FileHandler &fileHandler, GameList &gameList,
                     NotificationManager &nm, std::pair<HEX::HexNode *, HEX::HexNode *> &moveToMake,
                     std::vector<Button> &resultButtons, std::vector<Button> &inGameButtons) {
    if (!gameState.isGameOverVar && gameState.isGameOver()) {
        gameState.evaluateResult();
        sf::Color winnerColor = sf::Color::White;
        if (gameState.turn1() == HEX::Turn::Player1Won) winnerColor = sf::Color::Blue;
        else if (gameState.turn1() == HEX::Turn::Player2Won) winnerColor = sf::Color::Green;

        resultButtons[0].rect.setFillColor(winnerColor);
        inGameButtons[3].rect.setFillColor(winnerColor);
        resultButtons[0].setString(HEX::TurnHandler::turnToString(gameState.turn1()));

        gameState.isGameOverVar = true;
        drawResult = true;
        drawBoard = false;
    }

    if (shouldMakeMove) {
        auto error = HEX::TurnHandler::makeMove(gameState, *moveToMake.first, *moveToMake.second);
        if (error.has_value()) {
            nm.list.push_back(*error);
        } else {
            lastPlayerMove = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        }
        shouldMakeMove = false;
    }

    if (gameState.AI && gameState.turn1() == HEX::Turn::Player2) {
        auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        if (difftime(now, lastPlayerMove) > 2) {
            auto [src, dest] = HEX::TurnHandler::findBestMove(gameState, gameState.turn1());
            auto error = HEX::TurnHandler::makeMove(gameState, *(src), *(dest), true);
            if (error.has_value()) nm.list.push_back(*error);
        }
    }

    if (shouldSaveGame) {
        fileHandler.saveGame(gameState, userInput);
        nm.list.emplace_back(HEX::notificationTypes::Info, "Game saved");
        shouldSaveGame = false;
    }

    if (shouldLoadGame) {
        shouldLoadGame = false;
        if (gameToLoad > -1) {
            try {
                auto [states, turn] = FileHandler::loadGame(std::filesystem::path(gameList.gameFiles[gameToLoad]), 61);
                gameState.newGame(screenWidth, screenHeight);
                gameState.importPositions(states);
                gameState.set_turn(turn);
            } catch (std::ios_base::failure &e) {
                nm.list.emplace_back(HEX::notificationTypes::Error, e.what());
            }
        }
    }

    if (shouldUpdateGameList) {
        gameList.update(fileHandler, font, screenWidth);
        gameList.resetPositions(screenWidth);
        shouldUpdateGameList = false;
    }

    nm.update();

    for (auto &node: gameState.nodes) {
        node.hex.setFillColor(colorTheme[node.state]);
    }

    if (shouldUpdateInGameAIButton) {
        shouldUpdateInGameAIButton = false;
        inGameButtons[4].setString(gameState.AI ? "TURN OFF AI" : "TURN ON AI");
    }
}

void renderScene(sf::RenderWindow &window, HEX::GameState &gameState, GameList &gameList,
                 SaveMenuComponents &saveMenu, NotificationManager &nm, sf::RectangleShape &resultBackground,
                 sf::RectangleShape &loadBackground, Button &loadBackButton, std::vector<Button> &inGameButtons,
                 std::vector<Button> &resultButtons, std::vector<Button> &menuButtons) {
    window.clear(sf::Color::Red);

    nm.draw(window);

    if (drawMenu) {
        for (auto &btn: menuButtons) {
            if ((btn.text.getString() == "BACK" || btn.text.getString() == "SAVE") && !gameState.isGameInProgress) {
                continue;
            }
            btn.draw(window);
        }
    }

    if (drawBoard) {
        for (auto &node: gameState.nodes) window.draw(node.hex);

        inGameButtons[3].setString(HEX::TurnHandler::turnToString(gameState.turn1()));
        inGameButtons[3].rect.setFillColor(inGameButtons[3].text.getString() == "Player1"
                                               ? sf::Color::Blue
                                               : sf::Color::Green);

        for (auto &btn: inGameButtons) btn.draw(window);
    }

    if (drawResult) {
        window.draw(resultBackground);
        for (auto &btn: resultButtons) btn.draw(window);
    }

    if (drawSaveMenu) {
        saveMenu.draw(window);
    }

    if (loadListMoveDown) {
        for (auto &view: gameList.gameViews) view.move({0, -10});
        loadListMoveDown = false;
    }
    if (loadListMoveUp) {
        for (auto &view: gameList.gameViews) view.move({0, 10});
        loadListMoveUp = false;
    }

    if (drawLoadMenu) {
        window.draw(loadBackground);
        for (auto &view: gameList.gameViews) view.draw(window);
        loadBackButton.draw(window);
    }

    window.display();
}

int main() {
    auto window = sf::RenderWindow(
        sf::VideoMode({static_cast<unsigned>(screenWidth), static_cast<unsigned>(screenHeight)}),
        "Hexxagon!!!", sf::Style::None, sf::State::Windowed, settings);

    HEX::GameState gameState(false, screenWidth, screenHeight);
    FileHandler fileHandler;
    GameList gameList;
    NotificationManager nm;

    sf::RectangleShape resultBackground({screenWidth / 2.0f, screenHeight / 2.0f});
    setUpBackground(resultBackground, sf::Color::Black, {screenWidth / 2.0f - screenWidth / 4.0f, screenHeight / 4.0f},
                    sf::Color::White);

    sf::RectangleShape loadBackground({screenWidth / 2.0f, static_cast<float>(screenHeight)});
    setUpBackground(loadBackground, sf::Color::White, {screenWidth / 4.0f, 0}, sf::Color::Black);

    Button loadBackButton("BACK", {screenWidth - 200.0f, 200.0f}, {200.0f, 100.0f}, sf::Color::Black, sf::Color::Red,
                          font);
    SaveMenuComponents saveMenu(font, nm.list, gameState);

    // Button Generations
    auto resultButtons = ButtonGen::resultButtons(font, gameState);
    auto inGameButtons = ButtonGen::inGameButtons(font, gameState);
    auto menuButtons = ButtonGen::menuButtons(font, gameState, window);

    std::pair<HEX::HexNode *, HEX::HexNode *> moveToMake;

    // Clean, Self-Documenting Main Application Loop
    while (window.isOpen()) {
        handleInput(window, gameState, gameList, saveMenu, nm, loadBackButton,
                    inGameButtons, saveMenu.buttons, menuButtons, resultButtons, moveToMake);

        saveMenu.updateCursor();

        updateGameLogic(gameState, fileHandler, gameList, nm, moveToMake, resultButtons, inGameButtons);

        renderScene(window, gameState, gameList, saveMenu, nm, resultBackground,
                    loadBackground, loadBackButton, inGameButtons, resultButtons, menuButtons);
    }
    return 0;
}
