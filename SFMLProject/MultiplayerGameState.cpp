#include "MultiplayerGameState.hpp"

sf::IpAddress getAddressFromFile() {
	{ // Try to open existing file (RAII block)
		std::ifstream inputFile("ip.txt");
		std::string ipAddress;
		if (inputFile >> ipAddress)
			return ipAddress;
	}

	// If open/read failed, create new file
	std::ofstream outputFile("ip.txt");
	std::string localAddress = "127.0.0.1";
	outputFile << localAddress;
	return localAddress;
}

MultiplayerGameState::MultiplayerGameState(StateStack& stack, Context context, bool isHost) :
	State(stack, context),
	mWorld(*context.window, *context.fonts, *context.sounds, true),
	mWindow(*context.window),
	mTextureHolder(*context.textures),
	mConnected(false),
	mGameServer(nullptr),
	mActiveState(true),
	mHasFocus(true),
	mHost(isHost),
	mGameStarted(false),
	mClientTimeout(sf::seconds(2.f)),
	mTimeSinceLastPacket(sf::seconds(0.f))
{
	mBroadcastText.setFont(context.fonts->get(Fonts::Main));
	mBroadcastText.setPosition(1024.f / 2, 100.f);

	mPlayerInvitationText.setFont(context.fonts->get(Fonts::Main));
	mPlayerInvitationText.setCharacterSize(20);
	mPlayerInvitationText.setColor(sf::Color::White);
	mPlayerInvitationText.setString("Press Enter to spawn player 2");
	mPlayerInvitationText.setPosition(1000 - mPlayerInvitationText.getLocalBounds().width, 760 - mPlayerInvitationText.getLocalBounds().height);

	mFailedConnectionText.setFont(context.fonts->get(Fonts::Main));
	mFailedConnectionText.setString("Attempting to connect...");
	mFailedConnectionText.setCharacterSize(35);
	mFailedConnectionText.setColor(sf::Color::White);
	centerOrigin(mFailedConnectionText);
	mFailedConnectionText.setPosition(mWindow.getSize().x / 2.f, mWindow.getSize().y / 2.f);

	mWindow.clear(sf::Color::Black);
	mWindow.draw(mFailedConnectionText);
	mWindow.display();
	mFailedConnectionText.setString("Could not connect to remote server!");
	centerOrigin(mFailedConnectionText);

	sf::IpAddress ip;
	if (isHost) {
		mGameServer.reset(new GameServer(sf::Vector2f(mWindow.getSize())));
		ip = "127.0.0.1";
	}
	else {
		ip = getAddressFromFile();
	}
	if (mSocket.connect(ip, ServerPort, sf::seconds(5.f)) == sf::TcpSocket::Done)
		mConnected = true;
	else
		mFailedConnectionClock.restart();
	mSocket.setBlocking(false);
	context.music->play(Music::MissionTheme);
}

void MultiplayerGameState::draw() {
	if (mConnected) {
		mWorld.draw();
		mWindow.setView(mWindow.getDefaultView());
		if (!mBroadcasts.empty())
			mWindow.draw(mBroadcastText);
		if (mLocalPlayerIdentifiers.size() < 2 && mPlayerInvitationTime < sf::seconds(0.5f))
			mWindow.draw(mPlayerInvitationText);
	}
	else {
		mWindow.draw(mFailedConnectionText);
	}
}

void MultiplayerGameState::onActivate() {
	mActiveState = true;
}

void MultiplayerGameState::onDestroy() {
	if (!mHost && mConnected) {
		// Inform the Server that this is dying
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Client::Quit);
		mSocket.send(packet);
	}
}

bool MultiplayerGameState::update(sf::Time dt) {
	if (mConnected) {
		mWorld.update(dt);

		// Remove players whose aircrafts were destroyed
		bool foundLocalPlane = false;
		for (auto itr = mPlayers.begin(); itr != mPlayers.end();) {
			if (std::find(mLocalPlayerIdentifiers.begin(), mLocalPlayerIdentifiers.end(), itr->first) != mLocalPlayerIdentifiers.end())
				foundLocalPlane = true;


		}
	}
	else if (mFailedConnectionClock.getElapsedTime() >= sf::seconds(5.f)) {
		requestStateClear();
		requestStackPush(States::Menu);
	}
	return true;
}

void MultiplayerGameState::disableAllRealtimeActions() {
	mActiveState = false;
	for (sf::Int32 identifier : mLocalPlayerIdentifiers)
		mPlayers[identifier]->disableAllRealtimeActions();
}

