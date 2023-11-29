#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

class GameServer {
public:
	GameServer(sf::Vector2f battlefieldSize);
	~GameServer();
private:
	struct RemotePeer {
		RemotePeer();
		sf::TcpSocket socket;
		sf::Time lastPacketTime; // timestamp of last packet received from that Peer -> kickout when too long
		std::vector<sf::Int32> aircraftIdentifiers;
		bool ready;
		bool timedOut;
	};
	struct AircraftInfo {
		sf::Vector2f position;
		sf::Int32 hitpoints;
		sf::Int32 missileAmmo;
		std::map<sf::Int32, bool> realtimeActions;
	};
	typedef std::unique_ptr<RemotePeer> PeerPtr;
private:
	void setListening(bool enable);
	void executionThread();
	void tick(); // update client
	sf::Time now() const; // return current time

	void handleIncomingPackets();
	void handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout);

	void handleIncomingConnections();
	void handleDisconnections();
	void sendToAll(sf::Packet& packet);
	void updateClientState();

private:
	sf::Thread mThread;
	sf::Clock mClock;
	sf::TcpListener mListenerSocket;
	bool mListeningState; // is Server listening or not
	sf::Time mClientTimeoutTime;
	std::size_t mMaxConnectedPlayers;
	std::size_t mConnectedPlayers; 
	float mWorldHeight;
	std::size_t mAircraftCount; // total number of aircraft over all peers
	std::map<sf::Int32, AircraftInfo> mAircraftInfo; // map Aircraft ID and Info struct
	std::vector<PeerPtr> mPeers; // total number of peers PLUS one
	sf::Int32 mAircraftIdentifierCounter;
	bool mWaitingThreadEnd;
	sf::Time mLastSpawnTime;
	sf::Time mTimeForNextSpawn; // starts at 1
};