#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <thread> //for delay in  sounds
#include <chrono>
using namespace sf;
using namespace std;
// Data
// struct for buttons

struct Button
{
    RectangleShape shape;
    Text text;

    Button(Font &font, string label, Vector2f size, Vector2f position, Color color = Color::Black)
    {
        shape.setSize(size);
        shape.setFillColor(color);
        shape.setOutlineThickness(3);
        shape.setOutlineColor(Color::White);
        shape.setPosition(position);

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(28);
        text.setFillColor(Color::White);

        // Center text in the button
        FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
                       textRect.top + textRect.height / 2.0f);
        text.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
    }

    void draw(RenderWindow &window)
    {
        window.draw(shape);
        window.draw(text);
    }

    bool isHovered(Vector2f mousePos)
    {
        return shape.getGlobalBounds().contains(mousePos);
    }
};

// Enemy structure
struct Enemy1
{
    Texture texture;
    Sprite sprite;
    float speed;
    bool collision = false;

    // Loader of the enemy
    Enemy1(Texture &texture, float scale = 0.3f, float speedValue = 100.0f)
    {
        sprite.setTexture(texture);
        sprite.setScale(scale, scale);
        sprite.setColor(Color(150, 150, 255));
        speed = speedValue;
    }

    // Set initial position
    void setPosition(float x, float y)
    {
        sprite.setPosition(x, y);
    }

    // Move enemy downward
    void update(float dt)
    {
        sprite.move(0, speed * dt);
    }

    // Draw enemy on screen
    void draw(RenderWindow &window)
    {
        window.draw(sprite);
    }
    // To check if the bullet is offscreen
    bool isOffScreen(int screenHeight)
    {
        return (sprite.getPosition().y + sprite.getGlobalBounds().height < 0) || collision;
    }
    // For setting collision
    void setCollision(){
        collision=true;
    }
    // gets bounds of sprite /its postion
    FloatRect getBounds() const
    {
        return sprite.getGlobalBounds();
    }
};

// Bullet structure
struct Bullet
{
    Texture texture;
    Sprite sprite;
    float speed;

    // Loader of the bullet
    Bullet(Texture &texture, float speedValue = 400.0f)
    {
        sprite.setTexture(texture);
        sprite.setScale(0.055f, 0.055f);
        speed = speedValue;
    }

    // Set initial position
    void setPosition(float x, float y)
    {
        sprite.setPosition(x, y);
    }

    // Move bullet upward
    void update(float dt)
    {
        sprite.move(0, -speed * dt);
    }

    // Draw bullet on screen
    void draw(RenderWindow &window)
    {
        window.draw(sprite);
    }
    bool isOffScreen(int screenHeight)
    {
        return sprite.getPosition().y + sprite.getGlobalBounds().height < 0;
    }
    // getboundds of bullets (Abdullah bhai)
    FloatRect getBounds() const
    {
        return sprite.getGlobalBounds();
    }
};
//Explosion Struct
struct Explosion
{
    Texture texture;
    Sprite sprite;
    float lifetime = 0.3f;
    Explosion(Texture &texture, Vector2f position, float scale = 0.08f)
    {
        sprite.setTexture(texture);
        sprite.setScale(scale, scale);
        sprite.setPosition(position);
    }
    bool isExpired(float dt)
    {
        lifetime -= dt;
        return (lifetime <= 0);
    }
};

// Clock for enemies spawn
Clock spawnClock;           // used to track time since last enemy spawn
float spawnInterval = 2.0f; // spawn a new enemy every 2 seconds initially
Clock spawnClock1;
Clock fireClock;
float fireInterval = 0.25f;
Clock gameClock;
Clock soundClock;
// Global Enemy List
vector<Enemy1> enemies;
// Global Enemy2 List
vector<Enemy1> enemies1;
// Global Bullet List
vector<Bullet> bullets;
// Global Explosion List
vector<Explosion> explosions;
// Score variable
int score = 0;
// Enemies going past variable
int past=0;
// High Score variable
int highScore = 0;
// Function Declarations
int Mainmenu(bool gamestarted);
int game(bool gameStarted);
void checkCollisions(vector<Bullet> &bullets,
                     vector<Enemy1> &enemies,
                     Sprite &player,
                     bool &gameOver,
                     Texture &explosionTexture, VideoMode desktop = VideoMode::getDesktopMode());
