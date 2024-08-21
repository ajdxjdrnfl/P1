// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhaseDoor.generated.h"

UCLASS()
class P1_API APhaseDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	APhaseDoor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = Editable)
	int32 PhaseID;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* DoorComponent;

	bool bIsOpeningDoor;

	UPROPERTY(EditAnywhere, Category = Editable)
	float OpeningDist = 200;

public:
	void OpenDoor();

	FORCEINLINE int32 GetPhaseID() { return PhaseID; }

};
