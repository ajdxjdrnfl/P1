#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "Enemy.h"
#include "SkillActor.h"
#include "GameSession.h"
#include "ObjectUtils.h"
#include "Collision.h"
#include "Boss.h"
#include "Structure.h"
#include "ResourceManager.h"
#include "Map.h"
#include "QuadTree.h"
#include "Collision.h"

//RoomRef GRoom = make_shared<Room>();

Room::Room()
{
	_tree = new QuadTree();
	_updatedTree = new QuadTree();
}

Room::~Room()
{
	delete _tree;
	delete _updatedTree;
}

void Room::Init()
{
	// TODO : enemy 추가
	_tickManager.Init();
	_map = GResourceManager.GetMap(_mapName);
	_tree->Init(_map->GetBound());
	_updatedTree->Init(_map->GetBound());

	for (int32 i = 0; i < _maxEnemyCount; i++)
	{
		EnemyRef enemy = ObjectUtils::CreateEnemy(GetRoomRef());
		AddGameObject(enemy);
		SetObjectToRandomPos(enemy);
	}

	if (!_debug)
	{

		BossRef boss = ObjectUtils::CreateBoss(GetRoomRef());
		AddGameObject(boss);
		SetObjectToRandomPos(boss);
	}
	
}

void Room::Update(float deltaTime)
{
	_tickManager.Update();

	deltaTime = _tickManager.GetDeltaTime();

	if (_useQuadTree)
	{
		_tree->Clear();
		_updatedTree->Clear();

		auto v = GetGameObjects();

		for (auto gameObject : v)
		{
			gameObject->PreUpdate(deltaTime);
			if (_tree != nullptr && gameObject->GetObjectInfo()->castertype() != Protocol::CASTER_TYPE_NONE)
				_tree->Insert(gameObject);
		}
	}

	for (auto& v : _actors)
	{
		for (auto& item : v)
		{
			auto gameObject = item.second;
			gameObject->Update(deltaTime);
			if(_updatedTree != nullptr && gameObject->GetObjectInfo()->castertype() != Protocol::CASTER_TYPE_NONE)
				_updatedTree->Insert(item.second);
		}
	}

	DoTimer(64, &Room::Update, deltaTime);
}

bool Room::HandleEnterGame(GameSessionRef session, Protocol::C_LOGIN pkt)
{
	PlayerRef player = ObjectUtils::CreatePlayer(session, GetRoomRef(), pkt.castertype());
	session->_player.store(player);
	EnterGame(player);

	{
		Protocol::S_ENTER_GAME pkt;
		pkt.set_success(true);

		*pkt.mutable_info() = *player->GetObjectInfo();

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		if (auto session = player->GetSessionRef())
			session->Send(sendBuffer);
		
	}
	
	// 기존 접속한 유저들에게 새로운 유저 접속 전송
	{
		Protocol::S_SPAWN pkt;
		
		Protocol::ObjectInfo* info = pkt.add_info();
		info->CopyFrom(*player->GetObjectInfo());
		
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Broadcast(sendBuffer, player->GetObjectInfo()->object_id());
	}

	// 접속한 유저에게 존재하는 오브젝트 전송
	{
		Protocol::S_SPAWN pkt;

		auto v = GetGameObjects();

		for (auto& item : v)
		{
			GameObjectRef gameObject = item;
			Protocol::ObjectInfo* info = pkt.add_info();
			info->CopyFrom(*gameObject->GetObjectInfo());
		}

		// TODO : 새로 접속한 유저에게 스킬 액터들도 같이 스폰할지?

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		if (auto session = player->GetSessionRef())
			session->Send(sendBuffer);

	}
	
	return true;
}

