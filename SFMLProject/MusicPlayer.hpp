#pragma once
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include "ResourceIdentifiers.hpp"

class MusicPlayer : private sf::NonCopyable {
public:
	MusicPlayer();
	void play(Music::ID Theme);
	void stop();
	void setPaused(bool paused);
	void setVolume(float volume);
private:
	sf::Music mMusic;
	std::map<Music::ID, std::string> mFilenames;
	float mVolume;
};