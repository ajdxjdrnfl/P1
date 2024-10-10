#pragma once
#include "GameObject.h"

// @Yang
struct FSlotData
{
public:
    int32 index;
    int32 quantity;
    const char* itemID;

};

class Player :
    public GameObject
{
    using Super = GameObject;
public:
    Player(GameSessionRef session, RoomRef room, Protocol::CasterType casterType);
    virtual ~Player();

    virtual void Init() override;
    virtual void Update(float deltaTime) override;

    GameSessionRef GetSessionRef() { return _session.lock(); }
    

    // 이동 동기화 함수
public:
    bool HandleMovePacket(Protocol::C_MOVE pkt);
    void SetTargetInfo(Protocol::ObjectInfo targetInfo);

    void ClearTargetPath();
    void AddTargetPath(vector<VectorInt>& path);
    bool PopTargetPath();

    // @Yang
    void AddToInventory(FSlotData _itemData);
    void SwitchSlot(int32 _index1, int32 _index2);
    void RemoveFromInventory(int32 _index);

protected:
    virtual void TickIdle(float deltaTime) override;
    virtual void TickJump(float deltaTime) override;
    virtual void TickRun(float deltaTime) override;
    virtual void TickSkill(float deltaTime) override;
    virtual void TickStun(float deltaTime) override;
    virtual void TickDead(float deltaTime) override;

protected:
    weak_ptr<class GameSession> _session;

    // 이동 동기화
    Protocol::ObjectInfo* _targetInfo;
    queue<VectorInt> _targetPath;
    Vector _targetPos;

    float _jumpSpeed = 300.f;

    // @Yang
    std::vector<FSlotData> _inventory;
};

