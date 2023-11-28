#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceIdentifiers.hpp"
#include "ResourceHolder.hpp"
#include "StateStack.hpp"
#include "MusicPlayer.hpp"
#include "SoundPlayer.hpp"

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
	FontHolder mFonts;
	Player mPlayer;
	MusicPlayer mMusic;
	SoundPlayer mSounds;
	StateStack mStateStack;
	sf::Text mStatisticsText;
	sf::Time mStatisticsUpdateTime;
	std::size_t mStatisticsNumFrames;
};
