#pragma once
#include "State.hpp"
#include <SFML/Network.hpp>
#include "World.hpp"
#include "GameServer.hpp"
#include <fstream>

class MultiplayerGameState : public State {
public:
	MultiplayerGameState(StateStack& stack, Context context, bool isHost);
	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);
	virtual void onActivate();
	void onDestroy();
	void disableAllRealtimeActions();
private:
	void updateBroadcastMessage(sf::Time elapsedTime);
	void handlePacket(sf::Int32 packetType, sf::Packet& packet);
private:
	typedef std::unique_ptr<Player> PlayerPtr;
private:
	World mWorld;
	sf::RenderWindow& mWindow;
	TextureHolder& mTextureHolder;
	std::map<int, PlayerPtr> mPlayers; // all Aircrafts from the whole gaming network
	std::vector<sf::Int32> mLocalPlayerIdentifiers; // only local Aircrafts
	sf::TcpSocket mSocket;
	bool mConnected;
	std::unique_ptr<GameServer> mGameServer;
	sf::Clock mTickClock;
	std::vector<std::string> mBroadcasts;
	sf::Text mBroadcastText;
	sf::Time mBroadcastElapsedTime;
	sf::Text mPlayerInvitationText;
	sf::Time mPlayerInvitationTime; // for blinking the mPlayerInvitationText
	sf::Text mFailedConnectionText;
	sf::Clock mFailedConnectionClock;
	bool mActiveState;
	bool mHasFocus;
	bool mHost;
	bool mGameStarted;
	sf::Time mClientTimeout;
	sf::Time mTimeSinceLastPacket;
};