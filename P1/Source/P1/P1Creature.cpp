// Fill out your copyright notice in the Description page of Project Settings.


#include "P1Creature.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "P1/P1.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
//#include "P1/Component/DamageIndicator/DamageIndicatorComponentBase.h"

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
	GetClassTypeInt();
	
}

// Called every frame
void AP1Creature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (ObjectInfo->state() == Protocol::MOVE_STATE_RUN)
	{
		TickMove(DeltaTime);
	}

	/*if (CreatureState == ECreatureState::Stun)
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
	}*/
}

// Called to bind functionality to input
void AP1Creature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AP1Creature::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Die == nullptr || AnimInstance->Montage_IsPlaying(M_Die))
		return;

	AnimInstance->Montage_Play(M_Die);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AP1Creature::Despawn()
{
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
	//CreatureState = ECreatureState::Stun;
	CCTime = _CCTime;

	if (USkillManagerSubSystem* SkillSubSystem = GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SkillSubSystem->bCanUseSkill = false;
		SkillSubSystem->bCanMove = false;
	}
}

void AP1Creature::GetClassTypeInt()
{
	if (ClassType == FName("Warrior"))
	{
		ClassTypeInt = 0;
	}
	else if (ClassType == FName("Archer"))
	{
		ClassTypeInt = 1;
	}
	else if (ClassType == FName("Boss"))
	{
		ClassTypeInt = 2;
	}
	else if (ClassType == FName("Mob"))
	{
		ClassTypeInt = 3;
	}
}

void AP1Creature::SetMoveValueByServer(Protocol::S_MOVE Pkt)
{
	if (this == nullptr)
		return;

	if (CheckDeath(Pkt)) 
		return;

	TargetInfo->CopyFrom(Pkt.info());
	ObjectInfo->set_state(Pkt.info().state());

	/*FVector TargetLocation = FVector(Pkt.info().x(), Pkt.info().y(), Pkt.info().z());
	FRotator TargetRotation = FRotator(GetActorRotation().Pitch, Pkt.info().yaw(), GetActorRotation().Roll);

	bool NearX = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.X, GetActorLocation().X, 2);
	bool NearY = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetLocation.Y, GetActorLocation().Y, 2);
	bool NearYaw = UKismetMathLibrary::NearlyEqual_FloatFloat(TargetRotation.Yaw, GetActorRotation().Yaw, 5);

	bool bIsNearLoc = NearX && NearY;

	if (bIsNearLoc)
	{
		ObjectInfo->set_state(Protocol::MOVE_STATE_IDLE);
	}
	else
	{
		ObjectInfo->set_state(Protocol::MOVE_STATE_RUN);
	}*/
	
	GetCharacterMovement()->MaxWalkSpeed = Pkt.info().speed();
}

bool AP1Creature::CheckDeath(Protocol::S_MOVE Pkt)
{
	if (Pkt.info().state() == Protocol::MOVE_STATE_DEAD)
	{
		Die();
		return true;
	}
	return false;
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

int32 AP1Creature::GetObjectID()
{
	return ObjectInfo->object_id();
}
