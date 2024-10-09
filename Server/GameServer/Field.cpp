#include "pch.h"
#include "Field.h"
#include "Collision.h"
#include "GameObject.h"
#include "Map.h"

FieldRef GField = make_shared<Field>(3);

Field::Field(uint64 roomId) : Super(ERoomType::ERT_FIELD, roomId)
{
	_mapName = "Field";
}

Field::~Field()
{

}

void Field::Init()
{
	Super::Init();
}

void Field::Update(float deltaTime)
{
	Super::Update(deltaTime);
}

void Field::SetObjectToRandomPos(GameObjectRef gameObject)
{

	if (_map)
	{
		Vector startPos = _map->GetStartPoint();
		VectorInt gridPos = GetGridPos(startPos);

		VectorInt d[9] = { {-2,0}, {-2,-2}, {-2,2}, {0,0}, {0,2},{0,-2}, {2,0}, {2,-2}, {2,2} };
		int direction = 3;

		VectorInt newPos = gridPos + d[direction];

		Collision* collision = static_cast<Collision*>(gameObject->GetComponent(EComponentType::ECT_COLLISION));
		collision->SetPos(GetPosition(newPos));
		while (!IsWalkableAtPos(newPos) || CheckCollisionInQuadTree(collision) || CheckCollisionInMap(collision))
		{
			direction = (direction + 1) % 9;
			newPos = gridPos + d[direction];

			collision->SetPos(GetPosition(newPos));
		}

		Vector pos = GetPosition(newPos);
		float height = GetValidHeight(newPos) + 160.f;
		gameObject->GetObjectInfo()->set_x(pos.x);
		gameObject->GetObjectInfo()->set_y(pos.y);
		gameObject->GetObjectInfo()->set_z(height);
		gameObject->GetObjectInfo()->set_yaw(Utils::GetRandom(0.f, 100.f));

	}
}