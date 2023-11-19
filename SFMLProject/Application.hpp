#ifndef GAME_HPP
#define GAME_HPP

#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceIdentifiers.hpp"
#include "ResourceHolder.hpp"
#include "StateStack.hpp"

class Application {
public: 
	Application();
	void run();
private:
	void processInput();
	void update(sf::Time dt);
	void render();
	void updateStatistics(sf::Time dt);
	void registerStates();
private:
	static const sf::Time timePerFrame;
	sf::RenderWindow mWindow;
	TextureHolder mTextures;
	FontsHolder mFonts;
	Player mPlayer;
	StateStack mStateStack;
	sf::Text mStatisticsText;
	sf::Time mStatisticsUpdateTime;
	std::size_t mStatisticsNumFrames;
};
