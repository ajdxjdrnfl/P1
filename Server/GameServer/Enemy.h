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

private:
    void RandomWalk();
};

