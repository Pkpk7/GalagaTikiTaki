#include "SFML/Graphics.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>

using namespace sf;

int getPt(int n1, int n2, float perc)
{
    int diff = n2 - n1;

    return n1 + (diff * perc);
}

int* getBezierCurveValue(int x1, int x2, int x3, int x4, int y1, int y2, int y3, int y4) {

    static int point[202];
    int iterator = 0;
    for (float i = 0; i < 1; i += 0.01)
    {   // The Green Lines
        int xa = getPt(x1, x2, i);
        int ya = getPt(y1, y2, i);
        int xb = getPt(x2, x3, i);
        int yb = getPt(y2, y3, i);
        int xc = getPt(x3, x4, i);
        int yc = getPt(y3, y4, i);

        // The Blue Line
        int xm = getPt(xa, xb, i);
        int ym = getPt(ya, yb, i);
        int xn = getPt(xb, xc, i);
        int yn = getPt(yb, yc, i);

        // The Black Dot
        int x = getPt(xm, xn, i);
        int y = getPt(ym, yn, i);

        point[iterator] = x;
        point[iterator + 101] = y;

        iterator++;
        //std::cout << "(x - " << point[(int)(i*100)] << "  |  y - " << point[(int)(i*100)+100] << ")  +"<< iterator << std::endl;
    }

    for (int i = 0; i < 10; i++)
        std::cout << std::endl;

    return point;
}



class Bullet {
public:
    Sprite shape;

    Bullet(Texture* texture, Vector2f pos)
    {
        this->shape.setTexture(*texture);
        this->shape.setScale(2, 2);
        this->shape.setOrigin(-7, -215);
        this->shape.setPosition(pos);
        //this->bullet.setPosition(pos);
    }

    ~Bullet() {}
};

class Player {
public:
    Sprite shape;
    Texture* texture;
    int HP;
    int HPMax;

    std::vector<Bullet> bullets;

    Player(Texture* texture) {
        this->HPMax = 3;
        this->HP = this->HPMax;
        this->texture = texture;
        this->shape.setTexture(*texture);
        this->shape.setScale(2,2);
        this->shape.setOrigin(0.f, -225.f);
    }

    ~Player() {}
};


