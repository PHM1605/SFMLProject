#include "GameServer.hpp"

GameServer::RemotePeer::RemotePeer() :
	ready(false),
	timedOut(false)
{
	socket.setBlocking(false);
}

GameServer::GameServer(sf::Vector2f battlefieldSize) :
	mThread(&GameServer::executionThread, this),
	mListeningState(false),
	mClientTimeoutTime(sf::seconds(3.f)),
	mMaxConnectedPlayers(10),
	mConnectedPlayers(0),
	mWorldHeight(5000.f),
	mBattleFieldRect(0.f, mWorldHeight - battlefieldSize.y, battlefieldSize.x, battlefieldSize.y),
	mBattleFieldScrollSpeed(-50.f),
	mAircraftCount(0),
	mPeers(1),
	mAircraftIdentifierCounter(1),
	mWaitingThreadEnd(false),
	mLastSpawnTime(sf::Time::Zero),
	mTimeForNextSpawn(sf::seconds(5.f))
{
	mListenerSocket.setBlocking(false);
	mPeers[0].reset(new RemotePeer());
	// Start server thread
	mThread.launch();
}

GameServer::~GameServer() {
	mWaitingThreadEnd = true;
	mThread.wait();
}

void GameServer::notifyPlayerRealtimeChange(sf::Int32 aircraftIdentifier, sf::Int32 action, bool actionEnabled) {
	for (std::size_t i = 0; i < mConnectedPlayers; ++i) {
		if (mPeers[i]->ready) {
			sf::Packet packet; 
			packet << static_cast<sf::Int32>(Server::PlayerRealtimeChange);
			packet << aircraftIdentifier;
			packet << action;
			packet << actionEnabled;
			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::notifyPlayerEvent(sf::Int32 aircraftIdentifier, sf::Int32 action) {
	for (std::size_t i = 0; i < mConnectedPlayers; ++i) {
		if (mPeers[i]->ready) {
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PlayerEvent);
			packet << aircraftIdentifier;
			packet << action;
			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::notifyPlayerSpawn(sf::Int32 aircraftIdentifier) {
	for (std::size_t i = 0; i < mConnectedPlayers; ++i) {
		if (mPeers[i]->ready) {
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PlayerConnect);
			packet << aircraftIdentifier << mAircraftInfo[aircraftIdentifier].position.x << mAircraftInfo[aircraftIdentifier].position.y;
			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::setListening(bool enable) {
	if (enable) {
		if (!mListeningState)
			mListeningState = (mListenerSocket.listen(ServerPort) == sf::TcpListener::Done);
	}
	else {
		mListenerSocket.close();
		mListeningState = false;
	}
} 

void GameServer::executionThread() {
	setListening(true);

	// For Server gaming state update e.g. scrolling view
	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	// For Client update - less frequent
	sf::Time tickInterval = sf::seconds(1.f / 20.f);
	sf::Time tickTime = sf::Time::Zero;
	sf::Clock stepClock, tickClock;

	while (!mWaitingThreadEnd) {
		handleIncomingPackets();
		handleIncomingConnections();
		stepTime += stepClock.getElapsedTime();
		stepClock.restart();
		tickTime += tickClock.getElapsedTime();
		tickClock.restart();

		// Server gaming state update e.g. scrolling view
		while (stepTime >= stepInterval) {
			mBattleFieldRect.top += mBattleFieldScrollSpeed * stepInterval.asSeconds();
			stepTime -= stepInterval;
		}

		// Client update - less frequent
		while (tickTime >= tickInterval) {
			tick();
			tickTime -= tickInterval;
		}

		// Sleep to prevent server from consuming 100% CPU
		sf::sleep(sf::milliseconds(100));
	}
}

void GameServer::tick() {
	updateClientState();

	// Check if all players have crossed the finishing line
	bool allAircraftsDone = true;
	for (auto pair : mAircraftInfo) {
		if (pair.second.position.y > 0.f)
			allAircraftsDone = false;
	}
	if (allAircraftsDone) {
		sf::Packet missionSuccessPacket;
		missionSuccessPacket << static_cast<sf::Int32>(Server::MissionSuccess);
		sendToAll(missionSuccessPacket);
	}

	// Check if any Aircraft is destroyed (lose game)
	for (auto itr = mAircraftInfo.begin(); itr != mAircraftInfo.end();) {
		if (itr->second.hitpoints <= 0)
			mAircraftInfo.erase(itr++);
		else
			++itr;
	}

	// Check to spawn enemies
	if (now() >= mLastSpawnTime + mTimeForNextSpawn) {
		// Do not spawn more enemies near the end
		if (mBattleFieldRect.top > 600.f) {
			std::size_t enemyCount = 1u + randomInt(2);
			float spawnCenter = static_cast<float>(randomInt(500) - 250); // +-250px comparing to last position
			// In case only one enemy is being spawned, it appears directly at the spawnCenter
			float planeDistance = 0.f;
			float nextSpawnPosition = spawnCenter;
			// In case there are two enemies being spawned together, each is spawned at each side of spawnCenter
			if (enemyCount == 2) {
				planeDistance = static_cast<float>(150 + randomInt(250)); // [150, 400]
				nextSpawnPosition = spawnCenter - planeDistance / 2.f;
			}

			for (std::size_t i = 0; i < enemyCount; ++i) {
				sf::Packet packet;
				packet << static_cast<sf::Int32>(Server::SpawnEnemy);
				packet << static_cast<sf::Int32>(1 + randomInt(Aircraft::TypeCount - 1)); // index 0 is of Player type (Eagle); randoming from the rest types
				packet << mWorldHeight - mBattleFieldRect.top + 500;
				packet << nextSpawnPosition;

				nextSpawnPosition += planeDistance;
				sendToAll(packet);
			}

			mLastSpawnTime = now();
			mTimeForNextSpawn = sf::milliseconds(2000 + randomInt(6000));
		}
	}
}

sf::Time GameServer::now() const {
	return mClock.getElapsedTime();
}

void GameServer::handleIncomingPackets() {
	bool detectedTimeout = false;
	for (PeerPtr& peer: mPeers) {
		if (peer->ready) {
			sf::Packet packet;
			while (peer->socket.receive(packet) == sf::Socket::Done) {
				handleIncomingPacket(packet, *peer, detectedTimeout);
				peer->lastPacketTime = now();
				packet.clear();
			}
			if (now() >= peer->lastPacketTime + mClientTimeoutTime) {
				peer->timedOut = true;
				detectedTimeout = true;
			}
		}
	}
	if (detectedTimeout)
		handleDisconnections();
}

void GameServer::handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout) {
	sf::Int32 packetType;
	packet >> packetType;
	switch (packetType) {
		case Client::Quit: {
			receivingPeer.timedOut = true;
			detectedTimeout = true;
		} break;

		case Client::PlayerEvent: {
			sf::Int32 aircraftIdentifier;
			sf::Int32 action;
			packet >> aircraftIdentifier >> action;
			notifyPlayerEvent(aircraftIdentifier, action);
		} break;
			
		case Client::PlayerRealtimeChange: {
			sf::Int32 aircraftIdentifier;
			sf::Int32 action;
			bool actionEnabled;
			packet >> aircraftIdentifier >> action >> actionEnabled;
			mAircraftInfo[aircraftIdentifier].realtimeActions[action] = actionEnabled;
			notifyPlayerRealtimeChange(aircraftIdentifier, action, actionEnabled);
		} break;

		case Client::RequestCoopPartner: {
			receivingPeer.aircraftIdentifiers.push_back(mAircraftIdentifierCounter);
			mAircraftInfo[mAircraftIdentifierCounter].position = sf::Vector2f(mBattleFieldRect.width / 2, mBattleFieldRect.top + mBattleFieldRect.height / 2);
			mAircraftInfo[mAircraftIdentifierCounter].hitpoints = 100;
			mAircraftInfo[mAircraftIdentifierCounter].missileAmmo = 2;

			// To accept current peer about this new plane
			sf::Packet requestPacket;
			requestPacket << static_cast<sf::Int32>(Server::AcceptCoopPartner);
			requestPacket << mAircraftIdentifierCounter;
			requestPacket << mAircraftInfo[mAircraftIdentifierCounter].position.x;
			requestPacket << mAircraftInfo[mAircraftIdentifierCounter].position.y;
			receivingPeer.socket.send(requestPacket);
			mAircraftCount++;

			// To inform other peers about this new plane
			for (PeerPtr& peer : mPeers) {
				if (peer.get() != &receivingPeer && peer->ready) {
					sf::Packet notifyPacket;
					notifyPacket << static_cast<sf::Int32>(Server::PlayerConnect);
					notifyPacket << mAircraftIdentifierCounter;
					notifyPacket << mAircraftInfo[mAircraftIdentifierCounter].position.x;
					notifyPacket << mAircraftInfo[mAircraftIdentifierCounter].position.y;
					peer->socket.send(notifyPacket);
				}
			}
			mAircraftIdentifierCounter++;
		} break;

		case Client::PositionUpdate: {
			sf::Int32 numAircrafts;
			packet >> numAircrafts;
			for (sf::Int32 i = 0; i < numAircrafts; ++i) {
				sf::Int32 aircraftIdentifier;
				sf::Int32 aircraftHitpoints;
				sf::Int32 missileAmmo;
				sf::Vector2f aircraftPosition;
				packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y >> aircraftHitpoints >> missileAmmo;
				mAircraftInfo[aircraftIdentifier].position = aircraftPosition;
				mAircraftInfo[aircraftIdentifier].hitpoints = aircraftHitpoints;
				mAircraftInfo[aircraftIdentifier].missileAmmo = missileAmmo;
			}
		} break;

		// Only used for enemy explosion event for now. Listen only for the first Peer (host)
		case Client::GameEvent: {
			sf::Int32 action;
			float x, y;
			packet >> action >> x >> y;
			// When enemy exploding, spawn pickup at random rate
			if (action == GameActions::EnemyExplode && randomInt(3) == 0 && &receivingPeer == mPeers[0].get()) {
				sf::Packet packet;
				packet << static_cast<sf::Int32>(Server::SpawnPickup);
				packet << static_cast<sf::Int32>(randomInt(Pickup::TypeCount));
				packet << x;
				packet << y;
				sendToAll(packet);
			}
		}
	}
}

void GameServer::updateClientState() {
	sf::Packet updateClientStatePacket;
	updateClientStatePacket << static_cast<sf::Int32>(Server::UpdateClientState);
	updateClientStatePacket << static_cast<float>(mBattleFieldRect.top + mBattleFieldRect.height);
	updateClientStatePacket << static_cast<sf::Int32>(mAircraftInfo.size());
	for (auto aircraft : mAircraftInfo) {
		// first: Aircraft ID, second: AircraftInfo struct
		updateClientStatePacket << aircraft.first << aircraft.second.position.x << aircraft.second.position.y;
	}
	sendToAll(updateClientStatePacket);
}

void GameServer::handleIncomingConnections() {
	if (!mListeningState)
		return;
	if (mListenerSocket.accept(mPeers[mConnectedPlayers]->socket) == sf::TcpListener::Done) {
		// order the new client to spawn its own plane
		mAircraftInfo[mAircraftIdentifierCounter].position = sf::Vector2f(mBattleFieldRect.width / 2, mBattleFieldRect.top + mBattleFieldRect.height / 2);
		mAircraftInfo[mAircraftIdentifierCounter].hitpoints = 100;
		mAircraftInfo[mAircraftIdentifierCounter].missileAmmo = 2;

		// see NetworkProtocol.hpp for Packet protocol
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Server::SpawnSelf);
		packet << mAircraftIdentifierCounter;
		packet << mAircraftInfo[mAircraftIdentifierCounter].position.x;
		packet << mAircraftInfo[mAircraftIdentifierCounter].position.y;

		mPeers[mConnectedPlayers]->aircraftIdentifiers.push_back(mAircraftIdentifierCounter);
		broadcastMessage("New player!");
		informWorldState(mPeers[mConnectedPlayers]->socket);
		notifyPlayerSpawn(mAircraftIdentifierCounter++);

		mPeers[mConnectedPlayers]->socket.send(packet);
		mPeers[mConnectedPlayers]->ready = true;
		mPeers[mConnectedPlayers]->lastPacketTime = now();
		mAircraftCount++;
		mConnectedPlayers++;
		
		if (mConnectedPlayers >= mMaxConnectedPlayers)
			setListening(false);
		else // add a new waiting peer
			mPeers.push_back(PeerPtr(new RemotePeer()));
	}
}

void GameServer::handleDisconnections() {
	for (auto itr = mPeers.begin(); itr != mPeers.end();) {
		if ((*itr)->timedOut) {
			// Inform everyone of the disconnection, erase
			for (sf::Int32 identifier : (*itr)->aircraftIdentifiers) {
				sendToAll(sf::Packet() << static_cast<sf::Int32>(Server::PlayerDisconnect) << identifier);
				mAircraftInfo.erase(identifier); // erase info struct of Airplane from <map>
			}
			mConnectedPlayers--;
			mAircraftCount -= (*itr)->aircraftIdentifiers.size();
			itr = mPeers.erase(itr); // after erase that Peer, itr points to the next peer on the vector

			// Go back to listening state
			if (mConnectedPlayers < mMaxConnectedPlayers) {
				mPeers.push_back(PeerPtr(new RemotePeer()));
				setListening(true);
			}

			broadcastMessage("An ally has disconnected.");
		}
		else
			++itr;
	}
}

// Tell the newly connected Peer about how the world is currently
void GameServer::informWorldState(sf::TcpSocket& socket) {
	sf::Packet packet;
	packet << static_cast<sf::Int32>(Server::InitialState);
	packet << mWorldHeight << mBattleFieldRect.top + mBattleFieldRect.height;
	packet << static_cast<sf::Int32>(mAircraftCount);
	
	for (std::size_t i = 0; i < mConnectedPlayers; ++i) {
		if (mPeers[i]->ready) {
			for (sf::Int32 identifier : mPeers[i]->aircraftIdentifiers)
				packet << identifier << mAircraftInfo[identifier].position.x << mAircraftInfo[identifier].position.y << mAircraftInfo[identifier].hitpoints << mAircraftInfo[identifier].missileAmmo;
		}
	}

	socket.send(packet);
}

void GameServer::broadcastMessage(const std::string& message) {
	for (std::size_t i = 0; i < mConnectedPlayers; ++i) {
		if (mPeers[i]->ready) {
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::BroadcastMessage);
			packet << message;
			mPeers[i]->socket.send(packet);
		}
	}
}

// Send a packet to all peers
void GameServer::sendToAll(sf::Packet& packet) {
	for (PeerPtr& peer : mPeers) {
		if (peer->ready)
			peer->socket.send(packet);
	}
}