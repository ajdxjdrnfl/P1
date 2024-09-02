#include "pch.h"
#include "Raid.h"
#include "GameObject.h"
#include "Enemy.h"
#include "Boss.h"
#include "ObjectUtils.h"
#include "Map.h"

RaidRef GRaid = make_shared<Raid>(2);

Raid::Raid(uint64 roomId) : Super(ERoomType::ERT_RAID, roomId)
{
	_mapName = "Raid";
}

Raid::~Raid()
{

}

void Raid::Init()
{
	Super::Init();

	_phaseRequirements = vector<vector<weak_ptr<GameObject>>>(_phaseCount + 1);

	// phasenumber로 몬스터들 관리

	auto& v = _map->GetEnemies();

	for (int32 i = 0; i < v.size(); i++)
	{
		Vector pos = { v[i].Location.x, v[i].Location.y };
		float yaw = v[i].yaw;
		Protocol::CasterType type = v[i].casterType;

		if (type == Protocol::CASTER_TYPE_MOB)
		{
			EnemyRef enemy = ObjectUtils::CreateEnemy(GetRoomRef(), v[i].phaseNumber);
			AddGameObject(enemy);
			enemy->GetObjectInfo()->set_x(pos.x);
			enemy->GetObjectInfo()->set_y(pos.y);
			enemy->GetObjectInfo()->set_z(GetValidHeight(GetGridPos(pos)));
			enemy->GetObjectInfo()->set_yaw(yaw);
			enemy->SetStartPoint(pos);
			_phaseRequirements[v[i].phaseNumber].push_back(enemy);
		}
		else if (type == Protocol::CASTER_TYPE_BOSS)
		{
			boss = ObjectUtils::CreateBoss(GetRoomRef(), v[i].phaseNumber);
		
			boss->GetObjectInfo()->set_x(pos.x);
			boss->GetObjectInfo()->set_y(pos.y);
			boss->GetObjectInfo()->set_z(GetValidHeight(GetGridPos(pos)) + 100.f);
			boss->GetObjectInfo()->set_yaw(yaw);
			boss->SetStartPoint(pos);
			_phaseRequirements[v[i].phaseNumber].push_back(boss);
		}
	}
	
}

void Raid::Update(float deltaTime)
{
	if (ProceedNextPhase())
	{
		HandleNextPhase();
	}

	if (_currentPhase == _phaseCount && _bossEmergency)
	{
		if (_bossExist == false)
		{
			AddGameObject(boss);
			Protocol::S_SPAWN spawnPkt;
			spawnPkt.add_info()->CopyFrom(*boss->GetObjectInfo());

			DoAsync(&Room::HandleSpawn, spawnPkt);
			_bossExist = true;
		}
		
	}
	Super::Update(deltaTime);
	
}

bool Raid::HandleNextPhase()
{
	if (_currentPhase >= _phaseCount)
		return false;

	_currentPhase += 1;

	if (_currentPhase == _phaseCount)
	{
		AddGameObject(boss);
		Protocol::S_SPAWN spawnPkt;
		spawnPkt.add_info()->CopyFrom(*boss->GetObjectInfo());

		DoAsync(&Room::HandleSpawn, spawnPkt);
	}

	return HandlePhaseNumber(_currentPhase);
}

bool Raid::HandlePhaseNumber(uint64 currentPhase)
{
	Protocol::S_PHASE pkt;
	pkt.set_phasenumber(currentPhase);

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Broadcast(sendBuffer);
	return true;
}

bool Raid::ProceedNextPhase()
{
	if (_currentPhase > _phaseCount)
		return false;

	for (auto& item : _phaseRequirements[_currentPhase])
	{
		GameObjectRef gameObject = item.lock();

		if (gameObject != nullptr && gameObject->GetState() != Protocol::MOVE_STATE_DEAD)
			return false;
	}

	return true;
}
