#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(RoomRef room)
{
	_objectInfo = new Protocol::ObjectInfo();
	_room = room;
}

GameObject::~GameObject()
{
	delete _objectInfo;
	_objectInfo = nullptr;
}

void GameObject::SetObjectInfo(Protocol::ObjectInfo objectInfo)
{
	_objectInfo->CopyFrom(objectInfo);
}

void GameObject::Init()
{

}