#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include "State.hpp"
#include "StateIdentifiers.hpp"



class StateStack : private sf::NonCopyable {
public:
	enum Action {Push, Pop, Clear};
public:
	explicit StateStack(State::Context context);

	template<typename T>
	void registerState(States::ID stateID);
	template<typename T, typename Param1>
	void registerState(States::ID, Param1 arg1);

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

// T is State files e.g. MultiplayerGameState; States::ID is depends on which pressed Button e.g. States::HostGame, States::JoinGame
template<typename T>
void StateStack::registerState(States::ID stateID) {
	mFactories[stateID] = [this]() {
		return State::Ptr(new T(*this, mContext));
	};
}

template<typename T, typename Param1>
void StateStack::registerState(States::ID stateID, Param1 arg1) {
	mFactories[stateID] = [this, arg1]() {
		return State::Ptr(new T(*this, mContext, arg1))
	};
}