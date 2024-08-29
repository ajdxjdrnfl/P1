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
	
	for (int i = 0; i < GetMesh()->GetMaterials().Num(); i++)
	{
		DefaultMaterials.Add(GetMesh()->GetMaterial(i));
	}
}

// Called every frame
void AP1Creature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetObjectInfo();

	if (bIsHit)
	{
		if (CurrentHitMaterialTime < TargetHitMaterialTime)
		{
			CurrentHitMaterialTime += DeltaTime;
		}
		else
		{
			CurrentHitMaterialTime = 0;
			bIsHit = false;
			SetDefaultMaterial();
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
	case ECCType::NockBack:
		GetKnockBack(SkillInfo.CCTime);
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

void AP1Creature::GetKnockBack(float _CCTime)
{
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

FRotator AP1Creature::GetTargetYaw()
{
	FRotator RotationToGet;
	if (USkillManagerSubSystem* SubSystem = GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		RotationToGet = SubSystem->TargetingSkillLocation.Rotation();
	}

	return RotationToGet;
}

void AP1Creature::SetCollisionEnabled(bool bEnable)
{
	if (bEnable)
	{
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	}
	else
	{
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}
}

void AP1Creature::SetHitMaterial()
{
	TArray<UMaterialInterface*> Materials = GetMesh()->GetMaterials();
	for (int i = 0; i < Materials.Num(); i++)
	{
		GetMesh()->SetMaterial(i, HitMaterial);
	}

	bIsHit = true;
	CurrentHitMaterialTime = 0;
}

void AP1Creature::SetDefaultMaterial()
{
	TArray<UMaterialInterface*> Materials = GetMesh()->GetMaterials();
	for (int i = 0; i < Materials.Num(); i++)
	{
		GetMesh()->SetMaterial(i, DefaultMaterials[i]);
	}
}
