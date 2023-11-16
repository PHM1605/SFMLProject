#ifndef GAME_HPP
#define GAME_HPP

#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceHolder.hpp"
#include "World.hpp"

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
	World mWorld;
	bool mIsMovingUp, mIsMovingDown, mIsMovingLeft, mIsMovingRight;
	static const float playerSpeed; 
	static const sf::Time timePerFrame;

	ResourceHolder<sf::Texture, Textures::ID> textures;
	sf::Sprite landscape;
	sf::Sprite airplane;

	ResourceHolder<sf::Font, Fonts::ID> fonts;

	sf::Text mStatisticsText;
	sf::Time mStatisticsUpdateTime;
	std::size_t mStatisticsNumFrames;
};

#endif