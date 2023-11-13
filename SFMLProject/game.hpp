#include <SFML/Graphics.hpp>

class Game {
public: 
	Game();
	void run();
private:
	void processEvents();
	void update(sf::Time deltaTime);
	void render();
private:
	sf::RenderWindow mWindow;
	sf::CircleShape mPlayer;
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
	bool mIsMovingUp, mIsMovingDown, mIsMovingLeft, mIsMovingRight;
	float playerSpeed; 
};