bool Room::HandleLeaveGame(GameSessionRef session)
{
	PlayerRef player = session->_player;
	const uint64 objectId = player->GetObjectInfo()->object_id();
	session->_player.store(nullptr);
	LeaveGame(player);

	// 퇴장하는 플레이어에게 퇴장 사실 전달
	{
		Protocol::S_DESPAWN pkt;

		auto v = GetPlayers();

		for (auto& player : v)
		{
			pkt.add_info()->CopyFrom(*player->GetObjectInfo());
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		if (session)
			session->Send(sendBuffer);
	}
	
	{
		/*Protocol::S_LEAVE_GAME pkt;

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		if (session)
			session->Send(sendBuffer);*/
	}

	// 다른 플레이어들에게 퇴장 사실 전달
	{
		Protocol::S_DESPAWN pkt;
		pkt.add_info()->CopyFrom(*player->GetObjectInfo());

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Broadcast(sendBuffer, objectId);
	}
	
	return false;
}

bool Room::HandleMove(Protocol::C_MOVE pkt)
{
	const uint64 objectId = pkt.info().object_id();

	PlayerRef player = static_pointer_cast<Player>(GetGameObjectRef(objectId));

	if (player->HandleMovePacket(pkt))
	{
		Protocol::S_MOVE movePkt;
		{
			Protocol::ObjectInfo* info = movePkt.mutable_info();
			info->CopyFrom(pkt.targetinfo());
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
		Broadcast(sendBuffer);
	}
	else return false;
	

	return true;

}

bool Room::HandleSkillPkt(Protocol::C_SKILL pkt)
{
	GameObjectRef caster = GetGameObjectRef(pkt.caster().object_id());
	
	Skill* skill =  GResourceManager.GetSkill(pkt.caster().castertype(), pkt.skillid());

	if (skill == nullptr)
		return false;

	float damage = skill->GetSkillInfo().damage;

	if (skill->GetSkillInfo().skillType == Protocol::SKILL_TYPE_CHARGING)
		damage = pkt.damage();

	return HandleSkill(caster, pkt.skillid(), { pkt.x(), pkt.y() }, pkt.yaw(), damage);
}

bool Room::HandleAttack(Protocol::C_ATTACK pkt)
{
	GameObjectRef victim = GetGameObjectRef(pkt.victim().object_id());
	GameObjectRef skillActor = GetGameObjectRef(pkt.skillactor().object_id());
	GameObjectRef caster = GetGameObjectRef(pkt.caster().object_id());

	if (victim == nullptr || skillActor == nullptr || caster == nullptr)
		return false;

	Collision* victimCollision = static_cast<Collision*>(victim->GetComponent(EComponentType::ECT_COLLISION));
	Collision* skillActorCollision = static_cast<Collision*>(skillActor->GetComponent(EComponentType::ECT_COLLISION));

	victimCollision->SetPos({pkt.victim().x(), pkt.victim().y()});
	skillActorCollision->SetPos({pkt.skillactor().x(), pkt.skillactor().y()});

	if (!victimCollision->CheckCollision(skillActorCollision))
	{
		victimCollision->SetPos(victim->GetPos());
		skillActorCollision->SetPos(skillActor->GetPos());
		return false;
	}

	SkillActorRef skillActorCast = static_pointer_cast<SkillActor>(skillActor);

	bool bCounter = pkt.counter();
	victim->TakeDamage(skillActor, skillActorCast->GetSkillInfo().damageType, skillActorCast->GetDamage());
	

	{
		Protocol::S_ATTACK attackPkt;
		*attackPkt.mutable_caster() = *caster->GetObjectInfo();
		*attackPkt.mutable_victim() = *victim->GetObjectInfo();
		*attackPkt.mutable_skillactor() = *skillActor->GetObjectInfo();
		attackPkt.set_skillid(skillActorCast->GetSkillInfo().skillNum);
		attackPkt.set_damage(skillActorCast->GetDamage());
		
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(attackPkt);
		Broadcast(sendBuffer);
	}

	victimCollision->SetPos(victim->GetPos());
	skillActorCollision->SetPos(skillActor->GetPos());
	
	return true;
}

bool Room::HandleMontagePkt(Protocol::C_MONTAGE pkt)
{
	GameObjectRef caster = GetGameObjectRef(pkt.caster().object_id());

	if (caster == nullptr)
		return false;

	{
		Protocol::S_MONTAGE montagePkt;
		*montagePkt.mutable_caster() = *caster->GetObjectInfo();
		montagePkt.set_isstop(pkt.isstop());
		montagePkt.set_id(pkt.id());
		montagePkt.set_section_num(pkt.section_num());
		montagePkt.set_scalable(false);

		return HandleMontage(montagePkt);
	}
}

void Room::HandleDead(GameObjectRef gameObject)
{
	if (RemoveGameObject(gameObject))
	{
		Protocol::S_DESPAWN despawnPkt;

		despawnPkt.add_info()->CopyFrom(*gameObject->GetObjectInfo());

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer);
	}
	
}

bool Room::HandleSkill(GameObjectRef caster, uint64 skillid, Vector skillActorPos, float yaw, float damage)
{
	if (caster == nullptr)
		return false;

	SkillActorRef skillActor = ObjectUtils::CreateSkillActor(caster, GetRoomRef());
	Protocol::ObjectInfo* info = skillActor->GetObjectInfo();
	Protocol::ObjectInfo* casterInfo = caster->GetObjectInfo();
	info->set_x(skillActorPos.x);
	info->set_y(skillActorPos.y);
	info->set_z(GetValidHeight(GetGridPos(skillActorPos)));
	info->set_yaw(yaw);
	skillActor->SetCollisionBySkillId(casterInfo->castertype(), skillid, damage);

	SpawnSkill(skillActor);

	// TODO : S_SKILL 구조 수정
	{
		Protocol::S_SKILL skillPkt;
		*skillPkt.mutable_caster() = *caster->GetObjectInfo();
		*skillPkt.mutable_skillactor() = *skillActor->GetObjectInfo();
		skillPkt.set_skillid(skillid);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(skillPkt);
		Broadcast(sendBuffer);
	}
	return true;
}

bool Room::HandleMontage(Protocol::S_MONTAGE pkt)
{
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Broadcast(sendBuffer);
	return false;
}

bool Room::HandleSpawn(Protocol::S_SPAWN pkt)
{
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Broadcast(sendBuffer);
	return false;
}

bool Room::HandleDespawn(Protocol::S_DESPAWN pkt, bool remove)
{
	if (remove)
	{
		for (auto& item : pkt.info())
		{
			RemoveGameObject(GetGameObjectRef(item.object_id()));
		}
	}
	
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Broadcast(sendBuffer);
	return false;
}

StructureRef Room::SpawnStructure(Vector pos)
{
	StructureRef structure = ObjectUtils::CreateStructure(GetRoomRef());
	
	AddGameObject(structure);

	structure->GetObjectInfo()->set_x(pos.x);
	structure->GetObjectInfo()->set_y(pos.y);
	structure->GetObjectInfo()->set_z(100.f);
	structure->GetObjectInfo()->set_yaw(0.f);

	return structure;
}

void Room::EnterGame(PlayerRef player)
{
	const uint64 id = player->GetObjectInfo()->object_id();

	AddGameObject(player);
	SetObjectToRandomPos(player);
}

void Room::SpawnSkill(SkillActorRef skillActor)
{
	AddGameObject(skillActor);
}

void Room::LeaveGame(PlayerRef player)
{
	const uint64 id = player->GetObjectInfo()->object_id();

	RemoveGameObject(player);

}

bool Room::AddGameObject(GameObjectRef gameObject)
{
	if (gameObject == nullptr)
		return false;

	const uint64 id = gameObject->GetObjectInfo()->object_id();

	Protocol::CasterType type = gameObject->GetObjectInfo()->castertype();

	if (_actors[type].find(id) == _actors[type].end())
	{
		_actors[type][id] = gameObject;
		return true;
	}

	return false;
}


bool Room::RemoveGameObject(GameObjectRef gameObject)
{
	if (gameObject == nullptr)
		return false;

	const uint64 id = gameObject->GetObjectInfo()->object_id();

	Protocol::CasterType type = gameObject->GetObjectInfo()->castertype();
	
	if (_actors[type].find(id) != _actors[type].end())
	{
		_actors[type].erase(id);
		return true;
	}
		
	return false;
}

vector<PlayerRef> Room::GetPlayers()
{
	vector<PlayerRef> players;

	for (auto& item : _actors[Protocol::CASTER_TYPE_WARRIOR])
	{
		players.push_back(static_pointer_cast<Player>(item.second));
	}

	for (auto& item : _actors[Protocol::CASTER_TYPE_ARCHER])
	{
		players.push_back(static_pointer_cast<Player>(item.second));
	}

	for (auto& item : _actors[Protocol::CASTER_TYPE_MAGE])
	{
		players.push_back(static_pointer_cast<Player>(item.second));
	}

	return players;
	
}

vector<GameObjectRef> Room::GetGameObjects()
{
	vector<GameObjectRef> results;

	for (auto& v : _actors)
	{
		for (auto& item : v)
		{
			results.push_back(item.second);
		}
	}

	return results;
}

void Room::SetObjectToRandomPos(GameObjectRef player)
{
	while (true)
	{
		int32 x = Utils::GetRandom(0, 100);
		int32 y = Utils::GetRandom(0, 100);

		if (IsWalkableAtPos({ x, y }))
		{
			Vector pos = GetPosition({ x,y });
			player->GetObjectInfo()->set_x(pos.x);
			player->GetObjectInfo()->set_y(pos.y);
			player->GetObjectInfo()->set_z(GetValidHeight(GetGridPos(pos)) + 80.f);
			player->GetObjectInfo()->set_yaw(Utils::GetRandom(0.f, 100.f));
			break;
		}
	}
	
	
}

void Room::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	auto v = GetPlayers();

	for (auto& item : v)
	{
		PlayerRef player = item;
		
		if (player->GetObjectInfo()->object_id() == exceptId)
			continue;
		
		if (GameSessionRef gameSession = player->GetSessionRef())
		{
			gameSession->Send(sendBuffer);
		}
	}

}

