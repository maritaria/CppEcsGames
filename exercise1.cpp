// CppHelloWorld.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <memory>

class Floater
{
    std::shared_ptr<sf::Shape> m_shape;
    std::shared_ptr<sf::Text> m_text;
    sf::Vector2f m_velocity;
public:
    Floater(
        std::shared_ptr<sf::Shape> shape,
        std::shared_ptr<sf::Text> text,
        sf::Vector2f velocity
    )
        : m_shape(shape)
        , m_text(text)
        , m_velocity(velocity)
    {}

    void update(sf::RenderWindow& window)
    {
        // Move floater
        auto pos = m_shape->getPosition();
        auto newPos = pos + m_velocity;
        // Check for bounce
        if (newPos.x < 0) {
            newPos.x *= -1;
            m_velocity.x *= -1;
        }
        if (newPos.y < 0) {
            newPos.y *= -1;
            m_velocity.y *= -1;
        }
        auto shapeBounds = m_shape->getLocalBounds();
        auto windowSize = window.getSize() - sf::Vector2u(shapeBounds.width, shapeBounds.height);
        if (newPos.x >= windowSize.x) {
            auto delta = (newPos.x - windowSize.x);
            newPos.x -= delta * 2;
            m_velocity.x *= -1;
        }
        if (newPos.y >= windowSize.y) {
            auto delta = (newPos.y - windowSize.y);
            newPos.y -= delta * 2;
            m_velocity.y *= -1;
        }
        // Apply positioning
        m_shape->setPosition(newPos);
        // Set text pos
        auto shapeSize = sf::Vector2f(shapeBounds.width, shapeBounds.height);
        auto textBounds = m_text->getLocalBounds();
        auto textSize = sf::Vector2f(textBounds.width, textBounds.height);
        m_text->setPosition(newPos + (shapeSize - textSize) / 2.0f);
        // Render
        window.draw(*m_shape);
        window.draw(*m_text);
    }
};

int main()
{
    // Print runtime info
    std::cout << "Working directory: " << std::filesystem::current_path() << std::endl;

    // Init window and data
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "SFML works!");
    sf::Font font;
    int fontSize = -1;
    sf::Color fontColor;

    std::vector<Floater> floaters;

    // Open config
    std::ifstream file("resources/config.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Failed to load configuration file!" << std::endl;
        exit(-1);
    }

    // Parse config
    std::string lineText;
    while (file.good() && std::getline(file, lineText)) {
        std::istringstream line(lineText);
        std::string instruction;
        if (!(line >> instruction)) { continue; }
        if (instruction == "Window") {
            int width, height;
            line >> width >> height;
            window.setSize(sf::Vector2u(width, height));
            window.setView(sf::View(sf::FloatRect(0, 0, width, height)));

        }
        else if (instruction == "Font") {
            std::string fontPath;
            int r, g, b;
            line >> fontPath >> fontSize >> r >> g >> b;
            if (!font.loadFromFile(fontPath)) {
                std::cerr << "Error: Failed to load font!" << std::endl;
                exit(-1);
            }
            fontColor = sf::Color(r, g, b);
        }
        else {
            if (fontSize <= 0) {
                std::cerr << "Error: Creating shape before specifying font" << std::endl;
                exit(-1);
            }

            // Shapes
            std::string name;
            float x, y;
            float speedX, speedY;
            int r, g, b;
            line >> name >> x >> y >> speedX >> speedY >> r >> g >> b;
            std::shared_ptr<sf::Shape> shape;

            // Shape-specific parameters
            if (instruction == "Circle") {
                float radius;
                line >> radius;
                shape = std::make_shared<sf::CircleShape>(radius, 16);
            }
            else if (instruction == "Rectangle") {
                float width, height;
                line >> width >> height;
                shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(width, height));
            }
            shape->setPosition(x, y);
            shape->setFillColor(sf::Color(r, g, b));
            auto text = std::make_shared<sf::Text>(name, font, fontSize);
            text->setFillColor(fontColor);
            floaters.push_back(Floater(shape, text, sf::Vector2f(speedX, speedY)));
        }
    }

    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();
        for (auto& floater : floaters) {
            floater.update(window);
        }
        window.display();
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
