#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
					Room();
	virtual			~Room();

	virtual void	Init();
	virtual void	Update(float deltaTime);

public:
	bool			HandleEnterGame(GameSessionRef session);
	bool			HandleLeaveGame(GameSessionRef session);
	bool			HandleMove(Protocol::C_MOVE pkt);
	bool			HandleSkillPkt(Protocol::C_SKILL pkt);
	bool			HandleAttack(Protocol::C_ATTACK pkt);
	bool			HandleMontage(Protocol::C_MONTAGE pkt);

public:
	void			HandleDead(GameObjectRef gameObject);
	bool			HandleSkill(GameObjectRef caster, uint64 skillid);

private:
	void			EnterGame(PlayerRef player);
	void			SpawnSkill(SkillActorRef skillActor);
	void			LeaveGame(PlayerRef player);

	bool			RemoveObject(GameObjectRef gameObject);

	void			SetObjectToRandomPos(GameObjectRef player);

public:
	void			Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

	// Temp Utils
public:
	PlayerRef		FindClosestPlayer(Vector pos);

public:
	RoomRef			GetRoomRef();
	GameObjectRef	GetGameObjectRef(uint64 id);
	
private:
	const int32 _maxEnemyCount = 10;

protected:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, EnemyRef> _enemies;
	unordered_map<uint64, SkillActorRef> _skillActors;
	BossRef _boss;

};

extern RoomRef GRoom;