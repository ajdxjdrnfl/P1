#pragma once

// ��ų Ÿ�̸�
// 2D �ݸ���(�浹ó��)
// ���� ��ͼ���
// AI ������

class ComponentBase
{
public:
	ComponentBase();
	virtual ~ComponentBase();

	virtual void Init() = 0;
	virtual void Update(float deltaTime) = 0;


	GameObjectRef GetOwner() { return _owner; }
	void SetOwner(GameObjectRef owner) { _owner = owner; }
	
private:
	GameObjectRef _owner;
};

