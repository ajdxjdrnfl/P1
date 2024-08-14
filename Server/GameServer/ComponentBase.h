#pragma once

// ��ų Ÿ�̸�
// 2D �ݸ���(�浹ó��)
// ���� ��ͼ��� AI
// AI ������

class ComponentBase
{
public:
	ComponentBase(EComponentType type) : _componentType(type)
	{

	}
	virtual ~ComponentBase()
	{

	}

	virtual void Init() {};
	virtual void Update(float deltaTime) {};

	GameObjectRef GetOwner() { return _owner.lock(); }
	void SetOwner(GameObjectRef owner) { _owner = owner; }

	EComponentType GetComponentType() { return _componentType; }
	
private:
	weak_ptr<GameObject> _owner;
	EComponentType _componentType;
};