void High_score();
int main()
{
    srand(time(0));
    //  Game Start Indicator
    High_score();
    int from_main_menu = 0;
    bool gameStarted = false;
    from_main_menu = Mainmenu(gameStarted);
    if (from_main_menu == 1)
    {
        // if game load error occurs
        gameStarted = true;
        int error = game(gameStarted);
        if (error == -1)
        {
            return -1;
        }
    }
    else if (from_main_menu == 2)
    {
        cout << "";
    }
    if(score>highScore){
        highScore=score;
        ofstream fout;
        fout.open("assets/highScore.txt");
        fout<<highScore;
        fout.close();
    }
    return 0;
}
// implement krdana
int Mainmenu(bool gamestarted)
{
    VideoMode desktop = VideoMode::getDesktopMode();
    RenderWindow menuWindow(desktop, "Main Menu", Style::Default);

    Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        cout << "Font load error\n";
        return -1;
    }

    // Button dimensions
    Vector2f buttonSize(250, 70);
    float startY = desktop.height / 2 - 150; // top button start
    float centerX = desktop.width / 2 - buttonSize.x / 2;

    // Create buttons
    Button playButton(font, "PLAY GAME", buttonSize, {centerX, startY}, Color(30, 30, 30));
    Button highScoreButton(font, "HIGH SCORES", buttonSize, {centerX, startY + 100}, Color(30, 30, 30));
    Button exitButton(font, "EXIT", buttonSize, {centerX, startY + 200}, Color(30, 30, 30));

    bool gameStarted = false;

    while (menuWindow.isOpen())
    {
        Event event;
        while (menuWindow.pollEvent(event))
        {
            if (event.type == Event::Closed)
                menuWindow.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                Vector2f mousePos = menuWindow.mapPixelToCoords(Mouse::getPosition(menuWindow));

                if (playButton.isHovered(mousePos))
                {
                    menuWindow.close();
                    return 1;
                }
                if (highScoreButton.isHovered(mousePos))
                {
                    cout << "High Scores clicked!\n"; // You can add your logic later
                }
                if (exitButton.isHovered(mousePos))
                {
                    menuWindow.close();
                    return 0;
                }
            }
        }

        // Hover effect
        Vector2f mousePos = menuWindow.mapPixelToCoords(Mouse::getPosition(menuWindow));
        for (Button *btn : {&playButton, &highScoreButton, &exitButton})
        {
            if (btn->isHovered(mousePos))
                btn->shape.setFillColor(Color(70, 0, 0)); // retro red hover
            else
                btn->shape.setFillColor(Color(30, 30, 30)); // dark grey normal
        }

        menuWindow.clear(Color(10, 10, 10)); // retro black background
        playButton.draw(menuWindow);
        highScoreButton.draw(menuWindow);
        exitButton.draw(menuWindow);
        menuWindow.display();
    }
}
// Game Logic
int game(bool gameStarted)

