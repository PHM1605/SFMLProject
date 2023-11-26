#pragma once
#include "Entity.hpp"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "DataTables.hpp"
#include "TextNode.hpp"
#include "CommandQueue.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"

class Aircraft : public Entity {
public:
	enum Type { Eagle, Raptor, Avenger, TypeCount};
public: 
	Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts);
	virtual unsigned int getCategory() const;
	virtual sf::FloatRect getBoundingRect() const;
	virtual void remove();
	virtual bool isMarkedForRemoval() const;
	bool isAllied() const;
	float getMaxSpeed() const;
	void increaseFireRate();
	void increaseSpread();
	void collectMissiles(unsigned int count);
	void fire();
	void launchMissile();
	
private:
	Type mType;
	sf::Sprite mSprite;
	// Animation mExplosion;
	Command mFireCommand;
	Command mMissileCommand;
	sf::Time mFireCountdown;
	bool mIsFiring;
	bool mIsLaunchingMissile;
	bool mShowExplosion;
	bool mSpawnedPickup;
	int mFireRateLevel;
	int mSpreadLevel;
	int mMissileAmmo;
	Command mDropPickupCommand;
	float mTravelledDistance;
	std::size_t mDirectionIndex;
	TextNode* mHealthDisplay;
	TextNode* mMissileDisplay;
private:
	virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
	void updateMovementPattern(sf::Time dt);
	void checkPickupDrop(CommandQueue& commands);
	void checkProjectileLaunch(sf::Time dt, CommandQueue& commands);
	void createBullets(SceneNode& node, const TextureHolder& textures) const;
	void createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;
	void createPickup(SceneNode& node, const TextureHolder& textures) const;
	void updateTexts();
};