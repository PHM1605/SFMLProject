#include "StateStack.hpp"

StateStack::StateStack(State::Context context)

template<typename T>
void StateStack::registerState(States::ID stateID) {
	mFactories[stateID] = [this]() {
		return State::Ptr(new T(*this, mContext));
		};
}

State::Ptr StateStack::createState(States::ID stateID) {
	auto found = mFactories.find(stateID);
	assert(found != mFactories.end());
	return found->second();
}