#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
using namespace std;
int main()
{
    VideoMode desktop = VideoMode::getDesktopMode();
    RenderWindow menuWindow(desktop, "Main Menu", Style::Default);

    Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        return -1;

    RectangleShape button(Vector2f(200, 60));
    int centerw = desktop.width / 2 - 100;
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
                }
            }
        }
        menuWindow.clear(Color::Black);
        menuWindow.draw(button);
        menuWindow.draw(buttonText);
        menuWindow.display();
    }

    if (gameStarted)
    {
        RenderWindow gameWindow(desktop, "Space Shooter", Style::Default);
        Texture shipTexture;
        if (!shipTexture.loadFromFile("assets/spaceship.png"))
            return -1;

        Sprite player(shipTexture);
        player.setScale(0.2f, 0.2f);
        player.setPosition(desktop.width - centerw - player.getGlobalBounds().width, desktop.height - (centerh / 2));

        Texture bullet;
        if (!bullet.loadFromFile("assets/bullet.png"))
        {
            return -1;
        }
        Sprite bullet_asset(bullet);
        bullet_asset.setScale(0.055f, 0.055f);
        bullet_asset.setPosition(player.getPosition().x + (player.getGlobalBounds().width / 2) - (bullet_asset.getGlobalBounds().width / 2), player.getPosition().y);
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
                bullet_asset.move(-speed * dt, 0);
            }

            if (Keyboard::isKeyPressed(Keyboard::Up))
            {
                player.move(0, -speed * dt);
                bullet_asset.move(0, -speed * dt);
            }
            if (Keyboard::isKeyPressed(Keyboard::Down))
            {
                player.move(0, speed * dt);
                bullet_asset.move(0, speed * dt);
            }
            // Right movement
            if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                player.move(speed * dt, 0);
                bullet_asset.move(speed * dt, 0);
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
                player.setPosition(desktop.width - player.getGlobalBounds().width, player.getPosition().y);

            
            gameWindow.clear(Color::Black);
            float bulletSpeed = 400.0f;

            if (bullet_asset.getPosition().y >= 0)
                bullet_asset.move(0, -dt*speed);

            gameWindow.draw(bullet_asset);
            gameWindow.draw(player);
            gameWindow.display();
        }
    }

    return 0;
}
