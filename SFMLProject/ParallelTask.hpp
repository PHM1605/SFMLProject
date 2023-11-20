#pragma once
#include <SFML/Graphics.hpp>

class ParallelTask {
public:
	ParallelTask();
	void execute();
	bool isFinished();
	float getCompletion();
private:
	sf::Thread mThread;
	bool mFinished;
	sf::Clock mElapsedTime;
	sf::Mutex mMutex;
private:
	void runTask();
};