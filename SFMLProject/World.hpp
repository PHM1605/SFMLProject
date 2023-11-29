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
#include "ParticleNode.hpp"
#include "PostEffect.hpp"
#include "BloomEffect.hpp"
#include "SoundPlayer.hpp"

class World : private sf::NonCopyable {
public:
	World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds);
	void update(sf::Time dt);
	void draw();
	CommandQueue& getCommandQueue();
	bool hasAlivePlayer() const;
	bool hasPlayerReachEnd() const;

private:
	enum Layer { Background, LowerAir, UpperAir, LayerCount };
	struct SpawnPoint {
		Aircraft::Type type;
		float x;
		float y;
		SpawnPoint(Aircraft::Type type, float x, float y) :
			type(type), x(x), y(y) {}
	};
	sf::RenderTarget& mTarget;
	sf::RenderTexture mSceneTexture; // scene buffer - for post effect
	sf::View mWorldView;
	TextureHolder mTextures;
	FontHolder& mFonts;
	SoundPlayer& mSounds;
	SceneNode mSceneGraph;
	std::array<SceneNode*, LayerCount> mSceneLayers;
	CommandQueue mCommandQueue;
	sf::FloatRect mWorldBounds;
	sf::Vector2f mSpawnPosition;
	float mScrollSpeed;
	Aircraft* mPlayerAircraft;
	std::vector<SpawnPoint> mEnemySpawnPoints;
	std::vector<Aircraft*> mActiveEnemies;
	BloomEffect mBloomEffect;
	
private:
	void loadTextures();
	void adaptPlayerPosition(); // when player moves out of screen
	void adaptPlayerVelocity();
	void handleCollisions();
	void updateSounds();
	void buildScene();
	void addEnemies();
	void addEnemy(Aircraft::Type type, float relX, float relY);
	void spawnEnemies();
	void destroyEntitiesOutsideView();
	void guideMissiles();
	sf::FloatRect getViewBounds() const;
	sf::FloatRect getBattlefieldBounds() const;

};