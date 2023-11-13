#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

class Game {
public: 
	Game();
	void run();
private:
	void processEvents();
	void update(sf::Time elapsedTime);
	void render();
	void updateStatistics(sf::Time elapsedTime);
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
private:
	sf::RenderWindow mWindow;
	bool mIsMovingUp, mIsMovingDown, mIsMovingLeft, mIsMovingRight;
	static const float playerSpeed; 
	static const sf::Time timePerFrame;
	sf::Texture mTexture;
	sf::Sprite mPlayer;
	sf::Font mFont;
	sf::Text mStatisticsText;
	sf::Time mStatisticsUpdateTime;
	std::size_t mStatisticsNumFrames;
};

#endif