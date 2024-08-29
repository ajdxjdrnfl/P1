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

    bool IsSpawned() { return _isSpawned; }
    void SpawnSkill();

public:
    SkillInfo& GetSkillInfo() { return _skill->GetSkillInfo(); }
    GameObjectRef GetCaster() { return _caster.lock(); }
    bool IsPredictable() { return _skill->GetSkillInfo().isPredictable; }
    float GetDelayTime() { return _skill->GetSkillInfo().delayTime; }

public:
    void SetCollisionBySkillId(Protocol::CasterType casterType, const uint64& id, float damage, bool counter = false);
    float GetDamage() { return _damage; }

private:
    weak_ptr<GameObject> _caster;
    
    // Skill ���ҽ�
    Skill* _skill = nullptr;
    float _damage = 0.f;

    // ������ ���Ͱ� ���忡 ���� �� ���ӽð�
    float _lifeTime = 0.f;
    
    // ������ ȣ�� �� ���ӽð�
    float _elapsedTime = 0.f;

    bool _isSpawned = false;
};

