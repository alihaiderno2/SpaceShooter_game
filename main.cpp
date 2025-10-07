#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <thread>//for delay in  sounds
#include <chrono>
using namespace sf;
using namespace std;
// Data
// Enemy structure
struct Enemy1 {
    Texture texture;
    Sprite sprite;
    float speed;

    // Loader of the enemy
    Enemy1(Texture& texture, float scale = 0.3f, float speedValue = 100.0f) {
        sprite.setTexture(texture);
        sprite.setScale(scale, scale);
        speed = speedValue;
    }

    // Set initial position
    void setPosition(float x, float y) {
        sprite.setPosition(x, y);
    }

    // Move enemy downward
    void update(float dt) {
        sprite.move(0, speed * dt);
    }

    // Draw enemy on screen
    void draw(RenderWindow& window) {
        window.draw(sprite);
    }
};


// Bullet structure
struct Bullet{
    Texture texture;
    Sprite sprite;
    float speed;

    // Loader of the bullet
    Bullet(Texture& texture, float speedValue = 300.0f){
        sprite.setTexture(texture);
        sprite.setScale(0.055f, 0.055f);
        speed = speedValue;
    }

    // Set initial position
    void setPosition(float x, float y){
        sprite.setPosition(x, y);
    }

    // Move bullet upward
    void update(float dt){
        sprite.move(0, -speed * dt);
    }

    // Draw bullet on screen
    void draw(RenderWindow& window){
        window.draw(sprite);
    }
    bool isOffScreen(int screenHeight) {
    return sprite.getPosition().y + sprite.getGlobalBounds().height < 0;
    }
};

// Clock for enemies spawn
    Clock spawnClock;     // used to track time since last enemy spawn
    float spawnInterval = 2.0f;  // spawn a new enemy every 2 seconds initially
    Clock fireClock;
    float fireInterval = 0.3f;
    Clock gameClock;
    Clock soundClock;
// Global Enemy List
    vector<Enemy1> enemies;
// Global Bullet List
    vector<Bullet> bullets;

