#include "game.hpp"
#include "StringHelpers.hpp"
#include <iostream>

const float Game::playerSpeed = 100.f;
const sf::Time Game::timePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: mWindow(sf::VideoMode(640, 480), "World", sf::Style::Close),
	mWorld(mWindow),
	mStatisticsNumFrames(0)
{
	fonts.load(Fonts::Sansation, "Media/Sansation.ttf");
	mStatisticsText.setFont(fonts.get(Fonts::Sansation));
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10);
}

void Game::run() {
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen()) {
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > timePerFrame) {
			timeSinceLastUpdate -= timePerFrame;
			processEvents();
			update(timePerFrame);
		}
		updateStatistics(elapsedTime);
		render();
	}
}
void Game::processEvents() {
	sf::Event event;
	while (mWindow.pollEvent(event)) {
		switch (event.type) {
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;
		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;
		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed) {
}

void Game::update(sf::Time elapsedTime) {
	mWorld.update(elapsedTime);
}

void Game::updateStatistics(sf::Time elapsedTime) {
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;
	if (mStatisticsUpdateTime >= sf::seconds(1.0f)) {
		mStatisticsText.setString(
			"Frames/Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");
		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}
}

void Game::render() {
	mWindow.clear();
	mWorld.draw();
	mWindow.setView(mWindow.getDefaultView());
	mWindow.draw(mStatisticsText);
	mWindow.display();
}