#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceHolder.hpp"
#include "SceneNode.hpp"
#include "Aircraft.hpp"
#include <array>
#include "SpriteNode.hpp"
#include "CommandQueue.hpp"
#include "Player.hpp"

class World : private sf::NonCopyable {
public:
	explicit World(sf::RenderWindow& window);
	void update(sf::Time dt);
	void draw();
	CommandQueue& getCommandQueue();
private:
	enum Layer { Background, Air, LayerCount };
	sf::RenderWindow& mWindow;
	sf::View mWorldView;
	TextureHolder mTextures;
	SceneNode mSceneGraph;
	std::array<SceneNode*, LayerCount> mSceneLayers;
	sf::FloatRect mWorldBounds;
	sf::Vector2f mSpawnPosition;
	float mScrollSpeed;
	Aircraft* mPlayerAircraft;
	CommandQueue mCommandQueue;
	
private:
	void loadTextures();
	void buildScene();
	void adaptPlayerVelocity();
	void adaptPlayerPosition(); // when player moves out of screen
};