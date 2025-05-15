//
// Created by Michał Figurski on 5/6/2025.
//

#ifndef BUTTONGEN_H
#define BUTTONGEN_H


#include <functional>
#include <GameState.h>
#include <main.h>
#include "SFML/Window/Mouse.hpp"

#include <vector>
#include <SFML/Graphics.hpp>

class Button {
public:
    sf::RectangleShape rect;
    sf::Text text;
    std::function<void()> action;

    Button(const std::string &text, sf::Vector2<float> pos, sf::Vector2<float> size, sf::Color colorBut,
           sf::Color colorTxt,
           const sf::Font &font): text(font) {
        this->rect = sf::RectangleShape(size);
        this->rect.setFillColor(colorBut);
        this->text = sf::Text(font);
        this->text.setString(text);
        this->text.setCharacterSize(40);
        this->text.setPosition(pos + sf::Vector2<float>(size.x / 2 - this->text.getLocalBounds().size.x / 2.0f,
                                                        size.y / 2 - this->text.getLocalBounds().size.y / 2.0f));
        this->text.setFillColor(colorTxt);
        rect.setPosition(pos);
        action = []() {
        };
    }

    auto handleEvent(const sf::Event &event, const sf::Window &window) -> void;

    auto draw(sf::RenderTarget &target, sf::RenderStates states = sf::RenderStates::Default) const -> void;

    auto setString(const std::string &text) -> void;
};

class ButtonGen {
public:
    static auto menuButtons(const sf::Font &font, HEX::GameState &gameState,
                            sf::RenderWindow &window) -> std::vector<Button>;

    static auto inGameButtons(const sf::Font &font, HEX::GameState &gameState) -> std::vector<Button>;

    static auto saveButtons(const sf::Font &font, std::vector<HEX::notification> &notifications,
                            const HEX::GameState &gameState) -> std::vector<Button>;

    static auto resultButtons(const sf::Font &font, HEX::GameState &gameState) -> std::vector<Button>;

    static auto bestGamesButtons(const sf::Font &font);//TODO
};


#endif //BUTTONGEN_H
