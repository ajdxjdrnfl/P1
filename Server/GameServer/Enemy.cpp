#include "pch.h"
#include "Enemy.h"
#include "Room.h"

Enemy::Enemy(RoomRef room) : GameObject(room)
{

}

Enemy::~Enemy()
{
}

void Enemy::Update(float deltaTime)
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return;

	

	Super::Update(deltaTime);
}

void Enemy::Init()
{
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

	Vector nextPos = Vector(currentPos.x * randomX, currentPos.y * randomY);
	
	_objectInfo->set_x(nextPos.x);
	_objectInfo->set_y(nextPos.y);
	_objectInfo->set_state(Protocol::MOVE_STATE_RUN);

	_dirtyFlag = true;
}


void Enemy::BroadcastUpdate()
{
	if (!_dirtyFlag)
		return;

	Protocol::S_MOVE pkt;
	pkt.CopyFrom(*GetObjectInfo());

}