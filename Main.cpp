#include <SFML/Graphics.hpp>
#include <iostream>
#include "ComplexPlane.h"

int main() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int windowWidth = desktop.width / 2; 
    unsigned int windowHeight = desktop.height / 2;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Mandelbrot Set");

    ComplexPlane complexPlane(windowWidth, windowHeight);

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    complexPlane.zoomIn();
                }
                if (event.mouseButton.button == sf::Mouse::Right) {
                    complexPlane.zoomOut();
                }
            }

            if (event.type == sf::Event::MouseMoved) {
                complexPlane.setMouseLocation(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            }
        }

        complexPlane.updateRender();

        window.clear();
        window.draw(complexPlane);
        window.display();
    }

    return 0;
}
