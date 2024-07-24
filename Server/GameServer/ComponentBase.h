#pragma once

// 스킬 타이머
// 2D 콜리전(충돌처리)
// 보스 기믹수행 AI
// AI 움직임

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

	GameObjectRef GetOwner() { return _owner; }
	void SetOwner(GameObjectRef owner) { _owner = owner; }

	EComponentType GetComponentType() { return _componentType; }
	
private:
	GameObjectRef _owner;
	EComponentType _componentType;
};

