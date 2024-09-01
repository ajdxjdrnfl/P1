#include "pch.h"
#include "Field.h"

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
