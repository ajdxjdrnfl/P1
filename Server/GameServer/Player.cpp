#include "pch.h"
#include "Player.h"
#include "Collision.h"
#include "Collider.h"
#include "ComponentBase.h"
#include "Room.h"

Player::Player(GameSessionRef session, RoomRef room, Protocol::CasterType casterType) : Super(room), _session(session)
{
	_objectInfo->set_hp(100.f);
	_objectInfo->set_max_hp(100.f);
	_objectInfo->set_stamina(100.f);
	_objectInfo->set_max_stamina(100.f);
	_objectInfo->set_castertype(casterType);
	_objectInfo->set_speed(400.f);

	_targetInfo = new Protocol::ObjectInfo();
}

Player::~Player()
{
	delete _targetInfo;
	_targetInfo = nullptr;
}

void Player::Init()
{
	// SkillTimer


	// TODO : Collision 초기 
	Collision* collision = new Collision();
	ColliderCircle* collider = new ColliderCircle();
	collider->_radius = 38.f;
	collision->SetCollider(collider);

	AddComponent(collision);
	Super::Init();
}

void Player::Update(float deltaTime)
{
	Super::Update(deltaTime);
}

bool Player::HandleMovePacket(Protocol::C_MOVE pkt)
{
	RoomRef room = GetRoomRef();
	if (room == nullptr)
		return false;

	Vector clientPos = Vector(pkt.info().x(), pkt.info().y());
	Protocol::ObjectInfo targetInfo = pkt.targetinfo();

	// TODO : 현재 위치에서 갈만한 곳인지 검증
	_objectInfo->CopyFrom(pkt.info());
	ClearTargetPath();

	if (targetInfo.state() == Protocol::MOVE_STATE_RUN)
	{
		SetTargetInfo(pkt.targetinfo());
		Vector targetPos = Vector(pkt.info().x(), pkt.info().y());

		// 갈 수 있는 곳인지 검증
		vector<VectorInt> path;
		if (!room->FindPath(clientPos, targetPos, path, 30))
			return false;

		AddTargetPath(path);
		PopTargetPath();

		return true;
	}
	else if (targetInfo.state() == Protocol::MOVE_STATE_JUMP)
	{
		_objectInfo->set_state(Protocol::MOVE_STATE_JUMP);
		return true;
	}
	else if (targetInfo.state() == Protocol::MOVE_STATE_IDLE)
	{
		_objectInfo->set_state(Protocol::MOVE_STATE_IDLE);
		return true;
	}
	else if (targetInfo.state() == Protocol::MOVE_STATE_STUN)
	{
		return true;
	}
	else if (targetInfo.state() == Protocol::MOVE_STATE_STOP)
	{
		// stop시에는 바로 IDLE;
		_objectInfo->set_state(Protocol::MOVE_STATE_STOP);
		return true;
	}
	else if (targetInfo.state() == Protocol::MOVE_STATE_SKILL)
	{
		_objectInfo->set_state(Protocol::MOVE_STATE_SKILL);
		return true;
	}

	return false;
}

void Player::SetTargetInfo(Protocol::ObjectInfo targetInfo)
{
	_targetInfo->CopyFrom(targetInfo);
}

void Player::ClearTargetPath()
{
	while (!_targetPath.empty())
	{
		_targetPath.pop();
	}
}

void Player::AddTargetPath(vector<VectorInt>& path)
{
	for(int32 i=0; i<path.size(); i++)
		_targetPath.emplace(path[i]);
}

bool Player::PopTargetPath()
{
	RoomRef room = GetRoomRef();

	if (room == nullptr)
		return false;

	if (_targetPath.empty())
		return false;

	VectorInt gridPos = _targetPath.front();
	_targetPath.pop();

	if (_targetPath.empty())
		_targetPos = Vector(_targetInfo->x(), _targetInfo->y());
	else
		_targetPos = room->GetPosition(gridPos);
}


void Player::TickIdle(float deltaTime)
{
	Super::TickIdle(deltaTime);
	

}

void Player::TickJump(float deltaTime)
{
	Super::TickJump(deltaTime);
	
	
}

void Player::TickRun(float deltaTime)
{
	Super::TickRun(deltaTime);

	if (GetState() != Protocol::MOVE_STATE_RUN)
		return;

	float dist = GetPos().Distance(_targetPos);
	// 충분히 가까울 때
	if (dist <= 10.f)
	{
		Protocol::ObjectInfo newInfo = *GetObjectInfo();
		newInfo.set_x(_targetPos.x);
		newInfo.set_y(_targetPos.y);

		SetObjectInfo(newInfo);

		if(!PopTargetPath())
			SetState(Protocol::MOVE_STATE_IDLE);

	}
	else
	{
		Vector moveVector = (_targetPos - GetPos()).Normalize();

		moveVector = moveVector * _objectInfo->speed() * deltaTime;

		Protocol::ObjectInfo newInfo = *GetObjectInfo();
		newInfo.set_x(GetPos().x + moveVector.x);
		newInfo.set_y(GetPos().y + moveVector.y);

		SetObjectInfo(newInfo);
	}
}

void Player::TickSkill(float deltaTime)
{
	Super::TickSkill(deltaTime);
}

void Player::TickStun(float deltaTime)
{
	Super::TickStun(deltaTime);
}

void Player::TickDead(float deltaTime)
{
	Super::TickDead(deltaTime);
}
