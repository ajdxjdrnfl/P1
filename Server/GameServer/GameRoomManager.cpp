#include "pch.h"
#include "GameRoomManager.h"
#include "Player.h"
#include "Room.h"
#include "GameSession.h"

GameRoomManager GRoomManager;

GameRoomManager::GameRoomManager()
{

}

GameRoomManager::~GameRoomManager()
{

}

void GameRoomManager::Update(float deltaTime)
{
	WRITE_LOCK;
	for (auto& p : _rooms)
	{
		RoomRef room = p.second;
		room->DoAsync(&Room::Update, deltaTime);
	}

	GRoom->Update(deltaTime);
}

void GameRoomManager::Init()
{
	/*namespace fs = std::filesystem;

	filesystem::directory_iterator itr(filesystem::current_path() / "Map");
	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;

		string name = entry.path().filename().replace_extension("").string();

		RoomRef room = make_shared<Room>(entry.path().string());
		room->Init();
		_rooms[name] = room;
		itr++;

	}*/

	GRoom->Init();
}

void GameRoomManager::EnterGame(GameSessionRef session)
{
	WRITE_LOCK;
	RoomRef room = GRoom;

	if (room)
		room->DoAsync(&Room::HandleEnterGame, session);
}

void GameRoomManager::ExitGame(GameSessionRef session)
{

	WRITE_LOCK;
	RoomRef room = session->_player.load()->GetRoomRef();

	if (room)
		room->DoAsync(&Room::HandleLeaveGame, session);
}

bool GameRoomManager::ChangeRoom(PlayerRef player)
{

	return false;
}
