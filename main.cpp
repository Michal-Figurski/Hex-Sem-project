#include <SFML/Window.hpp>
#include <HexGraph.h>
#include "SFML/Graphics/RenderWindow.hpp"


int main()
{
    sf::RenderWindow window(sf::VideoMode({1920, 1080}), "Hexx");

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::Black);
        window.display();
        
    }
}
