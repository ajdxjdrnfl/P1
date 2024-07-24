// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponentBase.h"
#include "P1/P1GameInstance.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"

USkillComponentBase::USkillComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void USkillComponentBase::BeginPlay()
{
	Super::BeginPlay();

	if (UP1GameInstance* GameInstance = Cast<UP1GameInstance>(GetWorld()->GetGameInstance()))
	{
		SkillDataTable = GameInstance->SkillDataTable;
	}
	
}


void USkillComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USkillComponentBase::SetSkills()
{
	if (SkillDataTable == nullptr) return;
	FString ContextString;
	FSkillsByClass SkillsByClass = *SkillDataTable->FindRow<FSkillsByClass>(/* TODO: */ FName("Warrior"), ContextString);
	Skills = SkillsByClass.SkillInfos;
}

void USkillComponentBase::UseSkill(uint16 SkillIndex)
{
	// TODO: Skills[SkillIndex];
	FGeneralSkillInfo GeneralSkillInfo = Skills[SkillIndex];

	if (OwnerAnimInstance == nullptr)
		return;

	//if (SkillInfo.AnimMontage == nullptr) 
	//	return;

	FVector Loc = GetOwner()->GetActorLocation();
	FRotator Rot = GetOwner()->GetActorRotation();

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = Cast<AP1Creature>(GetOwner());
	SpawnParam.Instigator = Cast<AP1Creature>(GetOwner());
	ASkillActorBase* SkillActor = Cast<ASkillActorBase>(GetWorld()->SpawnActor(GeneralSkillInfo.SkillActor, &Loc, &Rot, SpawnParam));
	if (SkillActor == nullptr)
		return;

	{
		Protocol::C_SKILL Pkt;
		Protocol::SkillInfo* SkillInfo = Pkt.mutable_skillinfo();
		Protocol::ObjectInfo* ObjectInfo = Pkt.mutable_caster();

		AP1Character* P1Character = Cast<AP1Character>(GetOwner());
		if (P1Character == nullptr)
			return;

		SkillInfo->set_skill_id(1);

		if (SkillActor->GetSkillInfo().SkillShape == ESkillShapeType::Circle)
		{
			SkillInfo->set_collision_type(Protocol::CollisionType::COLLISION_TYPE_CIRCLE);
		}
		else if (SkillActor->GetSkillInfo().SkillShape == ESkillShapeType::Box)
		{
			SkillInfo->set_collision_type(Protocol::CollisionType::COLLISION_TYPE_BOX);
		}

		SkillInfo->set_size_x(SkillActor->GetSkillInfo().XScale);
		SkillInfo->set_size_y(SkillActor->GetSkillInfo().YScale);
		
		ObjectInfo->CopyFrom(*P1Character->Info);
		SEND_PACKET(Pkt);
	}

	SkillActor->ActivateSkill();
}

