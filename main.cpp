#include "SFML/Graphics.hpp"
#include <iostream>

int main(int argc, char** argv) {
    int width = 640;
    int height = 480;
    sf::RenderWindow renderWindow(sf::VideoMode(width, height), "SFML Demo");
    sf::Event event;
    // A Clock starts counting as soon as it's created
    sf::Sprite enemies[30];
    sf::Texture texture3;
    sf::IntRect rectSourceSprite(160, 151, 20, 15);


    // Load first enemies
    int xChange = 0;
    int yChange = 1;
    texture3.loadFromFile("sprites.jpg");
    for (int i = 0; i < 30; i++) {
        enemies[i].setTextureRect(rectSourceSprite);
        enemies[i].setTexture(texture3);
        if(xChange%10 == 0) {
                yChange++;
                xChange = 0;
        }
        enemies[i].setPosition(sf::Vector2f(50.f*(xChange+1), 40.f*yChange));
        xChange++;
        enemies[i].setScale(2, 2);
    }


    sf::Clock clock;
    sf::Sprite sprite;
    sf::Sprite backgroundSprite;

    // Load texture
    sf::Texture texture;
    if (!texture.loadFromFile("sprites.jpg", sf::IntRect(157, 53, 20, 20)))
    {
        std::cout << "Error while loading the sprite!";
    }

    // Load background texture
    sf::Texture texture2;
    if (!texture2.loadFromFile("backgroundSprite.jpg"))
    {
        std::cout << "Error while loading the sprite!";
    }

    // Load sprites
    sprite.setTexture(texture);
    sprite.setOrigin(sf::Vector2f(0.f, -215.f));
    sprite.setScale(2, 2);
    int movement = 0;

    backgroundSprite.setTexture(texture2);
    backgroundSprite.setOrigin(0, 480);
    int bacgkroundMovement = 1;

    int enemiesAnimationWait = 1;
    while (renderWindow.isOpen()) {
        // Check for all the events that occured since the last frame.
        while (renderWindow.pollEvent(event)) {
            if (event.type == sf::Event::EventType::Closed)
                renderWindow.close();



        }
        enemiesAnimationWait++;

         if (clock.getElapsedTime().asMicroseconds() > 1.0f && enemiesAnimationWait > 300) {
                if (rectSourceSprite.left == 184)
                    rectSourceSprite.left = 160;
                else
                    rectSourceSprite.left += 24;
                for(int i =0; i<30; i++)
                    enemies[i].setTextureRect(rectSourceSprite);
                enemiesAnimationWait = 1;
            }


        //Handle events here
        switch (event.type) {
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Left && sprite.getPosition().x > 0) {
                movement = -1;
            }
            else if (event.key.code == sf::Keyboard::Right && sprite.getPosition().x < 600) {
                movement = 1;
            }
            else if (event.key.code == sf::Keyboard::Right && sprite.getPosition().x >= 600) {
                movement = 0;
            }
            else if (event.key.code == sf::Keyboard::Left && sprite.getPosition().x <= 0) {
                movement = 0;
            }
            break;
        case sf::Event::KeyReleased:
            if (event.key.code == sf::Keyboard::Left) {
                movement = 0;
            }
            else if (event.key.code == sf::Keyboard::Right) {
                movement = 0;
            }
            break;
        default:
            movement = 0;
            break;
        }
        backgroundSprite.move(0, bacgkroundMovement);
        sprite.move(movement, 0);

        if (backgroundSprite.getPosition().y == 480) {
            backgroundSprite.move(0, -480);
        }

        // A microsecond is 1/1,000,000th of a second, 1000 microseconds == 1 millisecond
        // std::cout << "Elapsed time since previous frame(microseconds): " << clock.getElapsedTime().asMicroseconds() << std::endl;
        // Start the countdown over.  Think of laps on a stop watch.
        clock.restart();

        renderWindow.clear();

        renderWindow.draw(backgroundSprite);
        renderWindow.draw(sprite);
        for(int i=0; i<30; i++)
            renderWindow.draw(enemies[i]);

        renderWindow.display();
    }
}