int main(int argc, char** argv) {
    bool canEnemyMove[30];
    std::fill_n(canEnemyMove, 30, true);
    int goDownIterator = 0;
    bool aliensAreGoingDown = true;

    int width = 640;
    int height = 480;
    sf::RenderWindow renderWindow(sf::VideoMode(width, height), "SFML Demo");
    sf::Event event;
    // A Clock starts counting as soon as it's created
    sf::Sprite enemies[30];
    sf::Texture texture3;
    sf::IntRect rectSourceSprite(160, 151, 20, 15);
    sf::Texture texture4;
    sf::IntRect rectSourceSprite2(370, 72, 7, 10);
    sf::Image image;
    image.loadFromFile("sprites.jpg");
    image.createMaskFromColor(sf::Color::Black);



    /*init textures*/
    Texture playerTex;
    playerTex.loadFromFile("shape.png");

    Texture bulletTex;
    bulletTex.loadFromFile("bullets.png");

    //Player init

    int shootTimer = 20;
    Player player(&playerTex);

    /* initialize random seed: */
    srand(time(NULL));

    int xBezier = rand() % 640 + 1;
    int yBezier = rand() % 480 + 240;

    int xBezier2 = rand() % 640 + 1;
    int yBezier2 = rand() % 480 + 240;



    // Load first enemies
    int xChange = 0;
    int yChange = -4;
    texture3.loadFromImage(image);
    for (int i = 0; i < 30; i++) {
        enemies[i].setTextureRect(rectSourceSprite);
        enemies[i].setTexture(texture3);
        if (xChange % 10 == 0) {
            yChange++;
            xChange = 0;
        }
        enemies[i].setPosition(sf::Vector2f(53.f * (xChange + 1), 40.f * yChange));
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


    int movement = 0;
    backgroundSprite.setTexture(texture2);
    backgroundSprite.setOrigin(0, 480);
    int bacgkroundMovement = 1;

    int enemiesAnimationWait = 1;

    int enemiesMovementSpeed = 2;
    int enemiesMovementCount = 0;


    int whichAlienDoesBezier = rand() % 30;
    int* point = getBezierCurveValue(enemies[whichAlienDoesBezier].getPosition().x, xBezier, xBezier2, enemies[whichAlienDoesBezier].getPosition().x, enemies[whichAlienDoesBezier].getPosition().y+200, yBezier, yBezier2, enemies[whichAlienDoesBezier].getPosition().y+200);
    int bezierIterator = 0;

    while (renderWindow.isOpen()) {
        // player
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            player.shape.move(-1.f, 0.f);
        }
        if (Keyboard::isKeyPressed(Keyboard::Right)) {
                player.shape.move(1.f, 0.f);
        }

        // collision with window
        if (player.shape.getPosition().x <= 0) // left
            player.shape.setPosition(0.f, player.shape.getPosition().y);
        else if (player.shape.getPosition().x >= renderWindow.getSize().x - player.shape.getGlobalBounds().width) // right
            player.shape.setPosition(renderWindow.getSize().x - player.shape.getGlobalBounds().width, player.shape.getPosition().y);



        // update
        if (shootTimer < 500) {
            shootTimer++;
        }

        if (Keyboard::isKeyPressed(Keyboard::Space) && shootTimer >=200) {
            player.bullets.push_back(Bullet(&bulletTex,player.shape.getPosition()));
            shootTimer = 0;
        }

        // bullets
        for (size_t i = 0; i < player.bullets.size(); i++) {

            player.bullets[i].shape.move(0.f, -1.f);

            for (int j = 0; j < 30; j++){
                if(player.bullets[i].shape.getGlobalBounds().intersects( enemies[j].getGlobalBounds()) && canEnemyMove[j]) {
                        player.bullets.erase(player.bullets.begin() + i);
                        canEnemyMove[j] = false;
                        if ((j + 1) % 10 == 0) enemies[j].setPosition(enemies[j - 1].getPosition().x + 53.f, enemies[j-1].getPosition().y);
                        else enemies[j].setPosition(enemies[j + 1].getPosition().x - 53.f, enemies[j+1].getPosition().y);

                }
            }



            if (player.bullets[i].shape.getPosition().y > renderWindow.getSize().y) {
                player.bullets.erase(player.bullets.begin() + i);
            }
        }

        enemiesAnimationWait++;

        if (clock.getElapsedTime().asMicroseconds() > 1.0f && enemiesAnimationWait > 230) {
            if (rectSourceSprite.left == 184)
                rectSourceSprite.left = 160;
            else
                rectSourceSprite.left += 24;
            for (int i = 0; i < 30; i++)
                enemies[i].setTextureRect(rectSourceSprite);
            enemiesAnimationWait = 1;
        }

        enemiesMovementCount++;
        if (enemiesMovementCount == 300) {
            enemiesMovementCount = 0;
            enemiesMovementSpeed = -enemiesMovementSpeed;
        }

        //Move enemies left and right
        if (enemiesMovementCount % 60 == 0)
        {
            for (int i = 0; i < 30; i++)
                enemies[i].move(enemiesMovementSpeed, 0);
        }

        if (enemiesMovementCount % 20 == 0 && !aliensAreGoingDown) {

            if(canEnemyMove[whichAlienDoesBezier])
                enemies[whichAlienDoesBezier].setPosition(*(point + bezierIterator), *(point + 101 + bezierIterator));
            //std::cout << *(point+12+100) << std::endl;
            //std::cout << "(x - " << enemies[whichAlienDoesBezier].getPosition().x << "  |  y - " << enemies[whichAlienDoesBezier].getPosition().y  << ")   +  " << bezierIterator << std::endl;
            bezierIterator += 1;
            if (bezierIterator == 101) {
                if(canEnemyMove[whichAlienDoesBezier]){
                    if ((whichAlienDoesBezier + 1) % 10 == 0) enemies[whichAlienDoesBezier].setPosition(enemies[whichAlienDoesBezier - 1].getPosition().x + 53.f, enemies[whichAlienDoesBezier].getPosition().y);
                    else enemies[whichAlienDoesBezier].setPosition(enemies[whichAlienDoesBezier + 1].getPosition().x - 53.f, enemies[whichAlienDoesBezier].getPosition().y);
                }
                whichAlienDoesBezier = rand() % 30;

                bezierIterator = 0;
                xBezier = rand() % 640 + 1;
                yBezier = rand() % 480 + 240;

                xBezier2 = rand() % 640 + 1;
                yBezier2 = rand() % 480 + 240;
                point = getBezierCurveValue(enemies[whichAlienDoesBezier].getPosition().x, xBezier, xBezier2, enemies[whichAlienDoesBezier].getPosition().x, enemies[whichAlienDoesBezier].getPosition().y, yBezier, yBezier2, enemies[whichAlienDoesBezier].getPosition().y);

            }
        }

        backgroundSprite.move(0, bacgkroundMovement);
        sprite.move(movement, 0);

        if (backgroundSprite.getPosition().y == 480) {
            backgroundSprite.move(0, -480);
        }


        //Check if all enemies are killed - if yes initalize them again and make the highscore bigger
        bool allEnemiesAreDead = true;
        for(int i = 0; i<30; i++){
            if(canEnemyMove[i] == true) allEnemiesAreDead = false;
        }

        if(allEnemiesAreDead){
            xChange = 0;
            yChange = -4;
            std::fill_n(canEnemyMove, 30, true);
            bezierIterator = 100;
            for (int i = 0; i < 30; i++) {
                if (xChange % 10 == 0) {
                    yChange++;
                    xChange = 0;
                }
                enemies[i].setPosition(sf::Vector2f(53.f * (xChange + 1), 40.f * yChange));
                xChange++;
            }
            goDownIterator = 0;
            aliensAreGoingDown = true;
        }


        if(goDownIterator < 800){
            for(int j = 0; j<30; j++){
                enemies[j].move(sf::Vector2f(0.f, 0.25f));
            }
            goDownIterator++;
            if(goDownIterator == 799) aliensAreGoingDown = false;
        }

        // A microsecond is 1/1,000,000th of a second, 1000 microseconds == 1 millisecond
        // std::cout << "Elapsed time since previous frame(microseconds): " << clock.getElapsedTime().asMicroseconds() << std::endl;
        // Start the countdown over.  Think of laps on a stop watch.
        clock.restart();

        renderWindow.clear();


        renderWindow.draw(backgroundSprite);
        //renderWindow.draw(sprite);
        for (int i = 0; i < 30; i++) {
            if(canEnemyMove[i])
                renderWindow.draw(enemies[i]);
        }

        renderWindow.draw(player.shape);

        for (size_t i = 0; i < player.bullets.size(); i++) {
            renderWindow.draw(player.bullets[i].shape);
        }


        renderWindow.display();
    }
}
