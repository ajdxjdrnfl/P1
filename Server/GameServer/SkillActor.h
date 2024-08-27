#pragma once
#include "GameObject.h"
#include "Skill.h"

class SkillActor :
    public GameObject
{
    using Super = GameObject;
public:
    SkillActor(GameObjectRef caster, RoomRef room);
    virtual ~SkillActor();

    virtual void Init() override;
    virtual void Update(float deltaTime) override;

    virtual void TickDead(float deltaTime) override;

public:
    SkillInfo& GetSkillInfo() { return _skill->GetSkillInfo(); }
public:
    // TEST Collider
    void SetCollisionBySkillId(Protocol::CasterType casterType, const uint64& id, float damage);
    float GetDamage() { return _damage; }
private:
    weak_ptr<GameObject> _caster;
    
    // Skill 府家胶
    Skill* _skill = nullptr;
    float _damage = 0.f;

    // 积己 饶 瘤加矫埃
    float _lifeTime = 0.f;
};