bool Mainmenu(bool gamestarted);
int game(bool gameStarted);
int main()
{
    //  Game Start Indicator
    bool gameStarted = false;
    gameStarted = Mainmenu(gameStarted);
    // if game load error occurs
    int error = game(gameStarted);
    if (error == -1)
    {
        return -1;
    }
    return 0;
}
// Abdullah Bhai implement krdana
bool Mainmenu(bool gamestarted)
{
    VideoMode desktop = VideoMode::getDesktopMode();
    RenderWindow menuWindow(desktop, "Main Menu");

    Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        return -1;

    RectangleShape button(Vector2f(200, 60));
    int centerw = desktop.width / 2;
    int centerh = desktop.height / 2 - 30;
    button.setPosition(centerw, centerh);
    button.setFillColor(Color::Blue);

    Text buttonText("Login", font, 28);
    buttonText.setFillColor(Color::White);
    buttonText.setPosition(centerw + 30, centerh + 10);

    bool gameStarted = false;

    while (menuWindow.isOpen() && !gameStarted)
    {
        Event event;
        while (menuWindow.pollEvent(event))
        {
            if (event.type == Event::Closed)
                menuWindow.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                Vector2f mousePos = menuWindow.mapPixelToCoords(Mouse::getPosition(menuWindow));
                if (button.getGlobalBounds().contains(mousePos))
                {
                    gameStarted = true;
                    menuWindow.close();
                    return true;
                }
            }
        }
        menuWindow.clear(Color::Black);
        menuWindow.draw(button);
        menuWindow.draw(buttonText);
        menuWindow.display();
    }
}
// Game Logic
int game(bool gameStarted)
{
    //loading sound from file (BUllets)
     SoundBuffer bufferBullet;
    if (!bufferBullet.loadFromFile("assets/shoot.wav")) {
        std::cout << "Error: Could not load sound.wav\n";
        return -1;
    }

   Sound soundBullet;
    soundBullet.setBuffer(bufferBullet);

    // Optional: set volume and pitch
    soundBullet.setVolume(50);
    soundBullet.setPitch(0.6f);

    VideoMode desktop = VideoMode::getDesktopMode();
    int centerw = desktop.width / 2;
    int centerh = desktop.height / 2 - 30;
    if (gameStarted)
    {
        RenderWindow gameWindow(desktop, "Space Shooter", Style::Default);
        Texture shipTexture;
        if (!shipTexture.loadFromFile("assets/spaceship.png"))
        {
            return -1;
        }

        Sprite player(shipTexture);
        player.setScale(0.2f, 0.2f);
        player.setPosition(desktop.width - centerw - player.getGlobalBounds().width, desktop.height - (centerh / 2));

        Texture bulletTexture;
        if (!bulletTexture.loadFromFile("assets/bullet.png"))
        {
            return -1;
        }

        Clock clock;

        while (gameWindow.isOpen())
        {
            float dt = clock.restart().asSeconds();
            float speed = 300.0f;

            Event event;
            while (gameWindow.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    gameWindow.close();
            }
            // Left movement
            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                player.move(-speed * dt, 0);
                // (*bullet_asset).move(-speed * dt, 0);
            }
            // Up movement
            if (Keyboard::isKeyPressed(Keyboard::Up))
            {
                player.move(0, -speed * dt);
                // (*bullet_asset).move(0, -speed * dt);
            }
            if (Keyboard::isKeyPressed(Keyboard::Down))
            {
                player.move(0, speed * dt);
                // (*bullet_asset).move(0, speed * dt);
            }
            // Right movement
            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                player.move(speed * dt, 0);
                // (*bullet_asset).move(speed * dt, 0);
            }
            if (player.getPosition().x < 0)
            {
                player.setPosition(0, player.getPosition().y);
            }
            if (player.getPosition().y < 0)
            {
                player.setPosition(player.getPosition().x, 0);
            }
            // Left boundry spaceship
            if ((player.getPosition().x + (player.getGlobalBounds().width)) >= desktop.width)
            {
                player.setPosition(desktop.width - player.getGlobalBounds().width, player.getPosition().y);
            }

            if ((player.getPosition().y + (player.getGlobalBounds().height)) >= desktop.height)
            {
                player.setPosition(player.getPosition().x, desktop.height - player.getGlobalBounds().height);
            }
            gameWindow.clear(Color::Black);

            // Bullets Loading condition
            if (fireClock.getElapsedTime().asSeconds() > fireInterval) {
                Bullet newBullet(bulletTexture, 600.0f);
                newBullet.setPosition(
                    player.getPosition().x + player.getGlobalBounds().width / 2 - newBullet.sprite.getGlobalBounds().width / 2,
                    player.getPosition().y
                );
                bullets.push_back(newBullet);
                fireClock.restart();
            }
            // Update Bullets
            for (auto& b : bullets) {
                b.update(dt);
                b.draw(gameWindow);
            }




            Texture enemyTexture;
            if(!enemyTexture.loadFromFile("assets/enemy1.png"))
            {
                cout << "Error loading enemy texture!\n";
            }
            // Example of random spawn
            if (spawnClock.getElapsedTime().asSeconds() > spawnInterval) {
                Enemy1 newEnemy(enemyTexture, 0.3f, 150.0f);
                newEnemy.setPosition(rand() % desktop.width, 0);
                enemies.push_back(newEnemy);
                spawnClock.restart();
            }

            // Update & draw all
            for (auto& e : enemies) {
                e.update(dt);
                e.draw(gameWindow);
            }


            // Erase all Bullets
            for (int i = 0; i < bullets.size(); i++) {
                if (bullets[i].isOffScreen(desktop.height)) {
                    bullets.erase(bullets.begin() + i);
                    i--;
                }
            }
                 float  delay=0.4f;
            if (soundClock.getElapsedTime().asSeconds() >= delay) {
            soundBullet.play();
            soundClock.restart(); // reset timer for next interval
        }

        // optional small sleep to prevent 100% CPU usage
        sf::sleep(sf::milliseconds(5));
            gameWindow.draw(player);
              
            gameWindow.display();
        }
    }
}