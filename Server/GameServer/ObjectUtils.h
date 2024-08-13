#pragma once
class ObjectUtils
{
public:
	static PlayerRef CreatePlayer(GameSessionRef session, RoomRef room, Protocol::CasterType casterType);
	static EnemyRef CreateEnemy(RoomRef room);
	static SkillActorRef CreateSkillActor(GameObjectRef caster, RoomRef room);
	static BossRef CreateBoss(RoomRef room);
	static StructureRef CreateStructure(RoomRef room);
	
private:
	static atomic<int64> s_idGenerator;
};

