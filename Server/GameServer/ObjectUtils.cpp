#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "Enemy.h"
#include "SkillActor.h"
#include "Room.h"
#include "GameSession.h"

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
