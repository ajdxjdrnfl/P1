#pragma once
#include "GameObject.h"
class Enemy :
    public GameObject
{
    using Super = GameObject;
public:
    Enemy(RoomRef room);
    virtual ~Enemy();

    virtual void Update(float deltaTime);
    virtual void Init() override;

protected:
    virtual void BroadcastUpdate() override;
    virtual void TakeDamage(GameObjectRef instigator, Protocol::DamageType damageType, float damage) override;

protected:
    virtual void TickIdle(float deltaTime) override;
    virtual void TickRun(float deltaTime) override;
    virtual void TickSkill(float deltaTime) override;
    virtual void TickStun(float deltaTime) override;
    virtual void TickDead(float deltaTime) override;

private:
    PlayerRef FindClosestTarget();

private:
    void RandomWalk();

    float randomWalkDuration = 0.f;
    float randomWalkCoolDonw = 2.f;

private:
    weak_ptr<Player> _target;
    Vector _targetPos;

    float _attackRange = 200.f;
    float _moveSpeed = 200.f;

    float _attackCooldown = 2.f;
    float _attackDelay = 0.f;

private:
    void MoveToTarget(GameObjectRef target);
    void AttackToTarget(GameObjectRef target);

private:
    float _updatePacketCooldown = 1.f;
    float _elapsedPacket = 0.f;
};

