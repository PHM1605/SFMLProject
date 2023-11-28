#include "Aircraft.hpp"
#include <iostream>

namespace {
	const std::vector<AircraftData> Table = initializeAircraftData();
}

Aircraft::Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts) :
	Entity(Table[type].hitpoints),
	mType(type),
	mSprite(textures.get(Table[type].texture), Table[type].textureRect), // convert Aircraft::Type e.g. 'Eagle' enum to Textures::ID enum 
	mExplosion(textures.get(Textures::Explosion)),
	mFireCommand(),
	mMissileCommand(),
	mFireCountdown(sf::Time::Zero),
	mIsFiring(false),
	mIsLaunchingMissile(false),
	mShowExplosion(true),
	mSpawnedPickup(false),
	mFireRateLevel(0),
	mSpreadLevel(1),
	mMissileAmmo(2),
	mDropPickupCommand(),
	mTravelledDistance(0.f),
	mDirectionIndex(0),
	mHealthDisplay(nullptr),
	mMissileDisplay(nullptr)
{
	mExplosion.setFrameSize(sf::Vector2i(256, 256));
	mExplosion.setNumFrames(16);
	mExplosion.setDuration(sf::seconds(1));
	centerOrigin(mSprite);
	centerOrigin(mExplosion);

	//mFireCommand.category = Category::SceneAirLayer;
	if (mType == Type::Eagle)
		mFireCommand.category = Category::PlayerAircraft;
	else
		mFireCommand.category = Category::EnemyAircraft;
	mFireCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		createBullets(node, textures);
	};

	//mMissileCommand.category = Category::SceneAirLayer;
	mMissileCommand.category = Category::PlayerAircraft;
	mMissileCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		createProjectile(node, Projectile::Missile, 0.f, 0.5f, textures);
	};

	mDropPickupCommand.category = Category::SceneAirLayer;
	mDropPickupCommand.action = [this, &textures](SceneNode& node, sf::Time) {
		createPickup(node, textures);
	};

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));
	if (getCategory() == Category::PlayerAircraft) {
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}

	updateTexts();
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
	if (isDestroyed() && mShowExplosion)
		target.draw(mExplosion, states);
	else
		target.draw(mSprite, states);
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands) {
	updateTexts();
	updateRollAnimation();
	if (isDestroyed()) {
		checkPickupDrop(commands);
		mExplosion.update(dt);
		return;
	}
	// Check if bullets or missiles are fired
	checkProjectileLaunch(dt, commands);
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);
	
}

unsigned int Aircraft::getCategory() const {
	if (isAllied())
		return Category::PlayerAircraft;
	else
		return Category::EnemyAircraft;
}

sf::FloatRect Aircraft::getBoundingRect() const {
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Aircraft::isMarkedForRemoval() const {
	return isDestroyed() && (mExplosion.isFinished() || !mShowExplosion);
}

void Aircraft::remove() {
	Entity::remove();
	mShowExplosion = false;
}

bool Aircraft::isAllied() const {
	return mType == Eagle;
}

float Aircraft::getMaxSpeed() const {
	return Table[mType].speed;
}

void Aircraft::increaseFireRate() {
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}

void Aircraft::increaseSpread() {
	if (mSpreadLevel < 3)
		++mSpreadLevel;
}

void Aircraft::collectMissiles(unsigned int count) {
	mMissileAmmo += count;
}

void Aircraft::fire() {
	if (Table[mType].fireInterval != sf::Time::Zero)
		mIsFiring = true;
}

void Aircraft::launchMissile() {
	if (mMissileAmmo > 0) {
		mIsLaunchingMissile = true;
		--mMissileAmmo;
	}
}

void Aircraft::updateMovementPattern(sf::Time dt) {
	const std::vector<Direction>& directions = Table[mType].directions;
	if (!directions.empty()) {
		// Moved long enough in current direction: change direction
		if (mTravelledDistance > directions[mDirectionIndex].distance) {
			mDirectionIndex = (mDirectionIndex + 1) % directions.size();
			mTravelledDistance = 0.f;
		}
		float radians = toRadian(directions[mDirectionIndex].angle + 90.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);
		setVelocity(vx, vy);
		mTravelledDistance += getMaxSpeed() * dt.asSeconds();
	}
}

void Aircraft::checkPickupDrop(CommandQueue& commands) {
	
	if (!isAllied() && randomInt(3) == 0 && !mSpawnedPickup) {
		commands.push(mDropPickupCommand);
	}
	mSpawnedPickup = true;
}

void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue& commands) {
	if (!isAllied()) {
		fire();
	}
	if (mIsFiring && mFireCountdown <= sf::Time::Zero) {
		commands.push(mFireCommand);
		mFireCountdown += Table[mType].fireInterval / (mFireRateLevel + 1.f);
		mIsFiring = false;
	}
	else if (mFireCountdown > sf::Time::Zero) {
		mFireCountdown -= dt;
		mIsFiring = false;
	}
	if (mIsLaunchingMissile) {
		commands.push(mMissileCommand);
		mIsLaunchingMissile = false;
	}
}

void Aircraft::createBullets(SceneNode& node, const TextureHolder& textures) const {
	Projectile::Type type = isAllied() ? Projectile::AlliedBullet : Projectile::EnemyBullet;
	switch (mSpreadLevel) {
	case 1:
		createProjectile(node, type, 0.f, 0.5f, textures);
		break;
	case 2:
		createProjectile(node, type, -0.33f, 0.33f, textures);
		createProjectile(node, type, 0.33f, 0.33f, textures);
		break;
	case 3:
		createProjectile(node, type, -0.5f, 0.33f, textures);
		createProjectile(node, type, 0.f, 0.5f, textures);
		createProjectile(node, type, 0.5f, 0.33f, textures);
		break;
	}
}

void Aircraft::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const {
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
	sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
	sf::Vector2f velocity(0, projectile->getMaxSpeed());
	float sign = -1.f;
	projectile->setPosition(offset * sign);
	projectile->setVelocity(velocity * sign);
	node.attachChild(std::move(projectile));
}

void Aircraft::createPickup(SceneNode& node, const TextureHolder& textures) const {
	std::cout << "Create pickup!" << std::endl;
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));
	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(0.f, 0.f);
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));
}

void Aircraft::updateTexts() {
	mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation()); // set health text not rotating when plane rotates
	if (mMissileDisplay) {
		if (mMissileAmmo == 0)
			mMissileDisplay->setString("");
		else
			mMissileDisplay->setString("M: " + toString(mMissileAmmo));
	}
}

void Aircraft::updateRollAnimation() {
	if (Table[mType].hasRollAnimation) {
		sf::IntRect textureRect = Table[mType].textureRect;
		if (getVelocity().x < 0.f)
			textureRect.left += textureRect.width;
		else if (getVelocity().x > 0.f)
			textureRect.left += 2 * textureRect.width;
		mSprite.setTextureRect(textureRect);
	}
}