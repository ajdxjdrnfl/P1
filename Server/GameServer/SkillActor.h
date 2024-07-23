#pragma once
#include "GameObject.h"
class SkillActor :
    public GameObject
{
    using Super = GameObject;
public:
    SkillActor(GameObjectRef caster, RoomRef room);
    virtual ~SkillActor();

    virtual void Init() override;
    virtual void Update(float deltaTime) override;

private:
    weak_ptr<GameObject> _caster;

    // SkillInfo에 대한 멤버변수
};

