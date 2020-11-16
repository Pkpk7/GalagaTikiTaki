#include "SFML/Graphics.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int getPt( int n1 , int n2 , float perc )
{
    int diff = n2 - n1;

    return n1 + ( diff * perc );
}

int * getBezierCurveValue(int x1, int x2, int x3, int x4, int y1, int y2, int y3, int y4){

    static int point[202];
    int iterator = 0;
    for(float i = 0 ; i < 1 ; i += 0.01)
    {   // The Green Lines
        int xa = getPt( x1 , x2 , i );
        int ya = getPt( y1 , y2 , i );
        int xb = getPt( x2 , x3 , i );
        int yb = getPt( y2 , y3 , i );
        int xc = getPt( x3 , x4 , i );
        int yc = getPt( y3 , y4 , i );

        // The Blue Line
        int xm = getPt( xa , xb , i );
        int ym = getPt( ya , yb , i );
        int xn = getPt( xb , xc , i );
        int yn = getPt( yb , yc , i );

        // The Black Dot
        int x = getPt( xm , xn , i );
        int y = getPt( ym , yn , i );

        point[iterator] = x;
        point[iterator+101] = y;

        iterator++;
        //std::cout << "(x - " << point[(int)(i*100)] << "  |  y - " << point[(int)(i*100)+100] << ")  +"<< iterator << std::endl;
    }

    for(int i = 0; i<10; i++)
        std::cout << std::endl;

    return point;
}

int main(int argc, char** argv) {
    int width = 640;
    int height = 480;
    sf::RenderWindow renderWindow(sf::VideoMode(width, height), "SFML Demo");
    sf::Event event;
    // A Clock starts counting as soon as it's created
    sf::Sprite enemies[30];
    sf::Texture texture3;
    sf::IntRect rectSourceSprite(160, 151, 20, 15);

    /* initialize random seed: */
    srand (time(NULL));

    int xBezier = rand() % 640 + 1;
    int yBezier = rand() % 480 + 240;

    int xBezier2 = rand() % 640 + 1;
    int yBezier2 = rand() % 480 + 240;

    sf::Image image;
    image.loadFromFile("sprites.jpg");
    image.createMaskFromColor(sf::Color::Black);


    // Load first enemies
    int xChange = 0;
    int yChange = 1;
    texture3.loadFromImage(image);
    for (int i = 0; i < 30; i++) {
        enemies[i].setTextureRect(rectSourceSprite);
        enemies[i].setTexture(texture3);
        if(xChange%10 == 0) {
                yChange++;
                xChange = 0;
        }
        enemies[i].setPosition(sf::Vector2f(53.f*(xChange+1), 40.f*yChange));
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

    int enemiesMovementSpeed = 2;
    int enemiesMovementCount = 0;


    int whichAlienDoesBezier = rand() % 30;
    int  * point = getBezierCurveValue(enemies[whichAlienDoesBezier].getPosition().x, xBezier, xBezier2, enemies[whichAlienDoesBezier].getPosition().x, enemies[whichAlienDoesBezier].getPosition().y, yBezier, yBezier2, enemies[whichAlienDoesBezier].getPosition().y);
    int bezierIterator = 0;

    while (renderWindow.isOpen()) {
        // Check for all the events that occured since the last frame.
        while (renderWindow.pollEvent(event)) {
            if (event.type == sf::Event::EventType::Closed)
                renderWindow.close();



        }
        enemiesAnimationWait++;

         if (clock.getElapsedTime().asMicroseconds() > 1.0f && enemiesAnimationWait > 230) {
                if (rectSourceSprite.left == 184)
                    rectSourceSprite.left = 160;
                else
                    rectSourceSprite.left += 24;
                for(int i =0; i<30; i++)
                    enemies[i].setTextureRect(rectSourceSprite);
                enemiesAnimationWait = 1;
            }

        enemiesMovementCount++;
        if(enemiesMovementCount == 300){
            enemiesMovementCount = 0;
            enemiesMovementSpeed = -enemiesMovementSpeed;
        }

        //Move enemies left and right
        if(enemiesMovementCount % 60 == 0)
        {
            for(int i =0; i<30; i++)
                enemies[i].move(enemiesMovementSpeed, 0);
        }

        if(enemiesMovementCount % 20 == 0){

            enemies[whichAlienDoesBezier].setPosition(*(point+bezierIterator), *(point+101+bezierIterator));
            //std::cout << *(point+12+100) << std::endl;
            //std::cout << "(x - " << enemies[whichAlienDoesBezier].getPosition().x << "  |  y - " << enemies[whichAlienDoesBezier].getPosition().y  << ")   +  " << bezierIterator << std::endl;
            bezierIterator+=1;
            if(bezierIterator == 101) {
                    if((whichAlienDoesBezier+1) % 10 == 0) enemies[whichAlienDoesBezier].setPosition(enemies[whichAlienDoesBezier-1].getPosition().x+53.f, enemies[whichAlienDoesBezier].getPosition().y);
                    else enemies[whichAlienDoesBezier].setPosition(enemies[whichAlienDoesBezier+1].getPosition().x-53.f, enemies[whichAlienDoesBezier].getPosition().y);

                    whichAlienDoesBezier = rand() % 30;

                    bezierIterator = 0;
                    xBezier = rand() % 640 + 1;
                    yBezier = rand() % 480 + 240;

                    xBezier2 = rand() % 640 + 1;
                    yBezier2 = rand() % 480 + 240;
                    point =  getBezierCurveValue(enemies[whichAlienDoesBezier].getPosition().x, xBezier, xBezier2, enemies[whichAlienDoesBezier].getPosition().x, enemies[whichAlienDoesBezier].getPosition().y, yBezier, yBezier2, enemies[whichAlienDoesBezier].getPosition().y);

            }
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

