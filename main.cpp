/*TODO create ?interface? plus/or button class to make rendering and managing buttons easier and dynamic justification *medium* *optional*
 *TODO result screen *medium*
 */
#include "fileHandler.h"
#include <SFML/Window.hpp>
#include "HexNode.h"
#include "GameState.h"
#include <iostream>
#include <map>

#include "TurnHandler.h"
#include "notification.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "GameList.h"
#include "main.h"
#include <buttonGen.h>

void printVector(const std::vector<HEX::notification> &vec) {
    for (auto &i: vec) {
        std::cout << std::string(i.message.getString()) << " " << i.timestamp << "\n";
    }
    std::cout << std::endl;
}


int main() {
    auto window = sf::RenderWindow(
        sf::VideoMode({static_cast<unsigned>(screenWidth), static_cast<unsigned>(screenHeight)}),
        "Hexxagon!!!", sf::Style::None, sf::State::Windowed, settings);
    auto gameState = HEX::GameState(false, screenWidth, screenHeight);
    auto fileHandler = FileHandler();
    auto gameList = GameList();
    auto notifications = std::vector<HEX::notification>();

    //result
    auto resultButtons = ButtonGen::resultButtons(font, gameState);
    auto resultBackground = sf::RectangleShape({screenWidth / 2, screenHeight / 2});
    resultBackground.setFillColor(sf::Color::Black);
    resultBackground.setPosition(sf::Vector2f(screenWidth / 2 - screenWidth / 4, screenHeight / 4));
    resultBackground.setOutlineColor(sf::Color::White);

    //load
    auto loadBackground = sf::RectangleShape({screenWidth / 2, screenHeight});
    loadBackground.setFillColor(sf::Color::White);
    loadBackground.setPosition(sf::Vector2<float>(screenWidth / 4, 0));
    loadBackground.setOutlineColor(sf::Color::Black);
    auto loadBackButton = (Button("BACK", {screenWidth - 200, 200}, {200.0, 100.0}, sf::Color::Black, sf::Color::Red,
                                  font));
    //save
    auto saveBackground = sf::RectangleShape({500, 200});
    saveBackground.setFillColor(sf::Color::Black);
    saveBackground.setPosition(sf::Vector2<float>(screenWidth / 2 - 250, 300));
    sf::Text userInputDisplay(font);
    userInputDisplay.setPosition(sf::Vector2<float>(screenWidth / 2 - 200, 400));
    userInputDisplay.setCharacterSize(24);
    auto saveButtons = ButtonGen::saveButtons(font, notifications, gameState);
    auto lastCursorToggle = sf::Clock();
    lastCursorToggle.start();
    auto cursor = sf::Text(font);
    cursor.setCharacterSize(24);
    cursor.setString("|");
    //board
    auto inGameButtons = ButtonGen::inGameButtons(font, gameState);
    //menu
    auto menuButtons = ButtonGen::menuButtons(font, gameState, window);
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    auto moveToMake = std::pair<HEX::HexNode *, HEX::HexNode *>();
    while (window.isOpen()) {
        while (std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (event->is<sf::Event::MouseButtonReleased>())
                ignoreMouse = false;
            if (drawLoadMenu)
                if (const auto *ev = event->getIf<sf::Event::MouseWheelScrolled>()) {
                    if (ev->delta < 0) {
                        loadListMoveDown = true;
                    }
                    if (ev->delta > 0) {
                        loadListMoveUp = true;
                    }
                }
            if (drawSaveMenu) {
                if (const auto *ev = event->getIf<sf::Event::KeyPressed>()) {
                    if (!((ev->code >= sf::Keyboard::Key::A && ev->code <= sf::Keyboard::Key::Z) || ev->code == sf::Keyboard::Key::Backspace)) {
                        notifications.emplace_back(HEX::notificationTypes::Warning,
                                                                         "Filename can only contain latin letters");
                    }
                    if (ev->code >= sf::Keyboard::Key::A && ev->code <= sf::Keyboard::Key::Z) {
                        if (userInput.size() < 26) {
                            userInput += static_cast<char>(static_cast<char>(ev->code) + 97);
                        } else {
                            notifications.emplace_back(HEX::notificationTypes::Info,
                                                                         "Filename can not be longer than 26 characters");
                        }
                    }
                    if (ev->code == sf::Keyboard::Key::Backspace) {
                        if (!userInput.empty()) {
                            userInput.pop_back();
                        }
                    }
                }
            }

            if (!ignoreMouse && event->is<sf::Event::MouseButtonPressed>()) {
                ignoreMouse = true;
                auto pos = sf::Mouse::getPosition(window);
                if (drawLoadMenu) {
                    int index = 0;
                    for (auto &i: gameList.gameViews) {
                        if (i.rect.getGlobalBounds().contains({
                            static_cast<float>(pos.x), static_cast<float>(pos.y)
                        })) {
                            gameToLoad = index;
                            shouldLoadGame = true;
                            drawLoadMenu = false;
                            drawBoard = true;
                            drawMenu = false;
                        }
                        index++;
                    }
                    if (loadBackButton.rect.getGlobalBounds().contains({
                        static_cast<float>(pos.x), static_cast<float>(pos.y)
                    })) {
                        if (gameState.isGameInProgress) {
                            drawLoadMenu = false;
                            drawBoard = true;
                            drawMenu = false;
                            drawSaveMenu = false;
                        } else {
                            drawMenu = true;
                            drawLoadMenu = false;
                            drawSaveMenu = false;
                            drawBoard = false;
                        }
                    }
                }
                if (drawBoard) {
                    HEX::HexNode *node = nullptr;
                    for (auto &i: gameState.nodes)
                        if (i.hex.getGlobalBounds().contains({static_cast<float>(pos.x), static_cast<float>(pos.y)}))
                            node = &i;
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
                    //buttons
                    for (auto &i: inGameButtons)
                        i.handleEvent(event.value(), window);
                }
                if (drawSaveMenu) {
                    for (auto &i: saveButtons) {
                        i.handleEvent(event.value(), window);
                    }
                }
                if (drawMenu) {
                    for (auto &i: menuButtons) {
                        i.handleEvent(event.value(), window);
                    }
                }
                if (drawResult) {
                    for (auto &i: resultButtons) {
                        i.handleEvent(event.value(), window);
                    }
                }
            }
        }
        if (lastCursorToggle.getElapsedTime() > sf::milliseconds(500)) {
            showCursor = !showCursor;
            lastCursorToggle.restart();
        }
        if (!gameState.isGameOverVar && gameState.isGameOver()) {
            //Player1 result : int, Player2 result : int
            gameState.evaluateResult();
            switch (gameState.turn1()) {
                case 4:
                    resultButtons[0].rect.setFillColor(sf::Color::Blue);
                    inGameButtons[3].rect.setFillColor(sf::Color::Blue);
                    break;
                case 5:
                    resultButtons[0].rect.setFillColor(sf::Color::Green);
                    inGameButtons[3].rect.setFillColor(sf::Color::Green);
                    break;
                case 6:
                    resultButtons[0].rect.setFillColor(sf::Color::White);
                    inGameButtons[3].rect.setFillColor(sf::Color::White);
                    break;
                default: ;
            }
            resultButtons[0].setString(HEX::TurnHandler::turnToString(gameState.turn1()));
            gameState.isGameOverVar = true;
            drawResult = true;
            drawBoard = false;
        }
        if (shouldMakeMove) {
            auto error = HEX::TurnHandler::makeMove(gameState, *moveToMake.first, *moveToMake.second);
            if (error.has_value()) {
                //std::cerr << std::string(error.value().message.getString());
                notifications.push_back(*error);
            } else {
                lastPlayerMove = std::chrono::duration_cast<std::chrono::seconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                        .count();
            }
            shouldMakeMove = false;
        }
        if (gameState.AI && gameState.turn1() == HEX::Turn::Player2 && difftime(
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                .count(), lastPlayerMove) > 2) {
            auto [src, dest] = HEX::TurnHandler::findBestMove(gameState, gameState.turn1());
            //std::cout << stateToString(src->state) << " " << stateToString(dest->state) << '\n';
            auto error = HEX::TurnHandler::makeMove(gameState, *(src), *(dest), true);
            if (error.has_value()) {
                //std::cerr << error.value().message;
                notifications.push_back(*error);
            }
        }

        if (shouldSaveGame) {
            fileHandler.saveGame(gameState, userInput);
            notifications.emplace_back(HEX::notificationTypes::Info, "Game saved");
            shouldSaveGame = false;
        }
        if (shouldLoadGame) {
            shouldLoadGame = false;
            if (gameToLoad > -1) {
                bool failed = false;
                std::vector<HEX::State> states;
                HEX::Turn turn;
                try {
                    auto temp = FileHandler::loadGame(std::filesystem::path(gameList.gameFiles[gameToLoad]), 61);
                    states = temp.first;
                    turn = temp.second;
                } catch (std::ios_base::failure &e) {
                    failed = true;
                    notifications.emplace_back(HEX::notificationTypes::Error, e.what());
                }
                if (!failed) {
                    gameState.newGame(screenWidth, screenHeight);
                    gameState.importPositions(states);
                    gameState.set_turn(turn);
                }
            }
        }
        if (shouldUpdateGameList) {
            gameList.update(fileHandler, font, screenWidth);
            shouldUpdateGameList = false;
            gameList.resetPositions(screenWidth);
            // std::cout << gameList.gameViews.size() << "\n";
        }
        //clear outdated notifications (older than 6s)
        auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                .count();
        if (difftime(now, lastNotificationCheck) > 1) {
            HEX::notification::clearNotifications(now, notifications);
            lastNotificationCheck = now;
        }
        for (int i = 0; i < gameState.nodes.size(); i += 1) {
            gameState.nodes[i].hex.setFillColor(colorTheme[gameState.nodes[i].state]);
        }
        if (shouldUpdateInGameAIButton) {
            shouldUpdateInGameAIButton = false;
            if (gameState.AI) {
                inGameButtons[4].setString("TURN OFF AI");
            } else {
                inGameButtons[4].setString("TURN ON AI");
            }
        }
        window.clear(sf::Color::Red);
        if (true) {
            //notifications
            int notificationIndex = 0;
            for (auto &notification: notifications) {
                notification.message.setPosition({0, static_cast<float>(notificationIndex++) * 40});
                notification.draw(window);
            }
        }
        if (drawMenu) {
            for (auto &i: menuButtons) {
                if (i.text.getString() == "BACK" || i.text.getString() == "SAVE") {
                    if (gameState.isGameInProgress) {
                        i.draw(window);
                    }
                } else {
                    i.draw(window);
                }
            }
        }
        if (drawBoard) {
            for (auto &i: gameState.nodes)
                window.draw(i.hex);
            //turn indicator
            inGameButtons[3].setString(HEX::TurnHandler::turnToString(gameState.turn1()));
            if (inGameButtons[3].text.getString() == "Player1")
                inGameButtons[3].rect.setFillColor(sf::Color::Blue);
            else if (inGameButtons[3].text.getString() == "Player2")
                inGameButtons[3].rect.setFillColor(sf::Color::Green);
            for (auto &i: inGameButtons) {
                i.draw(window);
            }
        }
        if (drawResult) {
            window.draw(resultBackground);
            for (auto &i: resultButtons) {
                i.draw(window);
            }
        }
        if (drawSaveMenu) {
            userInputDisplay.setString(userInput);
            window.draw(saveBackground);
            window.draw(userInputDisplay);
            if (showCursor) {
                auto pos = userInputDisplay.getPosition();
                cursor.setPosition({pos.x + userInputDisplay.getGlobalBounds().size.x, pos.y});
                window.draw(cursor);
            }
            for (auto &i: saveButtons) {
                i.draw(window);
            }
        }
        if (loadListMoveDown) {
            for (auto &i: gameList.gameViews) {
                i.move({0, -10});
            }
            loadListMoveDown = false;
        }
        if (loadListMoveUp) {
            for (auto &i: gameList.gameViews) {
                i.move({0, 10});
            }
            loadListMoveUp = false;
        }
        if (drawLoadMenu) {
            window.draw(loadBackground);
            for (auto &i: gameList.gameViews) {
                i.draw(window);
            }
            loadBackButton.draw(window);
        }
        if (drawBestGames) {
        }
        window.display();
    }
}
