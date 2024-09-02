#pragma once
#include "Room.h"
class Raid :
    public Room
{
    using Super = Room;
public:
    Raid(uint64 roomId);
    virtual ~Raid();

    virtual void Init() override;
    virtual void Update(float deltaTime) override;
    
public:
    bool HandleNextPhase();

    bool HandlePhaseNumber(uint64 currentPhase);

    uint64 GetCurrentPhase() { return _currentPhase; }

    void SetBossEmergency(bool emergency) { _bossEmergency = emergency; }
private:
    bool ProceedNextPhase();

private:
    uint64 _currentPhase = 1;
    uint64 _phaseCount = 3;
       
    bool _bossEmergency = false;
    bool _bossExist = false;
    vector<vector<weak_ptr<GameObject>>> _phaseRequirements;
    BossRef boss;
};

extern RaidRef GRaid;