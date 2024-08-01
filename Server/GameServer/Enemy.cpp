#include "pch.h"
#include "Enemy.h"
#include "Room.h"
#include "Collision.h"
#include "Collider.h"
#include "ComponentBase.h"


Enemy::Enemy(RoomRef room) : GameObject(room)
{
	_objectInfo->set_hp(50.f);
	_objectInfo->set_max_hp(50.f);
	_objectInfo->set_castertype(Protocol::CASTER_TYPE_MOB);
}

Enemy::~Enemy()
{
}

void Enemy::Update(float deltaTime)
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	randomWalkDuration += deltaTime;

	if (randomWalkDuration >= randomWalkCoolDonw)
	{
		//RandomWalk();
		randomWalkDuration = 0.f;
	}
	
	Super::Update(deltaTime);
}

void Enemy::Init()
{
	Collision* collision = new Collision();
	ColliderCircle* collider = new ColliderCircle();
	collider->_radius = 38.f;
	collision->SetCollider(collider);

	AddComponent(collision);
	Super::Init();
}

void Enemy::RandomWalk()
{
	Vector currentPos = Vector(_objectInfo->x(), _objectInfo->y());
	VectorInt d[9] =
	{
		{-1,-1},
		{-1,1},
		{1,-1},
		{1,1},
		{0,1},
		{1,0},
		{-1,0},
		{0,-1},
		{0,0}
	};

	float randomX = Utils::GetRandom(0.f, 50.f);
	float randomY = Utils::GetRandom(0.f, 50.f);
	
	uint8 randomIndex = Utils::GetRandom(0, 8);

	Vector nextPos = Vector(currentPos.x + randomX * d[randomIndex].x, currentPos.y + randomY * d[randomIndex].y);
	
	_objectInfo->set_x(nextPos.x);
	_objectInfo->set_y(nextPos.y);
	_objectInfo->set_state(Protocol::MOVE_STATE_RUN);

	_dirtyFlag = true;
}


void Enemy::BroadcastUpdate()
{
	Super::BroadcastUpdate();
}

void Enemy::TakeDamage(GameObjectRef instigator, Protocol::DamageType damageType, float damage)
{
	Super::TakeDamage(instigator, damageType, damage);
}