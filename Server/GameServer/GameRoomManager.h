#pragma once
class GameRoomManager
{
public:
	GameRoomManager();
	~GameRoomManager();

	void Init();
	void Update(float deltaTime);

	void EnterGame(GameSessionRef session);
	void ExitGame(GameSessionRef session);

	bool ChangeRoom(PlayerRef session);


private:
	USE_LOCK;
	// map name - room
	unordered_map<string, RoomRef> _rooms;

};

extern GameRoomManager GRoomManager;