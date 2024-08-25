#pragma once
#include "ComponentBase.h"

class Collider;

class Collision :
    public ComponentBase
{
	using Super = ComponentBase;
public:
	Collision();
	virtual ~Collision();

	virtual void Init();
	virtual void Update(float deltaTime);

public:
	Bound GetBound() { return _bound; }
	void SetBound(Vector pos);

	void SetPos(Vector pos);
	Vector GetPos() { return _pos; }

	Collider* GetCollider() { return _collider; }
	void SetCollider(Collider* collider) { _collider = collider; }

public:
	bool CheckCollision(Collision* other);

private:
	// Collision resource
	Collider* _collider = nullptr;

	Bound _bound;
	Vector _pos = { 0,0 };

public:
	// obb for collision box
	OBB obb;

};
