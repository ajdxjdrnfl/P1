#pragma once
#include "GameObject.h"
class Npc :
    public GameObject
{
    using Super = GameObject;
public:
    Npc(RoomRef room);
    virtual ~Npc();
};

