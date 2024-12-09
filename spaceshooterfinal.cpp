#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <fstream>
using namespace std;

const int win_width = 800;
const int win_height = 600;
const int fighter_speed = 5;
const int bullet_speed = 7;
const int total_bullets = 20;
const int max_aliens_total = 30;  
const int initial_aliens_max = 10;
const int lives = 5;
const int levels_max = 5;
const string HighScore = "highscore.txt";

int customMin(int a, int b) {
    return (a < b) ? a : b;
}

int customMax(int a, int b) {
    return (a > b) ? a : b;
}

struct Savior {
    sf::RectangleShape shapesavior;
};

struct galaxyaliens {
    sf::RectangleShape shapealien;
    bool kill;
};

struct bullet {
    sf::RectangleShape shapebullet;
    bool fire;
};

int printhighscore() {
    ifstream openhighScore(HighScore);
    int highScore = 0;
    if (openhighScore.is_open()) {
        openhighScore >> highScore;
        openhighScore.close();
    }
    return highScore;
}

void writehighscore(int score) {
    ofstream highScoreFile(HighScore);
    if (highScoreFile.is_open()) {
        highScoreFile << score;
        highScoreFile.close();
    }
}

void saviorlooks(Savior& player) {
    player.shapesavior.setSize(sf::Vector2f(30, 50));
    player.shapesavior.setFillColor(sf::Color::Cyan);
    player.shapesavior.setPosition(win_width / 2 - 24, win_height - 68);
}

void movesavior(Savior& player, sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Right && player.shapesavior.getPosition().x + player.shapesavior.getSize().x < win_width)
        player.shapesavior.move(fighter_speed, 0);
    else if (key == sf::Keyboard::Left && player.shapesavior.getPosition().x > 0)
        player.shapesavior.move(-fighter_speed, 0);

}

void bulletapp(bullet& bullet) {
    bullet.shapebullet.setSize(sf::Vector2f(5, 10));
    bullet.shapebullet.setFillColor(sf::Color::Red);
    bullet.fire = false;
}

void bulletfire(bullet& bullet, float x, float y) {
    bullet.shapebullet.setPosition(x, y);
    bullet.fire = true;
}

void bulletmove(bullet& bullet) {
    if (bullet.fire) {
        bullet.shapebullet.move(0, -bullet_speed);
        if (bullet.shapebullet.getPosition().y < 0)
            bullet.fire = false;
    }
}

void alienslooks(galaxyaliens& enemy) {
    enemy.shapealien.setSize(sf::Vector2f(50, 50));
    enemy.shapealien.setFillColor(sf::Color::Magenta);
    enemy.kill = false;
}

void activeenemy(galaxyaliens& enemy, float x, float y) {
    enemy.shapealien.setPosition(x, y);
    enemy.kill = true;
}

void movealiens(galaxyaliens& enemy, int enemySpeed) {
    if (enemy.kill) {
        enemy.shapealien.move(0, enemySpeed);
        if (enemy.shapealien.getPosition().y > win_height)
            enemy.kill = false;
    }
}

bool collision(const sf::RectangleShape& a, const sf::RectangleShape& b) {
    return a.getGlobalBounds().intersects(b.getGlobalBounds());
}

