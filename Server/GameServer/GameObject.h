#pragma once
class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject(RoomRef room);
	virtual ~GameObject();

	virtual void Init();
	virtual void Update(float deltaTime);

	virtual void TakeDamage(GameObjectRef instigator, Protocol::DamageType damageType, float damage);

public:
	void SetObjectInfo(Protocol::ObjectInfo objectInfo, bool dirtyFlag = false);

	Protocol::MoveState GetState() { return _objectInfo->state(); }
	void SetState(Protocol::MoveState state, bool dirtyFlag = false);

	Protocol::ObjectInfo* GetObjectInfo() { return _objectInfo; }

	RoomRef GetRoomRef() { return _room.lock(); }

	Vector GetPos() { return Vector{ _objectInfo->x(), _objectInfo->y() }; }

protected:
	virtual void BroadcastUpdate();

protected:
	virtual void TickIdle(float deltaTime) { }
	virtual void TickRun(float deltaTime) { }
	virtual void TickSkill(float deltaTime) { }
	virtual void TickStun(float deltaTime) { }

	void TickDot(float deltaTime);

protected:
	Protocol::ObjectInfo* _objectInfo;
	weak_ptr<Room> _room;

	bool _dirtyFlag = false;
	
public:
	class ComponentBase* GetComponent(EComponentType type);
	void AddComponent(class ComponentBase* component);

private:
	array<ComponentBase*, EComponentType::ECT_MAX> _components;
	queue<vector<float>> _dotDamages; // 총 시간 - 지속시간 - 데미지
};

