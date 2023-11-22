#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "Aircraft.hpp"
#include <array>
#include <algorithm>
#include "SpriteNode.hpp"
#include "CommandQueue.hpp"
#include "Player.hpp"

class World : private sf::NonCopyable {
public:
	World(sf::RenderWindow& window, FontHolder& fonts);
	void update(sf::Time dt);
	void draw();
	CommandQueue& getCommandQueue();

private:
	enum Layer { Background, Air, LayerCount };
	struct SpawnPoint {
		Aircraft::Type type;
		float x;
		float y;
		SpawnPoint(Aircraft::Type type, float x, float y) :
			type(type), x(x), y(y) {}
	};
	sf::RenderWindow& mWindow;
	sf::View mWorldView;
	TextureHolder mTextures;
	FontHolder& mFonts;
	SceneNode mSceneGraph;
	std::array<SceneNode*, LayerCount> mSceneLayers;
	CommandQueue mCommandQueue;
	sf::FloatRect mWorldBounds;
	sf::Vector2f mSpawnPosition;
	float mScrollSpeed;
	Aircraft* mPlayerAircraft;
	std::vector<SpawnPoint> mEnemySpawnPoints;
	std::vector<Aircraft*> mActiveEnemies;
	
private:
	void loadTextures();
	void adaptPlayerVelocity();
	void adaptPlayerPosition(); // when player moves out of screen
	void buildScene();
	void addEnemies();
	void addEnemy(Aircraft::Type type, float relX, float relY);
	void spawnEnemies();
	void destroyEntitiesOutsideView();
	sf::FloatRect getViewBounds() const;
	sf::FloatRect getBattlefieldBounds() const;

};