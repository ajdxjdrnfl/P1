#pragma once
#include "GameObject.h"
class Structure :
    public GameObject
{
    using Super = GameObject;
public:
    Structure(RoomRef room);
    virtual ~Structure();

    virtual void Init() override;
    
    virtual void TakeDamage(GameObjectRef instigator, struct SkillInfo skillInfo, float damage, bool counter) override;
};

