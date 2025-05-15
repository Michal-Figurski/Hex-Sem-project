//
// Created by Michał Figurski on 4/15/2025.
//
//
// Created by Michał Figurski on 4/13/2025.
//


#include <string>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <utility>
#include "notification.h"
#include "main.h"

namespace HEX {
    notification::notification(const notificationTypes type, std::string message) : message(font), type(type) {
        this->message.setString(message);
        timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            (std::chrono::system_clock::now().time_since_epoch())).count();
    }

    auto notification::draw(sf::RenderWindow &window) const -> void {
        window.draw(this->message);
    }

    [[deprecated]]auto notification::notificationToStringPair(const HEX::notification &a,
                                                              const sf::Font &font) -> std::pair<HEX::notification
        ,
        sf::Text> {
        auto text = sf::Text(font);
        text.setString(a.message.getString());
        text.setCharacterSize(24);
        return std::make_pair(a, text);
    }

    auto notification::clearNotifications(const time_t now,
                                          std::vector<HEX::notification > &notifications) -> void {
        for (int i = 0; i < notifications.size(); i++) {
            if (difftime(now, (notifications[i].timestamp)) > 4) {
                notifications.erase(notifications.begin() + i);
                i--;
            }
        }
    }
}
