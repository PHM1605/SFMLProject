#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "CommandQueue.hpp"
class Player: private sf::NonCopyable {
public:
	typedef PlayerAction::Type Action;

	enum MissionStatus {
		MissionRunning, MissionSuccess, MissionFailure
	};
public:
	Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding);

	void handleEvent(const sf::Event& event, CommandQueue& commands);
	void handleRealtimeInput(CommandQueue& commands);
	void handleRealtimeNetworkInput(CommandQueue& commands);
	void handleNetworkEvent(Action action, CommandQueue& commands);
	void handleNetworkRealtimeChange(Action action, bool actionEnabled);
	
	void setMissionStatus(MissionStatus status);
	MissionStatus getMissionStatus() const;

	void disableAllRealtimeActions();
	bool isLocal() const;

private: 
	void initializeActions();
private:
	const KeyBinding* mKeyBinding;
	std::map<Action, Command> mActionBinding;
	std::map<Action, bool> mActionProxies;
	MissionStatus mCurrentMissionStatus;
	int mIdentifier; // also the Aircraft ID which Player controls
	sf::TcpSocket* mSocket;
};