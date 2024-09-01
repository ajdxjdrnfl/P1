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
					Room(ERoomType roomType, uint64 roomId);
	virtual			~Room();

	virtual void	Init();
	virtual void	Update(float deltaTime);

public:
	bool			HandleEnterGame(GameSessionRef session, Protocol::C_ENTER_GAME pkt);
	bool			HandleLeaveGame(GameSessionRef session);
	bool			HandleMove(Protocol::C_MOVE pkt);
	bool			HandleSkillPkt(Protocol::C_SKILL pkt);
	bool			HandleAttack(Protocol::C_ATTACK pkt);
	bool			HandleMontagePkt(Protocol::C_MONTAGE pkt);

public:
	void			HandleDead(GameObjectRef gameObject);
	bool			HandleSkill(SkillActorRef skillActor);
	bool			HandlePredictSkill(Protocol::S_PREDICTSKILL pkt);
	bool			HandleDamage(Protocol::S_DAMAGE pkt);
	bool			HandleMontage(Protocol::S_MONTAGE pkt);
	bool			HandleSpawn(Protocol::S_SPAWN pkt);
	bool			HandleDespawn(Protocol::S_DESPAWN pkt, bool remove = false);

	bool			CreateSkillActor(GameObjectRef caster, uint64 skillid, Vector skillActorPos, float yaw, float damage);

	StructureRef	SpawnStructure(Vector pos);

private:
	void			EnterGame(PlayerRef player);
	void			SpawnSkill(SkillActorRef skillActor);
	void			LeaveGame(PlayerRef player);

	void			SetObjectToRandomPos(GameObjectRef player);

public:
	void			Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);
	// AOI version
	void			BroadcastAOI(SendBufferRef sendBuffer, GameObjectRef gameObject, uint64 exceptId = 0);

	// Temp Utils with map
public:
	GameObjectRef	FindClosestPlayer(Vector pos, float maxDistance);
	VectorInt		GetGridPos(Vector pos);
	Vector			GetPosition(VectorInt gridPos);

	float			GetValidHeight(VectorInt gridPos);
	// Find Path a*
	bool			FindPath(Vector start, Vector end, vector<VectorInt>& path, int32 maxDepth = 100);

	// Map에서 특정 방향으로 움직일 수 있는지 검사 : Grid 버전
	bool			CanGoByDirection(VectorInt current, int32 dir, bool checkCollision = false, class Collision* collision = nullptr);
	// 특정 collision이 Vector 방향으로 움직일 수 있는지 검사
	bool			CanGoByVector(class Collision* collision, Vector moveVector, bool checkCollision = false);

	bool			CheckCollisionInMap(class Collision* collison);
	bool			CheckCollisionInQuadTree(class Collision* collision, vector<GameObjectRef>& collideObjects);
	bool			IsWalkableAtPos(VectorInt gridPos);

	// 움직일 수 있는지 검사 : Vector 버전
	bool			GoMoveVector(Vector currentPos, Vector moveVector);

private:
	vector<VectorInt>	Bresenham(Vector start, Vector end);

public:
	RoomRef			GetRoomRef();
	GameObjectRef	GetGameObjectRef(uint64 id);

	// Actor Utils
protected:
	bool			AddGameObject(GameObjectRef gameObject);
	bool			RemoveGameObject(GameObjectRef gameObject);
	vector<PlayerRef>	GetPlayers();
	vector<GameObjectRef> GetGameObjects();
	
private:
	const int32 _maxEnemyCount = 0;

	bool _debug = true;
	bool _useQuadTree = true;

protected:
	class Map* _map;
	string _mapName = "Test";

	class QuadTree* _tree = nullptr;
	class QuadTree* _updatedTree = nullptr;

protected:
	unordered_map<uint64, GameObjectRef> _actors[Protocol::CasterType_ARRAYSIZE];
	
	GameTickManager _tickManager;
	ERoomType _roomType = ERoomType::ERT_NONE;
	uint64 _roomId = 1;
	
};

extern RoomRef GRoom;