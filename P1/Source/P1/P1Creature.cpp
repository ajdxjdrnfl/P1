// Fill out your copyright notice in the Description page of Project Settings.


#include "P1Creature.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "P1/P1.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AP1Creature::AP1Creature()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bRunPhysicsWithNoController = true;

	ObjectInfo = new Protocol::ObjectInfo();
	TargetInfo = new Protocol::ObjectInfo();
}

// Called when the game starts or when spawned
void AP1Creature::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

// Called every frame
void AP1Creature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveByServer(DeltaTime);

	if (CreatureState == ECreatureState::Stun)
	{
		if (CCTime <= 0)
		{
			CreatureState = ECreatureState::Idle;
			if (USkillManagerSubSystem* SkillSubSystem = GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
			{
				SkillSubSystem->bCanUseSkill = true;
				SkillSubSystem->bCanMove = true;
			}
		}
		else
		{
			CCTime -= DeltaTime;
		}
	}
}

// Called to bind functionality to input
void AP1Creature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AP1Creature::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Die == nullptr)
		return;

	{
		Protocol::C_MONTAGE Pkt;
		Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
		CasterInfo->set_object_id(ObjectInfo->object_id());
		Pkt.set_isstop(false);

		SEND_PACKET(Pkt);
	}

	TArray<AActor*> Actors;
	GetAttachedActors(Actors);
	for (AActor* Actor : Actors)
	{
		Actor->Destroy();
	}
	Destroy();
}

void AP1Creature::SetCreatureStateByServer(FSkillInfo SkillInfo)
{
	switch (SkillInfo.CCType)
	{
	case ECCType::Stun:
		GetStun(SkillInfo.CCTime);
		break;
	default:
		break;
	}
}

void AP1Creature::GetStun(float _CCTime)
{
	CreatureState = ECreatureState::Stun;
	CCTime = _CCTime;

	if (USkillManagerSubSystem* SkillSubSystem = GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SkillSubSystem->bCanUseSkill = false;
		SkillSubSystem->bCanMove = false;
	}
}

int32 AP1Creature::GetClassTypeInt()
{
	if (ClassType == FName("Warrior"))
	{
		return 0;
	}
	else if (ClassType == FName("Archer"))
	{
		return 1;
	}
	else if (ClassType == FName("Boss"))
	{
		return 2;
	}
	else if (ClassType == FName("Mob"))
	{
		return 3;
	}
	return -1;
}

void AP1Creature::SetMoveValueByServer(Protocol::S_MOVE Pkt)
{
	if (this == nullptr) return;

	ObjectInfo->set_state(Pkt.info().state());
	FTransform TargetTransform;
	FVector TargetLocation = FVector(Pkt.info().x(), Pkt.info().y(), Pkt.info().z());
	FRotator TargetRotation = FRotator(GetActorRotation().Pitch, Pkt.info().yaw(), GetActorRotation().Roll);
	TargetTransform.SetLocation(TargetLocation);
	TargetTransform.SetRotation(TargetRotation.Quaternion());

	bool NearX = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.X, GetActorLocation().X, 15);
	bool NearY = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.Y, GetActorLocation().Y, 15);
	bool NearZ = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.Z, GetActorLocation().Z, 15);
	bool bIsNear = NearX && NearY && NearZ;

	if (bIsNear)
	{
		CreatureState = ECreatureState::Idle;
	}
	else
	{
		CreatureState = ECreatureState::Move;
		TargetInfo->set_x(TargetLocation.X);
		TargetInfo->set_y(TargetLocation.Y);
		TargetInfo->set_z(TargetLocation.Z);
		TargetInfo->set_yaw(TargetRotation.Yaw);
	}

	GetCharacterMovement()->MaxWalkSpeed = Pkt.info().speed();
}

void AP1Creature::SetObjectInfo()
{
	ObjectInfo->set_x(GetActorLocation().X);
	ObjectInfo->set_y(GetActorLocation().Y);
	ObjectInfo->set_z(GetActorLocation().Z);
	ObjectInfo->set_yaw(GetActorRotation().Yaw);
}

void AP1Creature::SetWalkSpeed(float WalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
