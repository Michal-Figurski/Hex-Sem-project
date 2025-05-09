//
// Created by Michał Figurski on 4/18/2025.
//

#ifndef MAIN_H
#define MAIN_H
#include <GameList.h>
inline bool ignoreMouse = false;
inline bool drawBoard = false;
inline bool drawMenu = true;
inline bool drawResult = false;
inline bool drawLoadMenu = false;
inline bool drawSaveMenu = false;
inline bool shouldUpdateGameList = true;
inline bool loadListMoveDown = false;
inline bool loadListMoveUp = false;
inline bool shouldSaveGame = false;
inline sf::Font font(".\\OpenSans-Medium.ttf");
inline auto colorTheme = HEX::Maps::color();
inline HEX::HexNode *lastSelected = nullptr;
inline time_t lastNotificationCheck = time(nullptr);
inline time_t lastPlayerMove = time(nullptr);
inline bool shouldMakeMove = false;
inline bool showCursor = false;
inline bool drawBestGames = false;
const float screenHeight = static_cast<float>(sf::VideoMode::getDesktopMode().size.y);
const float screenWidth = static_cast<float>(sf::VideoMode::getDesktopMode().size.x);
inline sf::ContextSettings settings = sf::ContextSettings({.antiAliasingLevel = 8});
inline std::string userInput;
inline bool shouldUpdateInGameAIButton = false;
inline bool shouldLoadGame = false;
inline int gameToLoad = -1;
#endif //MAIN_H
