#include "pch.h"
#include "GameObject.h"
#include "ComponentBase.h"

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

	BroadcastUpdate();
	_dirtyFlag = false;
}

void GameObject::SetObjectInfo(Protocol::ObjectInfo objectInfo, bool dirtyFlag)
{
	_objectInfo->CopyFrom(objectInfo);
	_dirtyFlag = dirtyFlag;
}

void GameObject::BroadcastUpdate()
{
	if (_dirtyFlag)
	{
		// TODO : Send update packet
	}
}
void GameObject::TakeDamage(GameObjectRef instigator, float damage)
{
	// TODO : »ç¸Á Ã³¸®
	float currentHp = _objectInfo->hp();
	float hp = max(0, currentHp - damage);
	_objectInfo->set_hp(hp);
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
