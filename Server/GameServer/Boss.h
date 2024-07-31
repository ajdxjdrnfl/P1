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
};

