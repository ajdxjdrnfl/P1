#pragma once
#include "GameObject.h"
class Boss :
    public GameObject
{
    using Super = GameObject;
public:
    Boss(RoomRef room);
    virtual ~Boss();

    virtual void Update(float deltaTime);
    virtual void Init() override;

    virtual void TakeDamage(GameObjectRef instigator, Protocol::DamageType damageType, float damage) override;

protected:
    virtual void TickIdle(float deltaTime) override;
    virtual void TickRun(float deltaTime) override;
    virtual void TickSkill(float deltaTime) override;
    virtual void TickStun(float deltaTime) override;

private:
    PlayerRef FindClosestTarget();

private:
    // Start Skill ( previously cast skill )
    void StartDefaultAttack();
    void StartRush();
    void StartSpawnPillars();
    void StartTeleport();
    void StartDot();

    // Default Skill
private:
    void SelectSkill(GameObjectRef target);

    // Skill Tick
    
    // Default Attack
    void DefaultAttack(GameObjectRef target);

    // Rush
    void Rush(GameObjectRef target, float deltaTime);
    void Rush_START(GameObjectRef target, float deltaTime);
    void Rush_ING(GameObjectRef target, float deltaTime);
    void Rush_END(GameObjectRef target, float deltaTime);

    // Dot Spell
    void DotSkill(GameObjectRef target, float deltaTime);
    void DotSkill_START(GameObjectRef target, float deltaTime);
    void DotSkill_ING(GameObjectRef target, float deltaTime);
    void DotSkill_END(GameObjectRef target, float deltaTime);
    
    // Pillar Spawn 
    void SpawnPillars(float deltaTime);
    void SpawnPillars_START();
    void SpawnPillars_ING();
    void SpawnPillars_END();

    // Teleport
    void Teleport(Vector pos, float deltaTime);

    // Gimmick

private:
    vector<weak_ptr<Structure>> _pillars;
    int32 _pillarNum;

private:
    void MoveToTarget(GameObjectRef target);
    
    
private:
    weak_ptr<class Player> _target;
    float _attackRange = 200.f;

    bool _isGimmik = false;
    bool _forceNext = false;
    
    float _moveSpeed = 300.f;
    float _rushSpeed = 600.f;

    float _nextStepCooldown = 1.f;
    float _prevStepElapsedTime = 0.f;
    float _attackCooldown = 2.f;
    float _attackDelay = 0.f;

    // update time
    float _updatePacketCooldown = 1.f;
    float _elapsedPacket = 0.f;

    Vector _targetPos;

    // Boss Skill
private:
    EBossSkillType _skillType = EBST_NONE;
    EBossMontageType _montageType = MONTAGE_TYPE_NONE;
};

