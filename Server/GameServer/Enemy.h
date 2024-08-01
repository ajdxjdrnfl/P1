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

private:
    void RandomWalk();

    float randomWalkDuration = 0.f;
    float randomWalkCoolDonw = 2.f;

};

