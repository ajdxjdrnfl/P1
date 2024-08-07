#include "pch.h"
#include "CollisionManager.h"
#include "Collider.h"
#include "GameObject.h"


bool CollisionManager::CheckOverlapCollisionBoxToBox(Collision* trigger, Collision* victim)
{
	Vector xAxis = { 1, 0 };
	Vector yAxis = { 0, 1 };

	float orient1 = trigger->obb.orientation;
	float orient2 = victim->obb.orientation;
	vector<Vector> axis;

	axis.push_back(xAxis.Rotate(orient1));
	axis.push_back(xAxis.Rotate(orient2));
	axis.push_back(yAxis.Rotate(orient1));
	axis.push_back(yAxis.Rotate(orient2));

	for (int32 i = 0; i < axis.size(); ++i)
	{
		Vector dAxis = axis[i];

		float aMax = dAxis.Dot(trigger->obb.GetMax());
		float aMin = dAxis.Dot(trigger->obb.GetMin());

		float bMax = dAxis.Dot(victim->obb.GetMax());
		float bMin = dAxis.Dot(victim->obb.GetMin());

		if (aMin > bMax || bMin > aMax)
			return false;
	}

	return true;
}
bool CollisionManager::CheckOverlapCollisionBoxToCircle(Collision* trigger, Collision* victim)
{
	Vector closestP = trigger->obb.ClosestPoint(victim->GetPos());

	float rad = static_cast<ColliderCircle*>(victim->GetCollider())->_radius;
	float dist = closestP.Distance(victim->GetPos());

	if (trigger->GetBound().IsInBound(victim->GetPos().x, victim->GetPos().y))
		return true;

	if (rad < dist)
		return false;
	else
		return true;
}
bool CollisionManager::CheckOverlapCollisionBoxToRay(Collision* trigger, Collision* victim)
{
	return false;
}

bool CollisionManager::CheckOverlapCollisionCircleToBox(Collision* trigger, Collision* victim)
{
	return CheckOverlapCollisionBoxToCircle(victim, trigger);
}
bool CollisionManager::CheckOverlapCollisionCircleToCircle(Collision* trigger, Collision* victim)
{
	Vector pos1 = trigger->GetPos();
	Vector pos2 = victim->GetPos();

	float radius1 = static_cast<ColliderCircle*>(trigger->GetCollider())->_radius;
	float radius2 = static_cast<ColliderCircle*>(victim->GetCollider())->_radius;

	float distance = pos1.Distance(pos2);

	if (distance > radius1 + radius2)
		return false;
	else
		return true;
}

bool CollisionManager::CheckOverlapCollisionCircleToRay(Collision* trigger, Collision* victim)
{
	Vector origin = trigger->GetPos();
	Vector rayOrigin = static_cast<ColliderRay*>(victim->GetCollider())->_start;

	Vector tV = static_cast<ColliderRay*>(victim->GetCollider())->_end -static_cast<ColliderRay*>(victim->GetCollider())->_start;

	Vector rayDirection = tV.Normalize();

	Vector e = origin - rayOrigin;

	float a = e.Dot(rayDirection);
	float eSq = e.LengthSquared();
	float rSq = static_cast<ColliderCircle*>(trigger->GetCollider())->_radius * static_cast<ColliderCircle*>(trigger->GetCollider())->_radius;
	float bSq = eSq - (a * a);

	if (rSq - bSq < 0.f)
		return false;

	float t = tV.Length();
	float f = sqrt(rSq - bSq);

	if (a > f + t)
		return false;

	return true;
}

bool CollisionManager::CheckOverlapCollisionRayToBox(Collision* trigger, Collision* victim)
{
	return CheckOverlapCollisionBoxToRay(victim, trigger);
}

bool CollisionManager::CheckOverlapCollisionRayToCircle(Collision* trigger, Collision* victim)
{
	return CheckOverlapCollisionCircleToRay(victim, trigger);
}

bool CollisionManager::CheckOverlapCollisionRayToRay(Collision* trigger, Collision* victim)
{
	return false;
}
