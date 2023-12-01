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
#include "NetworkNode.hpp"

class World : private sf::NonCopyable {
public:
	World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds, bool networked = false);
	void update(sf::Time dt);
	void draw();
	sf::FloatRect getViewBounds() const;
	CommandQueue& getCommandQueue();
	Aircraft* addAircraft(int identifier);
	void removeAircraft(int identifier);
	void setCurrentBattleFieldPosition(float lineY);
	void setWorldHeight(float height);
	void addEnemy(Aircraft::Type type, float relX, float relY);
	void sortEnemies();

	bool hasAlivePlayer() const;
	bool hasPlayerReachedEnd() const;

	void setWorldScrollCompensation(float compensation); // for lagging
	Aircraft* getAircraft(int identifier) const;
	sf::FloatRect getBattlefieldBounds() const;

	void createPickup(sf::Vector2f position, Pickup::Type type);
	bool pollGameAction(GameActions::Action& out);

private:
	void loadTextures();
	void adaptPlayerPosition(); // when player moves out of screen
	void adaptPlayerVelocity();
	void handleCollisions();
	void updateSounds();
	void buildScene();
	void addEnemies();

	void spawnEnemies();
	void destroyEntitiesOutsideView();
	void guideMissiles();

private:
	enum Layer { Background, LowerAir, UpperAir, LayerCount };
	struct SpawnPoint {
		Aircraft::Type type;
		float x;
		float y;
		SpawnPoint(Aircraft::Type type, float x, float y) :
			type(type), x(x), y(y) {}
	};
private:
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
	float mScrollSpeedCompensation;
	std::vector<Aircraft*> mPlayerAircrafts;
	std::vector<SpawnPoint> mEnemySpawnPoints;
	std::vector<Aircraft*> mActiveEnemies;
	BloomEffect mBloomEffect;
	bool mNetworkedWorld;
	NetworkNode* mNetworkNode;
	SpriteNode* mFinishSprite;
};