#include "SFML/Graphics.hpp"
#include <SFML/Audio.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>


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


class Explosion {
public:
    Sprite shape;
    int timer = 0;
    sf::IntRect rectSourceSprite;

    Explosion(Texture* texture, Vector2f pos, int top, int left, int height, int width) {
        this->rectSourceSprite.left = left;
        this->rectSourceSprite.top = top;
        this->rectSourceSprite.height = height;
        this->rectSourceSprite.width = width;
        this->shape.setTextureRect(this->rectSourceSprite);
        this->shape.setTexture(*texture);
        this->shape.setScale(2, 2);
        this->shape.setPosition(Vector2f(pos.x - 15, pos.y - 20));
    }

};

class Bullet {
public:
    Sprite shape;

    Bullet(Texture* texture, Vector2f pos, int originX, int originY)
    {
        this->shape.setTexture(*texture);
        this->shape.setScale(2, 2);
        this->shape.setOrigin(originX, originY);
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
        this->shape.setScale(2, 2);
        this->shape.setPosition(0.f, 440.f);
    }

    ~Player() {}
};


int main(int argc, char** argv) {

    bool isPlayerAlive = true;
    int counterFromDeathToGameOver = 1000;
    int invicible = 0;

    int highscore;
    int score = 0;
    int level = 1;

    //sound effects
    sf::SoundBuffer bufferShooting;
    sf::SoundBuffer bufferAlienExplosion;
    sf::SoundBuffer bufferPlayerExplosion;
    sf::SoundBuffer bufferPlayerDmg;
    sf::SoundBuffer bufferAliensAreFalling;
    sf::Sound sound;



    bufferShooting.loadFromFile("sfx_exp_short_hard14.wav");
    bufferAlienExplosion.loadFromFile("sfx_exp_shortest_soft6.wav");
    bufferPlayerExplosion.loadFromFile("sfx_wpn_laser8.wav");
    bufferPlayerDmg.loadFromFile("sfx_exp_shortest_soft9.wav");
    bufferAliensAreFalling.loadFromFile("sfx_movement_portal2.wav");

    sf::Music music;
    music.openFromFile("Theme Song 8-bit V1 _opening.wav");
    music.setLoop(true);


    std::ifstream readFile;
    readFile.open("highscore.txt");

    if (readFile.is_open())
    {
        while (!readFile.eof())
        {
            readFile >> highscore;
        }
    }
    readFile.close();



    sf::Font font;
    if (!font.loadFromFile("8-bit Arcade In.ttf"))
    {
        std::cout << "Couldnt load the font";
    }
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);


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
    sf::Texture explosionTexture;
    sf::Texture playerDeathTexture;

    sf::Image image;
    image.loadFromFile("sprites.jpg");
    image.createMaskFromColor(sf::Color::Black);




    sf::Image enemyBulletImage;
    enemyBulletImage.loadFromFile("enemyBullet.png");
    enemyBulletImage.createMaskFromColor(sf::Color::Black);

    sf::Image friendlyBulletImage;
    friendlyBulletImage.loadFromFile("bullets.png");
    friendlyBulletImage.createMaskFromColor(sf::Color::Black);


    /*init textures*/
    Texture playerTex;
    playerTex.loadFromFile("shape.png");

    Texture bulletTex;
    bulletTex.loadFromImage(friendlyBulletImage);

    Texture enemyBullet;
    enemyBullet.loadFromImage(enemyBulletImage);

    //Lives init
    Sprite lives[3];

    for (int i = 0; i < 3; i++) {
        lives[i].setTexture(playerTex);
        lives[i].setPosition(sf::Vector2f(5.f, 60 + 20.f * i));
    }

    //Player init

    int shootTimer = 20;
    Player player(&playerTex);

    std::vector<Explosion> explosions;

    /* initialize random seed: */
    srand(time(NULL));

    int shoot = rand() % 100 + 1;
    int shouldShoot = 99;
    int enemyBulletsIterator = 0;
    std::vector<Bullet> enemyBullets;

    int xBezier = rand() % 640 + 1;
    int yBezier = rand() % 480 + 240;

    int xBezier2 = rand() % 640 + 1;
    int yBezier2 = rand() % 480 + 240;


    explosionTexture.loadFromImage(image);

    playerDeathTexture.loadFromImage(image);
    sf::IntRect playerDeathRect(204, 48, 40, 40);

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
    int* point = getBezierCurveValue(enemies[whichAlienDoesBezier].getPosition().x, xBezier, xBezier2, enemies[whichAlienDoesBezier].getPosition().x, enemies[whichAlienDoesBezier].getPosition().y + 200, yBezier, yBezier2, enemies[whichAlienDoesBezier].getPosition().y + 200);
    int bezierIterator = 0;

    sound.setBuffer(bufferAliensAreFalling);
    sound.play();
    music.play();
    while (renderWindow.isOpen()) {

        // player
        if (Keyboard::isKeyPressed(Keyboard::Left) && isPlayerAlive) {
            player.shape.move(-1.f, 0.f);
        }
        if (Keyboard::isKeyPressed(Keyboard::Right) && isPlayerAlive) {
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

        if (Keyboard::isKeyPressed(Keyboard::Space) && shootTimer >= 200 && !aliensAreGoingDown && isPlayerAlive) {
            player.bullets.push_back(Bullet(&bulletTex, player.shape.getPosition(), -7, 10));
            shootTimer = 0;
            sound.setBuffer(bufferShooting);
            sound.play();
        }

        //CheckCollisionOfAliens
        for (int i = 0; i < 30; i++) {
            if (player.shape.getGlobalBounds().intersects(enemies[i].getGlobalBounds()) && canEnemyMove[i] && invicible == 0 && isPlayerAlive) {
                explosions.push_back(Explosion(&explosionTexture, enemies[i].getPosition(), 186, 201, 40, 25));
                canEnemyMove[i] = false;
                player.HP--;
                invicible = 1000;
                sound.setBuffer(bufferAlienExplosion);
                sound.play();
                sound.setBuffer(bufferPlayerDmg);
                sound.play();

            }
        }

        //Create enemy bullets
        enemyBulletsIterator++;
        if (enemyBulletsIterator == 500)
        {
            for (int i = 0; i < 30; i++) {
                if (shoot > shouldShoot && canEnemyMove[i]) {
                    enemyBullets.push_back(Bullet(&enemyBullet, enemies[i].getPosition(), -5, 5));
                    std::cout << "I shot" << std::endl;
                }
                shoot = rand() % 100 + 1;
            }
            enemyBulletsIterator = 0;
        }

        //enemy bullets
        for (size_t i = 0; i < enemyBullets.size(); i++) {

            enemyBullets[i].shape.move(0.f, 0.2f);

            if (enemyBullets[i].shape.getGlobalBounds().intersects(player.shape.getGlobalBounds()) && invicible == 0 && isPlayerAlive) {
                enemyBullets.erase(enemyBullets.begin() + i);
                player.HP--;
                invicible = 1000;
                sound.setBuffer(bufferPlayerDmg);
                sound.play();
            }


            //std::cout << enemyBullets[i].shape.getPosition().y << std::endl;
            if (enemyBullets[i].shape.getPosition().y > (int)(renderWindow.getSize().y)) {
                enemyBullets.erase(enemyBullets.begin() + i);
                //std::cout << "DETELE";
            }
        }


        // bullets
        for (size_t i = 0; i < player.bullets.size(); i++) {

            player.bullets[i].shape.move(0.f, -1.f);

            for (int j = 0; j < 30; j++) {
                if (player.bullets[i].shape.getGlobalBounds().intersects(enemies[j].getGlobalBounds()) && canEnemyMove[j]) {

                    sound.setBuffer(bufferAlienExplosion);
                    sound.play();
                    explosions.push_back(Explosion(&explosionTexture, enemies[j].getPosition(), 186, 201, 40, 25));
                    player.bullets.erase(player.bullets.begin() + i);
                    canEnemyMove[j] = false;
                    if ((j + 1) % 10 == 0) enemies[j].setPosition(enemies[j - 1].getPosition().x + 53.f, enemies[j - 1].getPosition().y);
                    else enemies[j].setPosition(enemies[j + 1].getPosition().x - 53.f, enemies[j + 1].getPosition().y);
                    score += 10;


                }
            }


            if (player.bullets[i].shape.getPosition().y < -(int)(renderWindow.getSize().y)) {
                player.bullets.erase(player.bullets.begin() + i);
                //std::cout << "DETELE";
            }
        }


        //animate explosions
        for (size_t i = 0; i < explosions.size(); i++) {
            explosions[i].timer++;
            if (explosions[i].timer == 100) {
                explosions[i].rectSourceSprite.left += 25;
                explosions[i].shape.setTextureRect(explosions[i].rectSourceSprite);
            }
            else if (explosions[i].timer == 200) {
                explosions[i].rectSourceSprite.left += 25;
                explosions[i].shape.setTextureRect(explosions[i].rectSourceSprite);
            }
            else if (explosions[i].timer == 300) {
                explosions[i].rectSourceSprite.left += 25;
                explosions[i].rectSourceSprite.width = 40;
                explosions[i].shape.setTextureRect(explosions[i].rectSourceSprite);
            }
            else if (explosions[i].timer == 400) {
                explosions[i].rectSourceSprite.left += 40;
                explosions[i].shape.setTextureRect(explosions[i].rectSourceSprite);
            }
            else if (explosions[i].timer == 500) {
                explosions.erase(explosions.begin() + i);
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

            if (canEnemyMove[whichAlienDoesBezier])
                enemies[whichAlienDoesBezier].setPosition(*(point + bezierIterator), *(point + 101 + bezierIterator));
            //std::cout << *(point+12+100) << std::endl;
            //std::cout << "(x - " << enemies[whichAlienDoesBezier].getPosition().x << "  |  y - " << enemies[whichAlienDoesBezier].getPosition().y  << ")   +  " << bezierIterator << std::endl;
            bezierIterator += 1;
            if (bezierIterator == 101) {
                if (canEnemyMove[whichAlienDoesBezier]) {
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
        for (int i = 0; i < 30; i++) {
            if (canEnemyMove[i] == true) allEnemiesAreDead = false;
        }

        if (allEnemiesAreDead) {
            sound.setBuffer(bufferAliensAreFalling);
            sound.play();
            level++;
            if (shouldShoot > 80) shouldShoot -= 1;
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


        if (goDownIterator < 800) {
            for (int j = 0; j < 30; j++) {
                enemies[j].move(sf::Vector2f(0.f, 0.25f));
            }
            goDownIterator++;
            if (goDownIterator == 799) aliensAreGoingDown = false;
        }

        text.setString(" HighScore " + std::to_string(highscore) + "\n Score " + std::to_string(score) + "\n LVL " + std::to_string(level));

        // A microsecond is 1/1,000,000th of a second, 1000 microseconds == 1 millisecond
        // std::cout << "Elapsed time since previous frame(microseconds): " << clock.getElapsedTime().asMicroseconds() << std::endl;
        // Start the countdown over.  Think of laps on a stop watch.
        clock.restart();

        renderWindow.clear();


        renderWindow.draw(backgroundSprite);
        //renderWindow.draw(sprite);
        for (int i = 0; i < 30; i++) {
            if (canEnemyMove[i])
                renderWindow.draw(enemies[i]);
        }

        if (invicible % 2 == 0)
            renderWindow.draw(player.shape);


        if (invicible != 0) invicible--;

        for (size_t i = 0; i < player.bullets.size(); i++) {
            renderWindow.draw(player.bullets[i].shape);
        }

        for (size_t i = 0; i < enemyBullets.size(); i++) {
            renderWindow.draw(enemyBullets[i].shape);
        }

        for (size_t i = 0; i < explosions.size(); i++) {
            renderWindow.draw(explosions[i].shape);
        }

        for (int i = 0; i < player.HP; i++) {
            renderWindow.draw(lives[i]);
        }

        if (player.HP == 0) {
            invicible = 0;
            isPlayerAlive = false;
            player.shape.setTexture(playerDeathTexture);
            counterFromDeathToGameOver--;
            if (counterFromDeathToGameOver == 800) {
                sound.setBuffer(bufferPlayerExplosion);
                sound.play();
                player.shape.move(-16.f, -11.f);
                playerDeathRect.left += 40;
                player.shape.setTextureRect(playerDeathRect);
            }
            else if (counterFromDeathToGameOver == 600) {
                playerDeathRect.left += 40;
                player.shape.setTextureRect(playerDeathRect);
            }
            else if (counterFromDeathToGameOver == 400) {
                playerDeathRect.left += 40;
                player.shape.setTextureRect(playerDeathRect);
            }
            else if (counterFromDeathToGameOver == 0) {
                renderWindow.close();
            }
        }

        renderWindow.draw(text);

        renderWindow.display();
    }

    std::ofstream writeFile;
    writeFile.open("highscore.txt");

    if (writeFile.is_open())
    {
        if (score > highscore)
        {
            highscore = score;
        }
        writeFile << highscore;
    }
    writeFile.close();
}
