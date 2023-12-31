#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Player.hpp"
#include "StateIdentifiers.hpp"
#include "MusicPlayer.hpp"
#include "SoundPlayer.hpp"
#include <memory>

class StateStack;
class KeyBinding;

class State {
public:
	typedef std::unique_ptr<State> Ptr;

	struct Context {
		Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, MusicPlayer& music, SoundPlayer& sounds, KeyBinding& key1, KeyBinding& key2);
		sf::RenderWindow* window;
		TextureHolder* textures;
		FontHolder* fonts;
		MusicPlayer* music;
		SoundPlayer* sounds;
		KeyBinding* keys1; 
		KeyBinding* keys2;
	};
public:
	State(StateStack& stack, Context context);
	virtual ~State();
	virtual void draw() = 0;
	virtual bool update(sf::Time dt) = 0;
	virtual bool handleEvent(const sf::Event& event) = 0;
	virtual void onActivate();
	virtual void onDestroy();

protected:
	void requestStackPush(States::ID stateID);
	void requestStackPop();
	void requestStateClear();
	Context getContext() const;

private:
	StateStack* mStack;
	Context mContext;
};