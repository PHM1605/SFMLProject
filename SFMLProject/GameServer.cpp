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
	
	// ADD MORE HERE

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