GameObjectRef Room::FindClosestPlayer(Vector pos, float maxDistance)
{
	GameObjectRef result = nullptr;
	auto v = GetPlayers();

	for (auto& item : v)
	{
		
		float dist = pos.Distance(item->GetPos());

		if (maxDistance > dist)
		{
			result = item;
			maxDistance = dist;
		}
		
	}

	return result;
}

VectorInt Room::GetGridPos(Vector pos)
{
	if (_map == nullptr)
		return { 0,0 };
	
	VectorInt result;

	if (!_map->ConvertToGridPos(pos, result))
	{
		return { -1, -1 };
	}
	else
	{
		return result;
	}
}

Vector Room::GetPosition(VectorInt gridPos)
{
	return _map->GetNode(gridPos).pos;
}

float Room::GetValidHeight(VectorInt gridPos)
{
	if (!_map->IsValidAtGridPos(gridPos))
		return 100.f;

	return _map->GetNode(gridPos).z;
}

bool Room::FindPath(Vector start, Vector end, vector<VectorInt>& path, int32 maxDepth)
{
	priority_queue<PQNode, vector<PQNode>, greater<PQNode>> pq;

	VectorInt gridStart = GetGridPos(start);
	VectorInt gridEnd = GetGridPos(end);

	map<VectorInt, int32> best;
	map<VectorInt, VectorInt> parent;

	{
		int32 cost = abs(gridEnd.x - gridStart.x) + abs(gridEnd.y - gridStart.y);
		pq.push(PQNode(0, cost, gridStart));
		best[gridStart] = cost;
		parent[gridStart] = gridStart;
	}

	// 북 , 북동 , 동 ...
	VectorInt d[8] = { {0,1}, {1, 1}, {1, 0}, {1, -1}, {0,-1}, {-1,-1}, {-1, 0}, {-1, 1} };

	bool found = false;

	while (!pq.empty())
	{
		PQNode node = pq.top();
		int32 curDepth = node.g;
		int32 curCost = node.h;
		pq.pop();

		if (best[node.pos] < curCost + curDepth)
		{
			continue;
		}

		if (node.pos == gridEnd)
		{
			found = true;
			break;
		}

		for (int32 dir = 0; dir < 8; dir++)
		{
			VectorInt nextPos = node.pos + d[dir];
			if (CanGoByDirection(node.pos, dir) == false) continue;

			int32 depth = curDepth + 1;
			if (depth >= maxDepth) continue;

			int32 cost = abs(gridEnd.y - nextPos.y) + abs(gridEnd.x - nextPos.x);
			int32 bestCost = best[nextPos];

			if (bestCost != 0)
			{
				if (bestCost <= depth + cost) continue;
			}

			best[nextPos] = depth + cost;
			pq.push(PQNode(depth, cost, nextPos));
			parent[nextPos] = node.pos;
		}
	}

	if (found == false)
	{
		float bestScore = FLT_MAX;

		for (auto& item : best)
		{
			VectorInt pos = item.first;
			int32 score = item.second;

			if (bestScore == score)
			{
				int32 dist1 = abs(gridEnd.x - gridStart.x) + abs(gridEnd.y - gridStart.y);
				int32 dist2 = abs(pos.x - gridStart.x) + abs(pos.y - gridStart.y);

				if (dist1 > dist2) gridEnd = pos;
			}
			if (bestScore > score)
			{
				gridEnd = pos;
				bestScore = score;
			}
		}
	}

	path.clear();
	VectorInt pos = gridEnd;
	while (true)
	{
		path.push_back(pos);

		if (pos == parent[pos]) break;

		pos = parent[pos];
	}
	std::reverse(path.begin(), path.end());
	return true;

	return true;
}

