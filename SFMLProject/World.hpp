#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceHolder.hpp"
#include "SceneNode.hpp"
#include "Aircraft.hpp"
#include <array>
#include "SpriteNode.hpp"

class World : private sf::NonCopyable {
public:
	explicit World(sf::RenderWindow& window);
	void update(sf::Time dt);
	void draw();
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

private:
	void loadTextures();
	void buildScene();
};