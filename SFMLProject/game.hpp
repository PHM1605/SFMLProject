#ifndef GAME_HPP
#define GAME_HPP

#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceHolder.hpp"
#include "World.hpp"

class Game: private sf::NonCopyable {
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
	static const float playerSpeed; 
	ResourceHolder<sf::Font, Fonts::ID> fonts;
	sf::Text mStatisticsText;
	sf::Time mStatisticsUpdateTime;
	std::size_t mStatisticsNumFrames;
	static const sf::Time timePerFrame;
};

#endif