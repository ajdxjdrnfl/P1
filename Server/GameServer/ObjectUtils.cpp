#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "Enemy.h"
#include "SkillActor.h"
#include "Room.h"
#include "GameSession.h"
#include "Boss.h"
#include "Structure.h"

atomic<int64> ObjectUtils::s_idGenerator = 1;

PlayerRef ObjectUtils::CreatePlayer(GameSessionRef session, RoomRef room)
{
	// ID »ý¼º±â
	const int64 newId = s_idGenerator.fetch_add(1);

	PlayerRef player = make_shared<Player>(session, room);
	player->GetObjectInfo()->set_object_id(newId);

	player->Init();

	return player;
}

EnemyRef ObjectUtils::CreateEnemy(RoomRef room)
{
	const int64 newId = s_idGenerator.fetch_add(1);

	EnemyRef enemy = make_shared<Enemy>(room);
	enemy->GetObjectInfo()->set_object_id(newId);

	enemy->Init();

	return enemy;
}

SkillActorRef ObjectUtils::CreateSkillActor(GameObjectRef caster, RoomRef room)
{

	const int64 newId = s_idGenerator.fetch_add(1);

	SkillActorRef skillActor = make_shared<SkillActor>(caster, room);
	skillActor->GetObjectInfo()->set_object_id(newId);

	skillActor->Init();
	
	return skillActor;
}

BossRef ObjectUtils::CreateBoss(RoomRef room)
{
	const int64 newId = s_idGenerator.fetch_add(1);

	BossRef boss = make_shared<Boss>(room);
	boss->GetObjectInfo()->set_object_id(newId);

	boss->Init();

	return boss;
}

StructureRef ObjectUtils::CreateStructure(RoomRef room)
{
	const int64 newId = s_idGenerator.fetch_add(1);

	StructureRef structure = make_shared<Structure>(room);
	structure->GetObjectInfo()->set_object_id(newId);

	structure->Init();

	return structure;
}
