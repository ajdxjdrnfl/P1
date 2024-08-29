#include "pch.h"
#include "Field.h"

Field::Field(uint64 roomId) : Super(ERoomType::ERT_FIELD, roomId)
{

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
