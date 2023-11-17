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
	void processInput();
	void update(sf::Time elapsedTime);
	void render();
	void updateStatistics(sf::Time elapsedTime);
private:
	sf::RenderWindow mWindow;
	World mWorld;
	Player mPlayer;
	static const float playerSpeed; 
	ResourceHolder<sf::Font, Fonts::ID> fonts;
	sf::Text mStatisticsText;
	sf::Time mStatisticsUpdateTime;
	std::size_t mStatisticsNumFrames;
	static const sf::Time timePerFrame;
	bool mIsPaused;
};

#endif