{

    // loading sound from file (BUllets)
    SoundBuffer bufferBullet;
    if (!bufferBullet.loadFromFile("assets/shoot.wav"))
    {
        cout << "Error: Could not load sound.wav\n";
        return -1;
    }

    SoundBuffer bufferExplosion;
    if(!bufferExplosion.loadFromFile("assets/invaderkilled.wav")){
        cout << "Error: Could not load sound.wav\n";
        return -1;
    }
    Sound soundBullet;
    soundBullet.setBuffer(bufferBullet);

    Sound soundExplosion;
    soundExplosion.setBuffer(bufferExplosion);

    // setting volume and pitch
    soundBullet.setVolume(30);
    soundBullet.setPitch(0.6f);
    soundExplosion.setVolume(60);
    soundExplosion.setPitch(0.8f);

    VideoMode desktop = VideoMode::getDesktopMode();
    int centerw = desktop.width / 2;
    int centerh = desktop.height / 2 - 30;
    if (gameStarted)
    {
        // Haider's contribution
        Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("assets/game_background.png"))
        {
            cout << "Error loading background image!\n";
        }
        Sprite backgroundSprite;
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            (float)desktop.width / backgroundTexture.getSize().x,
            (float)desktop.height / backgroundTexture.getSize().y);
        backgroundSprite.setColor(Color(255, 255, 255, 70));
        RenderWindow gameWindow(desktop, "Space Shooter", Style::Default);
        Texture shipTexture;
        if (!shipTexture.loadFromFile("assets/spaceship.png"))
        {
            return -1;
        }

        Sprite player(shipTexture);
        player.setScale(0.2f, 0.2f);
        player.setPosition(desktop.width - centerw - player.getGlobalBounds().width, desktop.height - (centerh / 2));
        player.setColor(Color(200,200,255));

        Texture bulletTexture;
        if (!bulletTexture.loadFromFile("assets/bullet.png"))
        {
            return -1;
        }

        Clock clock;

        while (gameWindow.isOpen())
        {
            float dt = clock.restart().asSeconds();
            float speed = 500.0f;

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
            gameWindow.draw(backgroundSprite);
            // Bullets Loading condition
            if (fireClock.getElapsedTime().asSeconds() > fireInterval)
            {
                Bullet newBullet(bulletTexture, 600.0f);
                newBullet.setPosition(
                    player.getPosition().x + player.getGlobalBounds().width / 2 - newBullet.sprite.getGlobalBounds().width / 2,
                    player.getPosition().y);
                bullets.push_back(newBullet);
                fireClock.restart();
            }
            // Update Bullets
            for (auto &b : bullets)
            {
                b.update(dt);
                b.draw(gameWindow);
            }
            // Enemy 1 Texture
            Texture enemyTexture;
            if (!enemyTexture.loadFromFile("assets/enemy1.png"))
            {
                cout << "Error loading enemy texture!\n";
            }
            //  Enemy 2 Texture
            Texture enemy2Texture;
            if(!enemy2Texture.loadFromFile("assets/enemy2.png")){
                cout << "Error loading enemy texture!\n";
            }
            // random spawn enemies
            if(spawnClock.getElapsedTime().asSeconds() > spawnInterval)
            {
                Enemy1 newEnemy(enemyTexture, 0.3f, 150.0f);
                int margin=250;
                newEnemy.setPosition(margin + rand() % (desktop.width-2*margin), 0);
                enemies.push_back(newEnemy);
                spawnClock.restart();
            }
            // random spawn enemies 2
            if(score>150){
                if(spawnClock1.getElapsedTime().asSeconds()> spawnInterval){
                Enemy1 newEnemy1(enemy2Texture, 0.4f, 200.0f);
                int margin=200;
                newEnemy1.setPosition(margin + rand() % (desktop.width-2*margin),0);
                enemies1.push_back(newEnemy1);
                spawnClock1.restart();
                }

                // Update & draw all new heavy enemies1
                for(auto &e: enemies1){
                    e.update(dt);
                    e.draw(gameWindow);
                }
            }
            

            // Update & draw all
            for(auto &e : enemies){
                e.update(dt);
                e.draw(gameWindow);
            }

            // Erase all Bullets
            for (int i = 0; i < bullets.size(); i++)
            {
                if (bullets[i].isOffScreen(desktop.height))
                {
                    bullets.erase(bullets.begin() + i);
                    i--;
                }
            }
            

            // Loading Explosion Texture
            Texture explosionTexture;
            if (!explosionTexture.loadFromFile("assets/explosion.png")) {
                cout << "Failed to load explosion.png\n";
            }
            // Collision Detection
            bool gameOver = false;
            checkCollisions(bullets, enemies, player, gameOver,explosionTexture);
            // Draw Explosions
            // --- Update explosions ---
            for (auto it = explosions.begin(); it != explosions.end();) {
                soundExplosion.play();
                if (it->isExpired(dt))
                    it = explosions.erase(it);
                else
                    ++it;
            }

            // --- Draw explosions ---
            for (auto &ex : explosions){
                gameWindow.draw(ex.sprite);
            }


            /// game over ka text
            Font gameOverFont;
            gameOverFont.loadFromFile("C:/Windows/Fonts/arial.ttf");
            Text gameOverText;
            gameOverText.setFont(gameOverFont);
            gameOverText.setString("GAME OVER\nPress Enter to Exit");
            gameOverText.setCharacterSize(100);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition((desktop.width / 2) - 250, (desktop.height / 2) - 180);

            // Displaying Score
            Text scoreText;
            scoreText.setFont(gameOverFont);
            scoreText.setCharacterSize(35);
            scoreText.setFillColor(Color::White);
            scoreText.setPosition(20, 20);
            scoreText.setString("Score: 0");

            // Chances Text
            Text chancesText;
            chancesText.setFont(gameOverFont);
            chancesText.setCharacterSize(35);
            chancesText.setFillColor(Color::White);
            chancesText.setPosition(desktop.width - 200, 20);
            chancesText.setString("Chances: " + to_string(3 - past));
            gameWindow.draw(chancesText);

            float delay = 0.4f;
            if (soundClock.getElapsedTime().asSeconds() >= delay)
            {
                soundBullet.play();
                soundClock.restart(); // reset timer for next interval
            }

            // optional small sleep to prevent 100% CPU usage
            sf::sleep(sf::milliseconds(5));
            gameWindow.draw(player);
            scoreText.setString("Score: " + to_string(score));
            gameWindow.draw(scoreText);
            gameWindow.display();
            if(past>=3){
                gameOver=true;
            }
            if (gameOver)
            {

                for (auto &enemy : enemies)
                {
                    enemy.speed = 0;
                }
                for (auto &bullet : bullets)
                {
                    bullet.speed = 0;
                }
                while (gameOver)
                {
                    Event e;
                    while (gameWindow.pollEvent(e))
                    {
                        if (e.type == Event::Closed)
                        {
                            return 0;
                            gameOver = false;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Enter))
                        {
                            return 0;
                            gameOver = false;
                        }
                    }
                    gameWindow.clear(Color::Black);
                    gameWindow.draw(backgroundSprite);
                    gameWindow.draw(gameOverText);
                    gameWindow.draw(scoreText);
                    gameWindow.draw(player);
                    gameWindow.display();
                }
                // gameWindow.close();
                //   gameWindow.close();
            }
        }
    }
}
void checkCollisions(vector<Bullet> &bullets,
                     vector<Enemy1> &enemies,
                     Sprite &player,
                     bool &gameOver,
                     Texture &explosionTexture, VideoMode desktop)
{
    
    // --- Bullet vs Enemy ---
    // trying collision
    for (auto b = bullets.begin(); b != bullets.end();)
    {
        bool collision = false;
        for (auto e = enemies.begin(); e != enemies.end();)
        {
            if (b->getBounds().intersects(e->getBounds()))
            {
                explosions.emplace_back(explosionTexture, e->sprite.getPosition());
                e = enemies.erase(e);
                b = bullets.erase(b);
                score += 10;
                collision = true;
                break;
            }
            else
            {
                ++e;
            }
        }
        if (!collision)
        {
            ++b;
        }
    }

    // --- Ship vs Enemy ---
    // --- Ship vs Screen bottom ---
    for (auto e = enemies.begin(); e != enemies.end();)
    {
        // Player collision with the enemu
        if (player.getGlobalBounds().intersects(e->getBounds()))
        {
            gameOver = true;
            break;
        }

        // If enemy goes offscreen
        if (e->sprite.getPosition().y > desktop.height)
        {
            past++;
            e = enemies.erase(e);
        }
        else
        {
            ++e;
        }
    }
}
void High_score(){
    ifstream fin;
    fin.open("highScore.txt");
    if(!fin.is_open()){
        fin>>highScore;
    }
    return ;
}