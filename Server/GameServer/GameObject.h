#pragma once
class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject(RoomRef room);
	virtual ~GameObject();

	virtual void Init();
	virtual void Update(float deltaTime);

	virtual void TakeDamage(GameObjectRef instigator, float damage);

public:
	void SetObjectInfo(Protocol::ObjectInfo objectInfo, bool dirtyFlag = false);
	Protocol::ObjectInfo* GetObjectInfo() { return _objectInfo; }

	RoomRef GetRoomRef() { return _room.lock(); }

	Vector GetPos() { return Vector{ _objectInfo->x(), _objectInfo->y() }; }

protected:
	virtual void BroadcastUpdate();

protected:
	Protocol::ObjectInfo* _objectInfo;
	weak_ptr<Room> _room;

	bool _dirtyFlag = false;
	
public:
	class ComponentBase* GetComponent(EComponentType type);
	void AddComponent(class ComponentBase* component);

private:
	array<ComponentBase*, EComponentType::ECT_MAX> _components;
};

