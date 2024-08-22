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
    
public:
    bool HandleNextPhase();

private:
    bool ProceedNextPhase();

private:
    uint64 _currentPhase = 1;
    uint64 _phaseCount = 3;
    
    vector<vector<weak_ptr<GameObject>>> _phaseRequirements;
};
