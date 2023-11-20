#include "ParallelTask.hpp"

ParallelTask::ParallelTask():
	mThread(&ParallelTask::runTask, this),
	mFinished(false)
{}

void ParallelTask::execute() {
	mFinished = false;
	mElapsedTime.restart();
	mThread.launch();
}

bool ParallelTask::isFinished() {
	// Protect mFinished to be accessed from outside
	sf::Lock lock(mMutex);
	return mFinished;
}

float ParallelTask::getCompletion() {
	sf::Lock lock(mMutex);
	// 100% completion when 10s
	return mElapsedTime.getElapsedTime().asSeconds() / 10.f;
}

void ParallelTask::runTask() {
	// Dummy task - stall 10 seconds
	bool ended = false;
	while (!ended) {
		sf::Lock lock(mMutex);
		if (mElapsedTime.getElapsedTime().asSeconds() >= 10.f)
			ended = true;
	}
	{
		// Lock mMutex
		sf::Lock lock(mMutex);
		mFinished = true;
	}
}