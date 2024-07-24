#pragma once
#include "Collision.h"

class CollisionManager
{
public:
	// Overlap
	static bool CheckOverlapCollisionBoxToBox(Collision* trigger, Collision* victim);
	static bool CheckOverlapCollisionBoxToCircle(Collision* trigger, Collision* victim);
	static bool CheckOverlapCollisionBoxToRay(Collision* trigger, Collision* victim);

	static bool CheckOverlapCollisionCircleToBox(Collision* trigger, Collision* victim);
	static bool CheckOverlapCollisionCircleToCircle(Collision* trigger, Collision* victim);
	static bool CheckOverlapCollisionCircleToRay(Collision* trigger, Collision* victim);

	static bool CheckOverlapCollisionRayToBox(Collision* trigger, Collision* victim);
	static bool CheckOverlapCollisionRayToCircle(Collision* trigger, Collision* victim);
	static bool CheckOverlapCollisionRayToRay(Collision* trigger, Collision* victim);

};