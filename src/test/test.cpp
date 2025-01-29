#include "../Loader.h"

int main() {
    sf::RenderWindow window;
    window.create(sf::VideoMode::getDesktopMode(), "Test Animation", sf::Style::Titlebar | sf::Style::Close);

    sf::Font font;
    font.loadFromFile("Font\\NicoPaint-Monospaced.otf");

    Loader::getInstance(window);
    Loader::getInstance().setLoaderScreenImage("Texture\\background.png");
    Loader::getInstance().setLoaderScreenText(
        "LOADING...",
        font,
        sf::Vector2f(window.getSize().x - 300.0f, window.getSize().y - 100.0f),
        24,
        sf::Color(255, 255, 255, 255),
        sf::Color::Black,
        3.0f
    );

    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type) {
            case event.Closed:
                window.close();
                break;
            }
        }
        Loader::setCheckpoint("heavy_task");
        sf::Clock clock;
        float time = 0.0f;
        for (size_t i = 0; time <= 3000.0f; ++i) {
            time = clock.getElapsedTime().asMilliseconds();
            std::cout << i << std::endl;
        }
        clock.restart();
        Loader::clearCheckpoint("heavy_task");
        window.clear();
        window.display();
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }

    return 0;
}