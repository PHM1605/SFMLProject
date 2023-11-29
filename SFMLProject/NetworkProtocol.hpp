#pragma once
const unsigned short ServerPort = 5000;

// For packets originated from the Server
namespace Server {
	enum PacketType {
		BroadcastMessage, // send a message to Client -> they show on screen for some seconds
		SpawnSelf, // take Aircraft ID [sf::Int32] and two Floats for Position -> to spawn an Aircraft
		InitialState, // take World Height [float], Battlefield bottom [float], Aircrafts count [sf::Int32], for each Aircraft -> Aircraft ID and position (two floats)
		PlayerEvent,
		PlayerConnect,
		PlayerDisconnect  // take Aircraft ID [sf::Int32] of the Aircraft to be destroyed
	};
}