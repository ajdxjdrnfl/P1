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

public:
    Protocol::SkillInfo* GetSkillInfo() { return _skillinfo; }
public:
    // TEST Collider
    void SetCollisionBySkillInfo(const Protocol::SkillInfo& skillinfo);

private:
    weak_ptr<GameObject> _caster;
    // TEST
    Protocol::SkillInfo* _skillinfo;
    // SKill ¸®¼Ò½º
    
};

