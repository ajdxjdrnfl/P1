#pragma once
#include "Room.h"

class Field : public Room
{
	using Super = Room;
public:
	Field(uint64 roomId);
	virtual ~Field();

	virtual void Init() override;
	virtual void Update(float deltaTime) override;

private:
	virtual void SetObjectToRandomPos(GameObjectRef gameObject) override;

private:
	// NPC 추가하면 좋을듯?
	// NPC는 어떤식으로 관리?
	unordered_map<uint64, NpcRef> _npcs;
};

extern FieldRef GField;