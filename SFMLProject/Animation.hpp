#pragma once
#include <SFML/Graphics.hpp>

class Animation : public sf::Drawable, public sf::Transformable {
public:
	Animation();
private:
	sf::Sprite mSprite;
	sf::Vector2i mFrameSize;
	std::size_t mNumFrames;
	std::size_t mCurrentFrame;
	sf::Time mDuration;
	sf::Time mElapsedTime;
	bool mRepeat;
};