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
    virtual void TickIdle(float deltaTime) override;
    virtual void TickRun(float deltaTime) override;
    virtual void TickSkill(float deltaTime) override;
    virtual void TickStun(float deltaTime) override;
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
    float _attackRange = 300.f;

    bool _isGimmik = false;
    
    float _moveSpeed = 300.f;
    float _rushSpeed = 400.f;

    float _nextStepCooldown = 1.f;
    float _prevStepElapsedTime = 0.f;
    float _attackCooldown = 2.f;

    // update time
    float _updatePacketCooldown = 1.f;
    float _elapsedPacket = 0.f;

    Vector _targetPos;
};

