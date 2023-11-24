#pragma once
#include <SFML/Graphics.hpp>
#include "CommandQueue.hpp"
class Player {
public:
	enum Action {
		MoveLeft, MoveRight, MoveUp, MoveDown, Fire, LaunchMissile, ActionCount
	};
	enum MissionStatus {
		MissionRunning, MissionSuccess, MissionFailure
	};
public:
	Player();
	void assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key getAssignedKey(Action action) const;
	void handleEvent(const sf::Event& event, CommandQueue& commands);
	void handleRealtimeInput(CommandQueue& commands);
	void setMissionStatus(MissionStatus status);
	MissionStatus getMissionStatus() const;
private: 
	static bool isRealtimeAction(Action action);
	void initializeActions();
private:
	std::map<sf::Keyboard::Key, Action> mKeyBinding;
	std::map<Action, Command> mActionBinding;
	MissionStatus mCurrentMissionStatus;
};