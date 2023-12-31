#pragma once
#include "ResourceIdentifiers.hpp"
#include <SFML/Audio.hpp>
#include <list>
#include <cmath>


// Note: Linker Input these files: https://www.sfml-dev.org/tutorials/2.6/start-vc.php
class SoundPlayer : private sf::NonCopyable {
public:
	SoundPlayer();
	void play(SoundEffect::ID effect);
	void play(SoundEffect::ID effect, sf::Vector2f position);
	void removeStoppedSounds();
	void setListenerPosition(sf::Vector2f position);
	sf::Vector2f getListenerPosition() const;
private:
	SoundBufferHolder mSoundBuffers;
	std::list<sf::Sound> mSounds;
};