bool Room::CanGoByDirection(VectorInt current, int32 dir, bool checkCollision, Collision* collision)
{
	if (!_map->IsValidToDirection(current, dir))
		return false;

	if (!checkCollision)
		return true;

	VectorInt d[8] =
	{ {0,1}, {1, 1}, {1, 0}, {1, -1}, {0,-1}, {-1,-1}, {-1, 0}, {-1, 1} };

	Vector pos = collision->GetPos();
	collision->SetPos(GetPosition(current + d[dir]));
	bool result = !CheckCollisionInMap(collision);
	collision->SetPos(pos);

	return result;
}

bool Room::CanGoByVector(Collision* collision, Vector moveVector)
{
	Vector currentPos = collision->GetPos();
	
	Vector movedPos = currentPos + moveVector;
	
	if (!GoMoveVector(currentPos, movedPos))
		return false;

	vector<GameObjectRef> collideObjects;

	collision->SetPos(currentPos + moveVector);
	bool result = CheckCollisionInQuadTree(collision, collideObjects);
	collision->SetPos(currentPos);

	return !result;
}

bool Room::CheckCollisionInMap(Collision* collision)
{
	Bound box = collision->GetBound();

	VectorInt topLeft = GetGridPos(box.topLeft);
	VectorInt bottomRight = GetGridPos(box.BottomRight);

	if (!_map->IsValidAtGridPos(topLeft) || !_map->IsValidAtGridPos(bottomRight))
		return true;

	for (int32 y = bottomRight.y; y <= topLeft.y; y++)
	{
		for (int32 x = topLeft.x; x <= bottomRight.x; x++)
		{
			if (_map->GetNode({ x,y }).value == EGT_Blocked)
				return true;
		}
	}

	return false;
}

