#include "World.hpp"

World::World(sf::RenderWindow& window, FontHolder& fonts):
	mWindow(window),
	mWorldView(window.getDefaultView()),
	mFonts(fonts),
	mTextures(),
	mSceneGraph(),
	mSceneLayers(),
	mWorldBounds(0.f, 0.f, mWorldView.getSize().x, 2000.f),
	mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldBounds.height - mWorldView.getSize().y/2.f),
	mScrollSpeed(-50.f),
	mPlayerAircraft(nullptr),
	mEnemySpawnPoints(),
	mActiveEnemies()
{
	loadTextures();
	buildScene();
	mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt) {
	sf::Vector2f topLeft = mWorldView.getCenter() - mWorldView.getSize() / 2.f;
	if (topLeft.y <= 0)
		mScrollSpeed = 0;
	mWorldView.move(0.f, mScrollSpeed * dt.asSeconds());
	mPlayerAircraft->setVelocity(0.f, 0.f);
	destroyEntitiesOutsideView();
	guideMissiles();
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();
	spawnEnemies();
	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();
}

void World::draw() {
	mWindow.setView(mWorldView);
	mWindow.draw(mSceneGraph);
}

CommandQueue& World::getCommandQueue() {
	return mCommandQueue;
}

bool World::hasAlivePlayer() const {
	return !mPlayerAircraft->isMarkedForRemoval();
}

bool World::hasPlayerReachEnd() const {
	return !mWorldBounds.contains(mPlayerAircraft->getPosition());
}

void World::loadTextures() {
	mTextures.load(Textures::Eagle, "Media/Textures/Eagle.png");
	mTextures.load(Textures::Raptor, "Media/Textures/Raptor.png");
	mTextures.load(Textures::Avenger, "Media/Textures/Avenger.png");
	mTextures.load(Textures::Desert, "Media/Textures/Desert.png");
	mTextures.load(Textures::Bullet, "Media/Textures/Bullet.png");
	mTextures.load(Textures::Missile, "Media/Textures/Missile.png");
}

void World::buildScene() {
	for (std::size_t i = 0; i < LayerCount; i++) {
		SceneNode::Ptr layer(new SceneNode());
		mSceneLayers[i] = layer.get();
		mSceneGraph.attachChild(std::move(layer));
	}
	// Prepare the tiled background
	sf::Texture& texture = mTextures.get(Textures::Desert);
	sf::IntRect textureRect(mWorldBounds);
	texture.setRepeated(true);
	
	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
	backgroundSprite->setPosition(mWorldBounds.left, mWorldBounds.top);
	mSceneLayers[Background]->attachChild(std::move(backgroundSprite));

	// Add player's aircraft
	std::unique_ptr<Aircraft> leader(new Aircraft(Aircraft::Eagle, mTextures, mFonts));
	mPlayerAircraft = leader.get();
	mPlayerAircraft->setPosition(mSpawnPosition);
	mPlayerAircraft->setVelocity(40.f, mScrollSpeed);
	mSceneLayers[Air]->attachChild(std::move(leader));

	addEnemies();
}

void World::adaptPlayerVelocity() {
	sf::Vector2f velocity = mPlayerAircraft->getVelocity();
	// when press two keys -> move diagonal is sqrt(speed^2 + speed^2)=sqrt(2)*speed -> should be reduced by sqrt(2)
	if (velocity.x != 0.f && velocity.y != 0.f)
		mPlayerAircraft->setVelocity(velocity / std::sqrt(2.f));
	mPlayerAircraft->accelerate(0.f, mScrollSpeed);
}

void World::adaptPlayerPosition() {
	const float borderDistance = 40.f;
	sf::FloatRect viewBounds(
		mWorldView.getCenter() - mWorldView.getSize() / 2.f,
		mWorldView.getSize()
		);
	sf::Vector2f position = mPlayerAircraft->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	mPlayerAircraft->setPosition(position);
}

void World::addEnemies() {
	addEnemy(Aircraft::Raptor, 0.f, 500.f);
	addEnemy(Aircraft::Raptor, 0.f, 1000.f);
	addEnemy(Aircraft::Raptor, +100.f, 1100.f);
	addEnemy(Aircraft::Raptor, -100.f, 1100.f);
	addEnemy(Aircraft::Avenger, -70.f, 1400.f);
	addEnemy(Aircraft::Avenger, -70.f, 1600.f);
	addEnemy(Aircraft::Avenger, 70.f, 1400.f);
	addEnemy(Aircraft::Avenger, 70.f, 1600.f);
	// Sort all enemies according to their y values, such that lower enemies are checked first for spawning
	std::sort(mEnemySpawnPoints.begin(), mEnemySpawnPoints.end(), [](SpawnPoint lhs, SpawnPoint rhs) {
		return lhs.y < rhs.y;
		});
}

void World::addEnemy(Aircraft::Type type, float relX, float relY) {
	SpawnPoint spawn(type, mSpawnPosition.x + relX, mSpawnPosition.y - relY);
	mEnemySpawnPoints.push_back(spawn);
}

void World::spawnEnemies() {
	while (!mEnemySpawnPoints.empty() && mEnemySpawnPoints.back().y > getBattlefieldBounds().top) {
		SpawnPoint spawn = mEnemySpawnPoints.back();
		std::unique_ptr<Aircraft> enemy(new Aircraft(spawn.type, mTextures, mFonts));
		enemy->setPosition(spawn.x, spawn.y);
		enemy->setRotation(180.f);
		mSceneLayers[Air]->attachChild(std::move(enemy));
		mEnemySpawnPoints.pop_back();
	}
}

void World::destroyEntitiesOutsideView() {
	Command command;
	command.category = Category::Projectile | Category::EnemyAircraft;
	command.action = derivedAction<Entity>([this](Entity& e, sf::Time) {
		if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
			e.destroy();
		});
	mCommandQueue.push(command);
}

void World::guideMissiles() {
	// Collect an array to enemies
	Command enemyCollector;
	enemyCollector.category = Category::EnemyAircraft;
	enemyCollector.action = derivedAction<Aircraft>([this](Aircraft& enemy, sf::Time) {
		if (!enemy.isDestroyed())
			mActiveEnemies.push_back(&enemy);
		});

	// Guild missiles
	Command missileGuider;
	missileGuider.category = Category::AlliedProjectile;
	missileGuider.action = derivedAction<Projectile>([this](Projectile& missile, sf::Time) {
		// Ignore unguided bullets
		if (!missile.isGuided())
			return;
		float minDistance = std::numeric_limits<float>::max();
		Aircraft* closestEnemy = nullptr;
		// Find closes enemy
		for (Aircraft* enemy : mActiveEnemies) {
			float enemyDistance = distance(missile, *enemy);
			if (enemyDistance < minDistance) {
				closestEnemy = enemy;
				minDistance = enemyDistance;
			}
		}
		if (closestEnemy)
			missile.guideTowards(closestEnemy->getWorldPosition());
	});
	mCommandQueue.push(enemyCollector);
	mCommandQueue.push(missileGuider);
	mActiveEnemies.clear();
}

sf::FloatRect World::getViewBounds() const {
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const {
	// Return view bounds + some area on top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;
	return bounds;
}