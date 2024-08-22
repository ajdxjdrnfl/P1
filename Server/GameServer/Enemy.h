#pragma once
#include "GameObject.h"
class Enemy :
    public GameObject
{
    using Super = GameObject;
public:
    Enemy(RoomRef room, uint64 phaseNumber);
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

public:
    uint64 GetPhaseNumber() { return _phaseNumber; }
private:
    PlayerRef FindClosestTarget(float maxDistance);

private:
    void RandomWalk();

    float randomWalkDuration = 0.f;
    float randomWalkCooldown = 2.f;

private:
    void RandomRotate();

private:
    weak_ptr<Player> _target;
    Vector _targetPos;

    float _detectRange = 600.f;
    float _attackRange = 300.f;
    float _moveSpeed = 200.f;

    float _attackCooldown = 2.f;
    float _attackDelay = 0.f;

private:
    void MoveToTarget(GameObjectRef target);
    void AttackToTarget(GameObjectRef target);

    bool _sendSkillPacket = false;
private:
    float _updatePacketCooldown = 1.f;
    float _elapsedPacket = 0.f;

private:
    uint64 _phaseNumber = 0;
};

