#include "pch.h"
#include "Raid.h"
#include "GameObject.h"
#include "Enemy.h"
#include "Boss.h"
#include "ObjectUtils.h"
#include "Map.h"

RoomRef GRoom = make_shared<Raid>();

Raid::Raid()
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
			enemy->GetObjectInfo()->set_z(100.f);
			enemy->GetObjectInfo()->set_yaw(yaw);

			_phaseRequirements[v[i].phaseNumber].push_back(enemy);
		}
		else if (type == Protocol::CASTER_TYPE_BOSS)
		{
			BossRef boss = ObjectUtils::CreateBoss(GetRoomRef(), v[i].phaseNumber);
			AddGameObject(boss);
			boss->GetObjectInfo()->set_x(pos.x);
			boss->GetObjectInfo()->set_y(pos.y);
			boss->GetObjectInfo()->set_z(100.f);
			boss->GetObjectInfo()->set_yaw(yaw);

			_phaseRequirements[v[i].phaseNumber].push_back(boss);
		}
	}
	
}

void Raid::Update(float deltaTime)
{
	Super::Update(deltaTime);
	
	if (ProceedNextPhase())
	{
		HandleNextPhase();
		_currentPhase += 1;
	}
}

bool Raid::HandleNextPhase()
{
	if (_currentPhase > _phaseCount)
		return false;

	Protocol::S_PHASE pkt;
	pkt.set_phasenumber(_currentPhase + 1);

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Broadcast(sendBuffer);

	return true;
}

bool Raid::ProceedNextPhase()
{
	for (auto& item : _phaseRequirements[_currentPhase])
	{
		GameObjectRef gameObject = item.lock();

		if (gameObject != nullptr && gameObject->GetState() != Protocol::MOVE_STATE_DEAD)
			return false;
	}
	return true;
}
