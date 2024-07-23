#include "pch.h"
#include "GameObject.h"
#include "ComponentBase.h"

GameObject::GameObject(RoomRef room)
{
	_objectInfo = new Protocol::ObjectInfo();
	_room = room;
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
	for (int32 i = 0; i < _components.size(); i++)
	{
		_components[i]->Init();
	}
}

void GameObject::Update(float deltaTime)
{
	for (int32 i = 0; i < _components.size(); i++)
	{
		_components[i]->Update(deltaTime);
	}
	_dirtyFlag = false;

	BroadcastUpdate();
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

ComponentBase* GameObject::GetComponent()
{
	return nullptr;
}

void GameObject::AddComponent(ComponentBase* component)
{
	_components.push_back(component);
	component->SetOwner(shared_from_this());
}
