// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "P1.h"
#include "P1/Data/SkillData.h"
#include "P1GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UP1GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	// GameServer
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;
	TSharedPtr<class PacketSession> GameServerSession;

	UPROPERTY()
	class AP1Character* MyCharacter;

	void InitSkillMap();
	void SetSkillInfo(int32 ID, const FSkillInfo& CurrentSkillInfo);

public:
	virtual void Init() override;

	UPROPERTY(EditAnywhere)
	UDataTable* SkillDataTable;

	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

	bool IsMyCharacter(uint64 ID);

	void SpawnActorByServer(Protocol::S_SPAWN& Pkt);
	void CharacterMove(Protocol::S_MOVE& Pkt);

	void SkillSpawn(Protocol::S_SKILL& Pkt);

	void AttackEnemy(Protocol::S_ATTACK& Pkt);

	void PlayMontage(Protocol::S_MONTAGE& Pkt);

	class AEnemyMob* SpawnMob(Protocol::ObjectInfo ObjInfo, FVector Loc);
	class AP1Character* SpawnCharacter(Protocol::ObjectInfo ObjInfo, FVector Loc);

	UPROPERTY()
	TMap<uint64, class AP1Character*> Characters;

	UPROPERTY()
	TMap<uint64, class AEnemyBase*> Enemies;

	UPROPERTY()
	TMap<uint64, class ASkillActorBase*> Skills;

	UPROPERTY()
	TMap<uint64, FSkillInfo> SkillInfo;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AP1Character> WarriorClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemyMob> EnemyMobClass;

	FORCEINLINE class AP1Character* GetMyCharacter() const { return MyCharacter; }

};