bool displayStartMenu(sf::RenderWindow& window, sf::Font& font, int highScore) {
    sf::Text title, instructions, startMessage, highScoreText;
    title.setFont(font);
    instructions.setFont(font);
    startMessage.setFont(font);
    highScoreText.setFont(font);

    title.setString("Galaxy me tabahi");
    title.setCharacterSize(40);
    title.setFillColor(sf::Color::White);
    title.setPosition(win_width / 2 - 148, 102);

    instructions.setString(
        "Instructions:\n"
        "- Press Left and Right arrow keys to move.\n"
        "- Press Space to fire at the alien spaceships.\n"
        "- The level increases after every 200 points.\n"
        "- There are 5 levels. Complete all to win.\n"
    );
    instructions.setCharacterSize(20);
    instructions.setFillColor(sf::Color::White);
    instructions.setPosition(win_width / 2 - 298, 202);

    highScoreText.setString("High Score: " + std::to_string(highScore));
    highScoreText.setCharacterSize(25);
    highScoreText.setFillColor(sf::Color::Yellow);
    highScoreText.setPosition(win_width / 2 - 101, 348);

    startMessage.setString("Press Enter to start...");
    startMessage.setCharacterSize(32);
    startMessage.setFillColor(sf::Color::Yellow);
    startMessage.setPosition(win_width / 2 - 149, 502);

    sf::RectangleShape border(sf::Vector2f(win_width - 98, win_height - 97));
    border.setOutlineThickness(10);
    border.setOutlineColor(sf::Color::White);
    border.setFillColor(sf::Color::Transparent);
    border.setPosition(40, 40);

    while (true) {
        window.clear();
        window.draw(border);
        window.draw(title);
        window.draw(instructions);
        window.draw(startMessage);
        window.draw(highScoreText);
        window.display();

        sf::Event keypress;
        while (window.pollEvent(keypress)) {
            if (keypress.type == sf::Event::KeyPressed && keypress.key.code == sf::Keyboard::Enter) {
                return true; 
            }
            if (keypress.type == sf::Event::Closed) {
                window.close();
                return false;
            }
        }
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(win_width, win_height), "Galaxy ko bachao");
    window.setFramerateLimit(61);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return -1;
    }

    int highScore = printhighscore();

    if (!displayStartMenu(window, font, highScore)) {
        return 0; 
    }

    Savior fighter;
    saviorlooks(fighter);

    galaxyaliens aliens[max_aliens_total];
    bullet bullets[total_bullets];

    for (int i = 0; i < total_bullets; ++i) bulletapp(bullets[i]);
    for (int i = 0; i < max_aliens_total; ++i) alienslooks(aliens[i]);

    int score = 0;
    int remaininglives = lives;
    int level = 1;
    int alienvelocity = 3;
    int aliens_max_current = initial_aliens_max;

    sf::Text scoreText, levelText, highScoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(22);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(8, 8);

    levelText.setFont(font);
    levelText.setCharacterSize(22);
    levelText.setFillColor(sf::Color::White);
    levelText.setPosition(10, 38);

    highScoreText.setFont(font);
    highScoreText.setCharacterSize(25);
    highScoreText.setFillColor(sf::Color::Yellow);
    highScoreText.setPosition(275, 8);

    sf::CircleShape hearts[lives];
    for (int i = 0; i < lives; ++i) {
        hearts[i].setRadius(12);
        hearts[i].setFillColor(sf::Color::Red);
        hearts[i].setPosition(10 + i * 25, 70);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                
                if (score > highScore) {
                    writehighscore(score);
                }
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            movesavior(fighter, sf::Keyboard::Left);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            movesavior(fighter, sf::Keyboard::Right);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            for (int i = 0; i < total_bullets; ++i) {
                if (!bullets[i].fire) {
                    bulletfire(bullets[i], fighter.shapesavior.getPosition().x + 22, fighter.shapesavior.getPosition().y);
                    break;
                }
            }
        }

        if (rand() % customMax(50 - (level * 5), 10) == 0) {
            for (int i = 0; i < aliens_max_current; ++i) {
                if (!aliens[i].kill) {
                    activeenemy(aliens[i], rand() % (win_width - 48), 0);
                    break;
                }
            }
        }

        for (int i = 0; i < total_bullets; ++i) bulletmove(bullets[i]);
        for (int i = 0; i < aliens_max_current; ++i) movealiens(aliens[i], alienvelocity);

        for (int i = 0; i < total_bullets; ++i) {
            if (bullets[i].fire) {
                for (int j = 0; j < aliens_max_current; ++j) {
                    if (aliens[j].kill && collision(bullets[i].shapebullet, aliens[j].shapealien)) {
                        bullets[i].fire = false;
                        aliens[j].kill = false;
                        score += 10;
                    }
                }
            }
        }

        if (score / 200 + 1 > level) {
            level = score / 200 + 1;
            alienvelocity += 1;

            aliens_max_current = customMin(initial_aliens_max + (level - 1) * 2, max_aliens_total);
        }

        if (level > levels_max) {
            sf::Text victoryText, restartText;
            victoryText.setFont(font);
            victoryText.setString("Congratulations! You've defended the galaxy for now");
            victoryText.setCharacterSize(30);
            victoryText.setFillColor(sf::Color::Yellow);
            victoryText.setPosition(win_width / 2 - 200, win_height / 2 - 49);

            restartText.setFont(font);
            restartText.setString("Press Enter to restart or Esc to exit");
            restartText.setCharacterSize(24);
            restartText.setFillColor(sf::Color::Cyan);
            restartText.setPosition(win_width / 2 - 251, win_height / 2 + 51);

            if (score > highScore) {
                highScore = score;
                writehighscore(highScore);
            }

            while (window.isOpen()) {
                window.clear();
                window.draw(victoryText);
                window.draw(restartText);
                window.display();

                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::KeyPressed) {
                        if (event.key.code == sf::Keyboard::Enter) {
                            score = 0;
                            remaininglives = lives;
                            level = 1;
                            alienvelocity = 3;
                            aliens_max_current = initial_aliens_max;

                            for (int i = 0; i < total_bullets; ++i) bullets[i].fire = false;
                            for (int i = 0; i < max_aliens_total; ++i) aliens[i].kill = false;
                            
                        }
                        if (event.key.code == sf::Keyboard::Escape) {
                            window.close();
                            return 0;
                        }
                    }
                    if (event.type == sf::Event::Closed) {
                        window.close();
                        return 0;
                    }
                }

                if (level == 1) break;
            }
        }

        for (int i = 0; i < aliens_max_current; ++i) {
            if (aliens[i].kill && collision(fighter.shapesavior, aliens[i].shapealien)) {
                remaininglives = remaininglives - 1;
                aliens[i].kill = false;
                if (remaininglives <= 0) {
                    
                    if (score > highScore) {
                        highScore = score;
                        writehighscore(highScore);
                    }
                    window.close();
                    break;
                }
            }
        }

        window.clear();
        window.draw(fighter.shapesavior);
        for (int i = 0; i < total_bullets; ++i)
            if (bullets[i].fire) window.draw(bullets[i].shapebullet);
        for (int i = 0; i < aliens_max_current; ++i)
            if (aliens[i].kill) window.draw(aliens[i].shapealien);

        scoreText.setString("Score: " + to_string(score));
        window.draw(scoreText);

        levelText.setString("Level: " + to_string(level));
        window.draw(levelText);

        highScoreText.setString("High Score: " + to_string(highScore));
        window.draw(highScoreText);

        for (int i = 0; i < remaininglives; ++i) window.draw(hearts[i]);
        window.display();
    }

    return 0;
}