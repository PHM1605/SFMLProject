#include <SFML/Graphics.hpp>
#include "CommandQueue.hpp"
class Player {
public:
	enum Action {
		MoveLeft, MoveRight, MoveUp, MoveDown, ActionCount
	};
public:
	Player();
public:
	void handleEvent(const sf::Event& event, CommandQueue& commands);
	void handleRealtimeInput(CommandQueue& commands);
};