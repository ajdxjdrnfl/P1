#include "pch.h"
#include "Collision.h"
#include "GameObject.h"
#include "Collider.h"
#include "CollisionManager.h"


Collision::Collision() : Super(EComponentType::ECT_COLLISION)
{

}

Collision::~Collision()
{
	delete _collider;
	_collider = nullptr;
}

void Collision::Init()
{
	Super::Init();

	if (_collider == nullptr)
		return;

	if (_collider->GetColliderType() == EColliderType::COLLIDER_BOX)
	{
		ColliderBox* box = static_cast<ColliderBox*>(_collider);
		obb.extent = box->extent;
	}
}

void Collision::Update(float deltaTime)
{
	GameObjectRef owner = GetOwner();
	
	if (owner == nullptr)
		return;

	SetPos(owner->GetPos());
	
	Super::Update(deltaTime);
}

void Collision::SetBound(Vector pos)
{
	if (_collider == nullptr)
		return;

	GameObjectRef gameObject = GetOwner();

	if (gameObject == nullptr)
		return;

	if (_collider->GetColliderType() == EColliderType::COLLIDER_BOX)
	{
		ColliderBox* box = static_cast<ColliderBox*>(_collider);

		obb.pos = pos;
		obb.orientation = gameObject->GetObjectInfo()->yaw();

		// TODO : orientation
		Vector min = obb.GetMin();
		Vector max = obb.GetMax();

		_bound.topLeft = { min.x, min.y };
		_bound.BottomRight = { max.x , max.y };
	}
	else if (_collider->GetColliderType() == EColliderType::COLLIDER_CIRCLE)
	{
		ColliderCircle* circle = static_cast<ColliderCircle*>(_collider);

		float radius = circle->_radius;
		_bound.topLeft = { pos.x - radius, pos.y - radius };
		_bound.BottomRight = { pos.x + radius, pos.y + radius };
	}
	
}

void Collision::SetPos(Vector pos)
{
	_pos = pos;
	SetBound(pos);
}

bool Collision::CheckCollision(Collision* other)
{
	EColliderType colliderType = GetCollider()->GetColliderType();
	EColliderType otherColliderType = other->GetCollider()->GetColliderType();
	
	switch (colliderType)
	{
	case EColliderType::COLLIDER_BOX:
		if (otherColliderType == EColliderType::COLLIDER_BOX)
		{
			return CollisionManager::CheckOverlapCollisionBoxToBox(this, other);
		}
		else if (otherColliderType == EColliderType::COLLIDER_CIRCLE)
		{
			return CollisionManager::CheckOverlapCollisionBoxToCircle(this, other);
		}
		break;
	case EColliderType::COLLIDER_CIRCLE:
		if (otherColliderType == EColliderType::COLLIDER_BOX)
		{
			return CollisionManager::CheckOverlapCollisionCircleToBox(this, other);
		}
		else if (otherColliderType == EColliderType::COLLIDER_CIRCLE)
		{
			return CollisionManager::CheckOverlapCollisionCircleToCircle(this, other);
		}
		break;
	default :
		return false;
		break;

	}

	return false;
}
