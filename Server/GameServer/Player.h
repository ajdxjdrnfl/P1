#pragma once
#include "GameObject.h"
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

protected:
    virtual void TickIdle(float deltaTime) override;
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
};

