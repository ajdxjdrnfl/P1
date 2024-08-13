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
	// NPC �߰��ϸ� ������?
	// NPC�� ������� ����?
	unordered_map<uint64, NpcRef> _npcs;
};

