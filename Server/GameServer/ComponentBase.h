#pragma once

// 스킬 타이머
// 2D 콜리전(충돌처리)
// 보스 기믹수행
// AI 움직임

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

