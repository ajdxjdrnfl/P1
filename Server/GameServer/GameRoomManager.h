#pragma once
class GameRoomManager
{
public:
	GameRoomManager();
	~GameRoomManager();

	void Init();
	void Update(float deltaTime);

	void EnterGame(GameSessionRef session, Protocol::C_LOGIN pkt);
	void ExitGame(GameSessionRef session);

	void EnterRaid(GameSessionRef session);

	bool ChangeRoom(PlayerRef session);

	RoomRef CreateRoom(ERoomType roomType);

private:
	USE_LOCK;
	// map name - room
	unordered_map<uint64, RoomRef> _rooms[ERT_MAX];

	static atomic<int64> s_idGenerator;
};

extern GameRoomManager GRoomManager;