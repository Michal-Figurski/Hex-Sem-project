//
// Created by Michał Figurski on 4/13/2025.
//

#pragma once
#include <string>

namespace HEX {
    enum class notificationTypes {
        Warning,
        Info,
        Error
    };

    class notification {
    public:
        notificationTypes type;
        sf::Text message;
        std::time_t timestamp;

        notification(notificationTypes type, std::string message);

        auto draw(sf::RenderWindow &window) const -> void;
        static auto notificationToStringPair(const HEX::notification &a,
                                             const sf::Font &font) -> std::pair<HEX::notification, sf::Text>;

        static auto clearNotifications(
            time_t now, std::vector<HEX::notification> &notifications) -> void;
    };
}
