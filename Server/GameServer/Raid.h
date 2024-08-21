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
    bool ProceedNextPhase();

private:
    uint64 _currentPhase = 1;
    uint64 _phaseCount = 2;
    
};

