#include "pch.h"
#include "Structure.h"
#include "Collision.h"
#include "Collider.h"

Structure::Structure(RoomRef room) : Super(room)
{
	_objectInfo->set_castertype(Protocol::CASTER_TYPE_STRUCTURE);
}

Structure::~Structure()
{

}

void Structure::Init()
{
	Collision* collision = new Collision();
	ColliderCircle* collider = new ColliderCircle();
	collider->_radius = 50.f;
	collision->SetCollider(collider);

	AddComponent(collision);
	Super::Init();
}

void Structure::TakeDamage(GameObjectRef instigator, Protocol::DamageType damageType, float damage)
{
	Super::TakeDamage(instigator, damageType, damage);

	// TODO : DESPAWN
}
