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
	mWaitingThreadEnd(false)
{
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
			mBattlefieldRect.top += mBattleFieldScrollSpeed * stepInterval.asSeconds();
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

// Send a packet to all peers
void GameServer::sendToAll(sf::Packet& packet) {
	for (PeerPtr& peer : mPeers) {
		if (peer->ready)
			peer->socket.send(packet);
	}
}