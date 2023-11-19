#pragma once
#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "StateIdentifiers.hpp"

class StateStack : private sf::NonCopyable {
public:
	enum Action {Push, Pop, Clear};
public:
	explicit StateStack(State::Context context);
	template<typename T>
	void registerState(States::ID stateID);
	void update(sf::Time dt);
	void draw();
	void handleEvent(const sf::Event& event);
	void pushState(States::ID stateID);
	void popState();
	void clearStates();
	bool isEmpty() const;
private:
	struct PendingChange {
		Action action;
		States::ID stateID;
		explicit PendingChange(Action action, States::ID = States::None);
	};
	std::vector<State::Ptr> mStack;
	std::vector<PendingChange> mPendingList;
	State::Context mContext;
	std::map<States::ID, std::function<State::Ptr()>> mFactories;
private:
	State::Ptr createState(States::ID stateID);
	void applyPendingChanges();
};