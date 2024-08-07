#include "pch.h"
#include "Player.h"
#include "Collision.h"
#include "Collider.h"
#include "ComponentBase.h"

Player::Player(GameSessionRef session, RoomRef room) : Super(room), _session(session)
{
	_objectInfo->set_hp(100.f);
	_objectInfo->set_max_hp(100.f);
	_objectInfo->set_stamina(100.f);
	_objectInfo->set_max_stamina(100.f);
	_objectInfo->set_castertype(Protocol::CASTER_TYPE_WARRIOR);
	_objectInfo->set_speed(400.f);
}

Player::~Player()
{

}

void Player::Init()
{
	// SkillTimer


	// TODO : Collision ÃÊ±â 
	Collision* collision = new Collision();
	ColliderCircle* collider = new ColliderCircle();
	collider->_radius = 38.f;
	collision->SetCollider(collider);

	AddComponent(collision);
	Super::Init();
}

void Player::Update(float deltaTime)
{
	Super::Update(deltaTime);
}
