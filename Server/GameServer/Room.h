#pragma once
#include "JobQueue.h"

// priority queue node for A* algorithm;
struct PQNode
{
	PQNode(int32 g, int32 h, VectorInt pos) : g(g), h(h), pos(pos) { }

	bool operator<(const PQNode& other) const { return g + h < other.g + other.h; }
	bool operator>(const PQNode& other) const { return g + h > other.g + other.h; }

	int32 g;
	int32 h;
	VectorInt pos;

};

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
	bool			HandleMontagePkt(Protocol::C_MONTAGE pkt);

public:
	void			HandleDead(GameObjectRef gameObject);
	bool			HandleSkill(GameObjectRef caster, uint64 skillid, Vector skillActorPos, float yaw, float damage);
	bool			HandleMontage(Protocol::S_MONTAGE pkt);
	bool			HandleSpawn(Protocol::S_SPAWN pkt);
	bool			HandleDespawn(Protocol::S_DESPAWN pkt, bool remove = false);

	StructureRef	SpawnStructure(Vector pos);

private:
	void			EnterGame(PlayerRef player);
	void			SpawnSkill(SkillActorRef skillActor);
	void			LeaveGame(PlayerRef player);

	bool			RemoveObject(GameObjectRef gameObject);

	void			SetObjectToRandomPos(GameObjectRef player);

public:
	void			Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

	// Temp Utils with map
public:
	PlayerRef		FindClosestPlayer(Vector pos);
	VectorInt GetGridPos(Vector pos);
	Vector GetPosition(VectorInt gridPos);

	// Find Path a*
	bool FindPath(Vector start, Vector end, vector<VectorInt>& path, int32 maxDepth = 100);
	bool CanGo(VectorInt current, int32 dir);
	bool IsValidAtPos(VectorInt gridPos);

public:
	RoomRef			GetRoomRef();
	GameObjectRef	GetGameObjectRef(uint64 id);
	
private:
	const int32 _maxEnemyCount = 1;

	bool _debug = false;
	bool _useQuadTree = true;
private:
	class Map* _map;
	string _mapName = "Test";

	class QuadTree* _tree = nullptr;

protected:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, EnemyRef> _enemies;
	unordered_map<uint64, SkillActorRef> _skillActors;
	unordered_map<uint64, StructureRef> _structures;
	BossRef _boss;

	GameTickManager _tickManager;
	
};

extern RoomRef GRoom;