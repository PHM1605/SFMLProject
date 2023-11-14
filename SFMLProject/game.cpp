#include "game.hpp"
#include "StringHelpers.hpp"
#include <iostream>

const float Game::playerSpeed = 100.f;
const sf::Time Game::timePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: mWindow(sf::VideoMode(640, 480), "SFML Application"),
	mTexture(),
	mPlayer(),
	mIsMovingDown(false),
	mIsMovingLeft(false),
	mIsMovingUp(false),
	mIsMovingRight(false),
	mFont(),
	mStatisticsText(),
	mStatisticsUpdateTime(),
	mStatisticsNumFrames(0)
{
	if (!mTexture.loadFromFile("Media/Textures/Eagle.png")) {
	}
	mPlayer.setTexture(mTexture);
	mPlayer.setPosition(100.f, 100.f);
	mFont.loadFromFile("Media/Sansation.ttf");
	mStatisticsText.setFont(mFont);
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
	if (key == sf::Keyboard::W) {
		mIsMovingUp = isPressed;
	}
	else if (key == sf::Keyboard::S) {
		mIsMovingDown = isPressed;
	}
	else if (key == sf::Keyboard::A) {
		mIsMovingLeft = isPressed;
	}
	else if (key == sf::Keyboard::D) {
		mIsMovingRight = isPressed;
	}
}

void Game::update(sf::Time deltaTime) {
	sf::Vector2f movement(0.f, 0.f);
	if (mIsMovingUp)
		movement.y -= playerSpeed;
	if (mIsMovingDown)
		movement.y += playerSpeed;
	if (mIsMovingLeft)
		movement.x -= playerSpeed;
	if (mIsMovingRight)
		movement.x += playerSpeed;
	mPlayer.move(movement * deltaTime.asSeconds());
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
	mWindow.draw(mPlayer);
	mWindow.display();
}