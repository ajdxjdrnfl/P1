// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P1/Interface/InteractInterface.h"
#include "P1/Data/InventorySlotData.h"
#include "ItemBase.generated.h"

UCLASS()
class P1_API AItemBase : public AActor, public IItemInteractInterface
{
	GENERATED_BODY()
	
public:	
	AItemBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Interact(class UInventoryComponent* InventoryComponent) override;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
	class UItemDataComponent* ItemDataComponent;

private:
	UPROPERTY(EditAnywhere, Category = Editable)
	FItemData ItemData;

public:
	UFUNCTION()
	virtual void OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnCollisionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
