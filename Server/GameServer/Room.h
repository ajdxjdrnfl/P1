#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
				Room();
	virtual		~Room();

	void		Init();
	void		Update(float deltaTime);

public:
	bool		HandleEnterGame(GameSessionRef session);
	bool		HandleLeaveGame(GameSessionRef session);
	bool		HandleMove(Protocol::C_MOVE pkt);
	bool		HandleSkill(Protocol::C_SKILL pkt);
	
private:
	void		EnterGame(PlayerRef player);
	void		LeaveGame(PlayerRef player);
	void		SetObjectToRandomPos(GameObjectRef player);

public:
	void		Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

public:
	RoomRef		GetRoomRef();
	GameObjectRef GetGameObjectRef(uint64 id);
	
private:
	const int32 _maxEnemyCount = 10;

protected:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, EnemyRef> _enemies;
	unordered_map<uint64, SkillActorRef> _skillActors;

};

extern RoomRef GRoom;