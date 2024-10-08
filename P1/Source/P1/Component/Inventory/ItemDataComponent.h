// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "P1/Interface/InteractInterface.h"
#include "P1/Data/InventorySlotData.h"
#include "ItemDataComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class P1_API UItemDataComponent : public UActorComponent, public IItemInteractInterface
{
	GENERATED_BODY()

public:	
	UItemDataComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere)
	int32 Quantity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FDataTableRowHandle ItemID;

public:
	virtual void Interact(class UInventoryComponent* InventoryComponent) override;

	FORCEINLINE FDataTableRowHandle GetItemID() { return ItemID; };
	FORCEINLINE int32 GetQuantity() { return Quantity; };
	FORCEINLINE void SetQuantity(int32 QuantityToSet) { Quantity = QuantityToSet; };

};
