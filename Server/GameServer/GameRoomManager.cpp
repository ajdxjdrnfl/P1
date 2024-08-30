#include "pch.h"
#include "GameRoomManager.h"
#include "Player.h"
#include "Room.h"
#include "Field.h"
#include "Raid.h"
#include "GameSession.h"

GameRoomManager GRoomManager;
atomic<int64> GameRoomManager::s_idGenerator = 1;

GameRoomManager::GameRoomManager()
{

}

GameRoomManager::~GameRoomManager()
{

}

void GameRoomManager::Update(float deltaTime)
{
	WRITE_LOCK;
	/*for (auto& p : _rooms)
	{
		RoomRef room = p.second;
		room->DoAsync(&Room::Update, deltaTime);
	}*/

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

void GameRoomManager::EnterGame(GameSessionRef session, Protocol::C_ENTER_GAME pkt)
{
	WRITE_LOCK;
	RoomRef room = GRoom;

	if (room)
		room->DoAsync(&Room::HandleEnterGame, session, pkt);
}

void GameRoomManager::ExitGame(GameSessionRef session)
{
	WRITE_LOCK;
	RoomRef room = session->_player.load()->GetRoomRef();

	if (room)
		room->DoAsync(&Room::HandleLeaveGame, session);
}

void GameRoomManager::EnterRaid(GameSessionRef session)
{
	RoomRef room = CreateRoom(ERT_RAID);
	
	
}

bool GameRoomManager::ChangeRoom(PlayerRef player)
{
	
	return false;
}

RoomRef GameRoomManager::CreateRoom(ERoomType roomType)
{
	RoomRef room;

	uint64 roomId = s_idGenerator.fetch_add(1);

	switch (roomType)
	{
	case ERoomType::ERT_FIELD:
		room = make_shared<Field>(roomId);
		break;
	case ERoomType::ERT_RAID:
		room = make_shared<Raid>(roomId);
		break;
	}
	
	{
		WRITE_LOCK;
		_rooms[roomType][roomId] = room;
	}
	
	return room;
}
