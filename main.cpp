#include <SFML/Window.hpp>
#include <HexGraph.h>
#include "SFML/Graphics/RenderWindow.hpp"


int main()
{
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Hexx");

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
