#include "World.hpp"

World::World(sf::RenderWindow& window) :
	mWindow(window),
	mWorldView(window.getDefaultView()),
	mScrollSpeed(-50.f),
	mWorldBounds(0.f, 0.f, mWorldView.getSize().x, 2000.f),
	mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldBounds.height - mWorldView.getSize().y/2.f),
	mPlayerAircraft(nullptr)
{
	loadTextures();
	buildScene();
	mWorldView.setCenter(mSpawnPosition);
}

void World::loadTextures() {
	mTextures.load(Textures::Eagle, "Media/Textures/Eagle.png");
	mTextures.load(Textures::Raptor, "Media/Textures/Raptor.png");
	mTextures.load(Textures::Desert, "Media/Textures/Desert.png");
}

void World::buildScene() {
	for (std::size_t i = 0; i < LayerCount; i++) {
		SceneNode::Ptr layer(new SceneNode());
		mSceneLayers[i] = layer.get();
		mSceneGraph.attachChild(std::move(layer));
	}
	// Prepare the tiled background
	sf::Texture& texture = mTextures.get(Textures::Desert);
	texture.setRepeated(true);
	sf::IntRect textureRect(mWorldBounds);
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
	backgroundSprite->setPosition(mWorldBounds.left, mWorldBounds.top);
	mSceneLayers[Background]->attachChild(std::move(backgroundSprite));

	// Add player's aircraft
	std::unique_ptr<Aircraft> leader(new Aircraft(Aircraft::Eagle, mTextures));
	mPlayerAircraft = leader.get();
	mPlayerAircraft->setPosition(mSpawnPosition);
	mPlayerAircraft->setVelocity(40.f, mScrollSpeed);
	mSceneLayers[Air]->attachChild(std::move(leader));

	// Adding two escorting aircrafts, placed relatively to the main plane
	std::unique_ptr<Aircraft> leftEscort(new Aircraft(Aircraft::Raptor, mTextures));
	leftEscort->setPosition(-80.f, 50.f);
	mPlayerAircraft->attachChild(std::move(leftEscort));

	std::unique_ptr<Aircraft>rightEscort(new Aircraft(Aircraft::Raptor, mTextures));
	rightEscort->setPosition(80.f, 50.f);
	mPlayerAircraft->attachChild(std::move(rightEscort));
}

void World::draw() {
	mWindow.setView(mWorldView);
	mWindow.draw(mSceneGraph);
}

void World::update(sf::Time dt) {
	mWorldView.move(0.f, mScrollSpeed * dt.asSeconds());
	mPlayerAircraft->setVelocity(0.f, 0.f);
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	mSceneGraph.update(dt);

	/*float mPlayerSpeed = 2.f * (-mScrollSpeed);
	sf::Vector2f movement(0.0f, 0.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		movement.y -= mPlayerSpeed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		movement.y += mPlayerSpeed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		movement.x -= mPlayerSpeed;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		movement.x += mPlayerSpeed;
	mPlayerAircraft->move(movement * dt.asSeconds());*/

	/*sf::Vector2f position = mPlayerAircraft->getPosition();
	sf::Vector2f velocity = mPlayerAircraft->getVelocity();
	if (position.x <= mWorldBounds.left + 150 || position.x >= mWorldBounds.left + mWorldBounds.width - 150) {
		velocity.x = -velocity.x;
		mPlayerAircraft->setVelocity(velocity);
	}
	mSceneGraph.update(dt);*/
}

CommandQueue& World::getCommandQueue() {
	return mCommandQueue;
}