bool MultiplayerGameState::handleEvent(const sf::Event& event) {
	CommandQueue& commands = mWorld.getCommandQueue();
	for (auto& pair : mPlayers)
		pair.second->handleEvent(event, commands);
	if (event.type == sf::Event::KeyPressed) {
		// Enter pressed, add second player co-op (only if we are one player)
		if (event.key.code == sf::Keyboard::Return && mLocalPlayerIdentifiers.size() == 1) {
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Client::RequestCoopPartner);
			mSocket.send(packet);
		}
		// Escape pressed, trigger the pause screen
		else if (event.key.code == sf::Keyboard::Escape) {
			disableAllRealtimeActions();
			requestStackPush(States::NetworkPause);
		}
	}
	else if (event.type == sf::Event::GainedFocus)
		mHasFocus = true;
	else if (event.type == sf::Event::LostFocus)
		mHasFocus = false;

	return true;
}

void MultiplayerGameState::updateBroadcastMessage(sf::Time elapsedTime) {
	if (mBroadcasts.empty())
		return;

	// Update broadcast timer
	mBroadcastElapsedTime += elapsedTime;
	if (mBroadcast)
}

void MultiplayerGameState::handlePacket(sf::Int32 packetType, sf::Packet& packet) {
	switch (packetType) {
	case Server::BroadcastMessage: {
			std::string message;
			packet >> message;
			mBroadcasts.push_back(message);
			if (mBroadcasts.size() == 1) {
				mBroadcastText.setString(mBroadcasts.front());
				centerOrigin(mBroadcastText);
				mBroadcastElapsedTime = sf::Time::Zero;
			}
	} break;

	// Order from Server to spawn 1 player Aircraft when connect
	case Server::SpawnSelf: {
		sf::Int32 aircraftIdentifier;
		sf::Vector2f aircraftPosition;
		packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y;
		Aircraft* aircraft = mWorld.addAircraft(aircraftIdentifier);
		aircraft->setPosition(aircraftPosition);
		mPlayers[aircraftIdentifier].reset(new Player(&mSocket, aircraftIdentifier, getContext().keys1));
		mLocalPlayerIdentifiers.push_back(aircraftIdentifier);
		mGameStarted = true;
	} break;

	// When a remote Player connects -> update on current screen
	case Server::PlayerConnect: {
		sf::Int32 aircraftIdentifier;
		sf::Vector2f aircraftPosition;
		packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y;
		Aircraft* aircraft = mWorld.addAircraft(aircraftIdentifier);
		aircraft->setPosition(aircraftPosition);
		mPlayers[aircraftIdentifier].reset(new Player(&mSocket, aircraftIdentifier, nullptr)); // current local has no control over remote Aircraft
	} break;

	case Server::PlayerDisconnect: {
		sf::Int32 aircraftIdentifier;
		packet >> aircraftIdentifier;
		mWorld.removeAircraft(aircraftIdentifier);
		mPlayers.erase(aircraftIdentifier);
	} break;

	case Server::InitialState: {
		sf::Int32 aircraftCount;
		float worldHeight, currentScroll;
		packet >> worldHeight >> currentScroll >> aircraftCount;
		mWorld.setWorldHeight(worldHeight);
		mWorld.setCurrentBattleFieldPosition(currentScroll);
		for (sf::Int32 i = 0; i < aircraftCount; ++i) {
			sf::Int32 aircraftIdentifier;
			sf::Int32 hitpoints;
			sf::I
		}
	} break;

	case Server::SpawnEnemy:{
		float height;
		sf::Int32 type;
		float relativeX;
	}

	case Server::MissionSuccess: {
		requestStackPush(States::MissionSuccess);
	} break;

	case Server::SpawnPickup: {
		sf::Int32 type;
		sf::Vector2f position;
		packet >> type >> position.x >> position.y;
		mWorld.createPickup(position, static_cast<Pickup::Type>(type));
	} break;

	case Server::UpdateClientState: {
		float currentWorldPosition;
		sf::Int32 aircraftCount;
		packet >> currentWorldPosition >> aircraftCount;
		float currentViewPosition = mWorld.getViewBounds().top + mWorld.getViewBounds().height;
		// compare local View bottom with Server View bottom; ideally they are same, otherwise compensating for lag (this value will be used in local World)
		mWorld.setWorldScrollCompensation(currentViewPosition / currentWorldPosition); 
		for (sf::Int32 i = 0; i < aircraftCount; ++i) {
			sf::Vector2f aircraftPosition;
			sf::Int32 aircraftIdentifier;
			packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y;

			Aircraft* aircraft = mWorld.getAircraft(aircraftIdentifier);
			bool isLocalPlane = std::find(mLocalPlayerIdentifiers.begin(), mLocalPlayerIdentifiers.end(), aircraftIdentifier) != mLocalPlayerIdentifiers.end();
			// For remote Aircrafts from other players -> lag compensating, update slowly to avoid "Aircraft jumps"
			if (aircraft && !isLocalPlane) {
				sf::Vector2f interpolatedPosition = aircraft->getPosition() + (aircraftPosition - aircraft->getPosition()) * 0.1f;
				aircraft->setPosition(interpolatedPosition);
			}
		}
	} break;


	}
}