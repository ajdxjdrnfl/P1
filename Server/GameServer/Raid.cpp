#include "pch.h"
#include "Raid.h"

Raid::Raid()
{

}

Raid::~Raid()
{

}

void Raid::Init()
{
	Super::Init();

	// phasenumber�� ���͵� ����
}

void Raid::Update(float deltaTime)
{
	Super::Update(deltaTime);
	
	if (ProceedNextPhase())
	{
		
	}
}

bool Raid::ProceedNextPhase()
{
	return false;
}
