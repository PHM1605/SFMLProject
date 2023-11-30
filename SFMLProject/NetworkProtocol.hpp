#pragma once
const unsigned short ServerPort = 5000;

// For packets originated from the Server
namespace Server {
	enum PacketType {
		BroadcastMessage, // send a message to Client -> they show on screen for some seconds
		SpawnSelf, // send Aircraft ID [sf::Int32] and two Floats for Position -> to spawn an Aircraft
		InitialState, // send World Height [float], Battlefield bottom [float], Aircrafts count [sf::Int32], for each Aircraft -> AircraftID and its position (two floats)
		PlayerEvent, // send Aircraft ID and Action ID (in Player - MoveUp/MoveDown/...)
		PlayerRealtimeChange, // send Aircraft ID and Realtime Action ID
		PlayerConnect, // send Aircraft ID and two Floats for Position of an entering Aircraft
		PlayerDisconnect,  // send Aircraft ID [sf::Int32] of the Aircraft to be destroyed
		AcceptCoopPartner, // send Aircraft ID and two Floats for Position -> tell Client to spawn another Aircraft
		SpawnEnemy, // send Aircraft Type [sf::Int32 - defined in Aircraft] and two Floats for EnemyPosition
		SpawnPickup, // send Pickup Type [sf::Int32 - defined in Pickup] and two Floats for PickupPosition
		UpdateClientState, // send Battlefield bottom, AircraftCount and for each Aircraft->AircraftID and its position (two floats)
		MissionSuccess // no body; to inform that the game is over
	};
}

namespace Client {
	enum PacketType {
		PlayerEvent, // AircraftID, EventID (in Player)
		PlayerRealtimeChange, // AircraftID, EventID, bool to state this Event is active or not
		RequestCoopPartner, // no body; when Client presses the Return key 
		PositionUpdate, // NumberOfAircrafts, for each Aircraft -> AircraftID, two floats of position (from Client tick code)
		GameEvent, // e.g. enemy explosion
		Quit // no body; to inform Server that game is over -> for Server to remove Aircraft
	};
}