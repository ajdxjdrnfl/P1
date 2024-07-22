#pragma once
class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject(RoomRef room);
	virtual ~GameObject();

	virtual void Init();

public:
	void SetObjectInfo(Protocol::ObjectInfo objectInfo);
	Protocol::ObjectInfo* GetObjectInfo() { return _objectInfo; }

	RoomRef GetRoomRef() { return _room.lock(); }
protected:
	Protocol::ObjectInfo* _objectInfo;
	weak_ptr<Room> _room;
};

