#include "pch.h"
#include "GameObject.h"
#include "ComponentBase.h"
#include "Room.h"

GameObject::GameObject(RoomRef room)
{
	_objectInfo = new Protocol::ObjectInfo();
	_room = room;

	for (auto& item : _components)
	{
		item = nullptr;
	}
}

GameObject::~GameObject()
{
	delete _objectInfo;
	_objectInfo = nullptr;

	for (int32 i = 0; i < _components.size(); i++)
	{
		delete _components[i];
		_components[i] = nullptr;
	}
}


void GameObject::Init()
{

}

void GameObject::Update(float deltaTime)
{

	for (auto& item : _components)
	{
		if (item != nullptr)
			item->Update(deltaTime);
	}
	
	switch (GetState())
	{
	case Protocol::MOVE_STATE_IDLE:
		TickIdle();
		break;

	case Protocol::MOVE_STATE_SKILL:
		TickSkill();
		break;

	case Protocol::MOVE_STATE_RUN:
		TickRun();
		break;
	
	case Protocol::MOVE_STATE_STUN:
		TickStun();
		break;

	case Protocol::MOVE_STATE_DEAD:
		
		break;
	}

	TickDot(deltaTime);

	float currentHp = _objectInfo->hp();

	if (currentHp <= 0)
	{
		SetState(Protocol::MOVE_STATE_IDLE, true);
	}
	
	BroadcastUpdate();
	_dirtyFlag = false;
	
}

void GameObject::SetObjectInfo(Protocol::ObjectInfo objectInfo, bool dirtyFlag)
{
	SetState(objectInfo.state(), dirtyFlag);
	_objectInfo->CopyFrom(objectInfo);
	_dirtyFlag = dirtyFlag;
}

void GameObject::SetState(Protocol::MoveState state, bool dirtyFlag)
{
	_objectInfo->set_state(state);
	_dirtyFlag = dirtyFlag;
}

void GameObject::BroadcastUpdate()
{
	if (_dirtyFlag)
	{
		RoomRef room = GetRoomRef();

		Protocol::S_MOVE updatePkt;

		*updatePkt.mutable_info() = *GetObjectInfo();

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(updatePkt);
		room->Broadcast(sendBuffer);
	}
}
void GameObject::TickDot(float deltaTime)
{
	queue<vector<float>> tempQueue;

	while (!_dotDamages.empty())
	{
		vector<float> dotDamage = _dotDamages.front();
		_dotDamages.pop();

		{
			float totalTime = dotDamage[0];
			float duration = dotDamage[1];
			float damage = dotDamage[2];

			duration += deltaTime;
			totalTime -= deltaTime;

			if (duration >= 1.f)
			{
				duration -= 1.f;
				float currentHp = _objectInfo->hp();
				float hp = max(0, currentHp - damage);
				_objectInfo->set_hp(hp);
				_dirtyFlag = true;
			}

			if (totalTime > 0.f)
			{
				tempQueue.push({totalTime, duration, damage});
			}
		}
	}

	_dotDamages = tempQueue;
}

void GameObject::TakeDamage(GameObjectRef instigator, Protocol::DamageType damageType, float damage)
{
	// TODO : 荤噶 贸府
	
	switch (damageType)
	{
	case Protocol::DAMAGE_TYPE_NORMAL:
	{
		float currentHp = _objectInfo->hp();
		float hp = max(0, currentHp - damage);
		_objectInfo->set_hp(hp);
	}
		break;
	case Protocol::DAMAGE_TYPE_DOT:
	{
		_dotDamages.push({5.f, 0.f, damage});
	}
		break;
	case Protocol::DAMAGE_TYPE_BUFF:
	{
		// TODO : 滚橇 贸府
	}
		break;
	}


}

ComponentBase* GameObject::GetComponent(EComponentType type)
{
	return _components[type];
}

void GameObject::AddComponent(ComponentBase* component)
{
	assert(_components[component->GetComponentType()] == nullptr);

	component->Init();
	_components[component->GetComponentType()] = component;
	component->SetOwner(shared_from_this());
}
