#pragma once
#include "GameObject.h"
class Boss :
    public GameObject
{
    using Super = GameObject;
public:
    Boss(RoomRef room);
    virtual ~Boss();

    virtual void Update(float deltaTime);
    virtual void Init() override;

    virtual void TakeDamage(GameObjectRef instigator, Protocol::DamageType damageType, float damage) override;

protected:
    virtual void TickIdle() override;
    virtual void TickRun() override;
    virtual void TickSkill() override;
    virtual void TickStun() override;
private:
    PlayerRef FindClosestTarget();

    // Default Skill
private:
    void DefaultAttack();
    void Rush(GameObjectRef target);
    void DotSkill();

    // Gimmick
private:
    

private:
    void MoveToTarget(GameObjectRef target);
    
    
private:
    weak_ptr<class Player> _target;
    float _attackRange = 1000.f;

    bool _isGimmik = false;
    
    float _moveSpeed = 100.f;
    float _rushSpeed = 200.f;

    float _attackCooldown = 2.f;

    Vector _targetPos;
};

