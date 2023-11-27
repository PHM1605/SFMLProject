#include "Animation.hpp"

Animation::Animation() :
	mSprite(),
	mFrameSize(),
	mNumFrames(0),
	mCurrentFrame(0),
	mDuration(sf::Time::Zero),
	mElapsedTime(sf::Time::Zero),
	mRepeat(false) 
{}

