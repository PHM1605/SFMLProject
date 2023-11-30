#include "MultiplayerGameState.hpp"

sf::IpAddress getAddressFromFile() {
	{ // Try to open existing file (RAII block)
		std::ifstream inputFile("ip.txt");
		std::string ipAddress;
		if (inputFile >> ipAddress)
			return ipAddress;
	}

	// If open/read failed, create new file
	std::ofstream outputFile("ip.txt");
	std::string localAddress = "127.0.0.1";
	outputFile << localAddress;
	return localAddress;
}

MultiplayerGameState::MultiplayerGameState(StateStack& stack, Context context, bool isHost) :
	State(stack, context),
	mWorld(*context.window, *context.fonts, *context.sounds, true),
	mWindow(*context.window),
	mTextureHolder(*context.textures),
	mConnected(false),
	mGameServer(nullptr),
	mActiveState(true),
	mHasFocus(true),
	mHost(isHost),
	mGameStarted(false),
	mClientTimeout(sf::seconds(2.f)),
	mTimeSinceLastPacket(sf::seconds(0.f))
{
	mBroadcastText.setFont(context.fonts->get(Fonts::Main));
	mBroadcastText.setPosition(1024.f / 2, 100.f);

	sf::IpAddress ip;
	if (isHost) {
		mGameServer.reset(new GameServer(sf::Vector2f(mWindow.getSize())));
		ip = "127.0.0.1";
	}
	else {
		ip = getAddressFromFile();
	}
	if (mSocket.connect(ip, ServerPort, sf::seconds(5.f)) == sf::TcpSocket::Done)
		mConnected = true;
	else
		mFailedConnectionClock.restart();
	mSocket.setBlocking(false);
	context.music->play(Music::MissionTheme);
}