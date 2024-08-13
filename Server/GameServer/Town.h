#pragma once
#include "Room.h"

class Town : public Room
{
	using Super = Room;
public:
	Town();
	virtual ~Town();

	virtual void Init() override;
	virtual void Update(float deltaTime) override;

private:
	// NPC 추가하면 좋을듯?
	// NPC는 어떤식으로 관리?
	unordered_map<uint64, NpcRef> _npcs;
};

