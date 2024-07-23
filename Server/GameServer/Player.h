#pragma once
#include "GameObject.h"
class Player :
    public GameObject
{
    using Super = GameObject;
public:
    Player(GameSessionRef session, RoomRef room);
    virtual ~Player();

    virtual void Init() override;
    virtual void Update(float deltaTime) override;

    GameSessionRef GetSessionRef() { return _session.lock(); }

protected:
    weak_ptr<class GameSession> _session;
};

