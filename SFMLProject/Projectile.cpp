#include "Projectile.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include <cassert>

namespace {
	const std::vector<ProjectileData> Table = initializeProjectileData();
}

Projectile::Projectile(Type type, const TextureHolder& textures) :
	Entity(1),
	mType(type),
	mSprite(textures.get(Table[type].texture)), 
	mTargetDirection()
{
	centerOrigin(mSprite);
}

void Projectile::guideTowards(sf::Vector2f position) {
	assert(isGuided());
	mTargetDirection = unitVector(position - getWorldPosition());
}

bool Projectile::isGuided() const {
	return mType == Missile;
}

unsign

float Projectile::getMaxSpeed() const {
	return Table[mType].speed;
}

float Projectile::getDamage() const {
	return Table[mType].damage;
}