bool Room::CheckCollisionInQuadTree(Collision* collision, vector<GameObjectRef>& collideObjects)
{
	auto v1 = _tree->Check(collision);
	auto v2 = _updatedTree->Check(collision);

	collideObjects.insert(collideObjects.begin(), v1.begin(), v1.end());
	collideObjects.insert(collideObjects.begin(), v2.begin(), v2.end());

	for (auto gameObject : v1)
	{
		if (!gameObject->IsUpdated())
			return true;
	}

	if (v2.size() > 0)
		return true;

	return false;
}

bool Room::IsWalkableAtPos(VectorInt gridPos)
{
	return _map->IsWalkableAtGridPos(gridPos);
}

bool Room::GoMoveVector(Vector currentPos, Vector moveVector)
{
	Vector gridSize = _map->GetGridSize();
	
	vector<VectorInt> path = Bresenham(currentPos, currentPos + moveVector);
	
	//float incl = moveVector.y / moveVector.x;

	//Vector result = currentPos;

	for (int32 i = 0; i < path.size(); i++)
	{
		VectorInt gridPos = path[i];
		//Vector pos = GetPosition(gridPos);

		if (!IsWalkableAtPos(gridPos))
			return false;

		/*if (abs(pos.x - currentPos.x) * abs(incl) < abs(pos.y - currentPos.y))
		{
			result.x = pos.x;
			result.y = currentPos.y + incl * (pos.x - currentPos.x);
		}
		else
		{
			result.x = currentPos.x + (pos.y - currentPos.y) / incl;
			result.y = pos.y;
		}

		if (i == path.size() - 1)
		{
			result = currentPos + moveVector;
		}*/
	}

	return true;
}

vector<VectorInt> Room::Bresenham(Vector start, Vector end)
{
	float incl = (end - start).y / (end - start).x;
	Vector gridSize = _map->GetGridSize();
	bool isReverse = false;

	if (incl < 0)
	{
		Vector temp = start;
		start = end;
		end = temp;
		isReverse = true;
	}

	vector<VectorInt> result;

	float x = start.x;
	float y = start.y;

	float W = (end.x - start.x);
	float H = (end.y - start.y);

	// 초기화
	y = GetPosition(GetGridPos({ x,y })).y;
	y += _map->GetGridSize().y / 2.f;

	float F = 2 * H * gridSize.x - W * gridSize.y;
	float F1 = 2.f * H * gridSize.x;
	float F2 = 2.f * (H * gridSize.x - W * gridSize.y);

	while (x < end.x)
	{
		// 점 넣기
		VectorInt gridPos = GetGridPos({ x, y });
		result.push_back(gridPos);

		if (F < 0)
		{
			F += F1;
			float tx = min(x + gridSize.x, end.x);
			x = tx;
		}
		else
		{
			y += gridSize.y;
			F += F2;
		}

	}

	if (isReverse)
	{
		reverse(result.begin(), result.end());
	}

	return result;
}

RoomRef Room::GetRoomRef()
{
	return static_pointer_cast<Room>(shared_from_this());
}

GameObjectRef Room::GetGameObjectRef(uint64 id)
{
	for (auto actors : _actors)
	{
		if (actors.find(id) != actors.end())
			return actors[id];
	}

	return nullptr;
}

