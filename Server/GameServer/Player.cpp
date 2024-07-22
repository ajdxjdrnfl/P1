#include "pch.h"
#include "Player.h"

Player::Player(GameSessionRef session, RoomRef room) : Super(room), _session(session)
{

}

Player::~Player()
{

}
