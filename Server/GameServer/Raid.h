#pragma once
#include "Room.h"
class Raid :
    public Room
{
    using Super = Room;
public:
    Raid();
    virtual ~Raid();

    virtual void Init() override;
    virtual void Update(float deltaTime) override;

    
private:
    int32 _phaseCount = 0;
};

