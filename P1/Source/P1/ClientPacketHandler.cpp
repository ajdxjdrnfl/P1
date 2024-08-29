#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "P1.h"
#include "P1GameInstance.h"
#include "P1GameMode.h"
#include "P1Character.h"
#include "P1/Enemy/EnemyBoss.h"
#include "Kismet/GameplayStatics.h"
#include "P1/SubSystem/GameInstance/DungeonManagerSubsystem.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	
	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));

	if (GameInstance == nullptr)
		return false;

	GameInstance->GetMyCharacter()->ObjectInfo->CopyFrom(pkt.info());
	GameInstance->GetMyCharacter()->InitOnSpawn(pkt.info());
	
	GameInstance->GetMyCharacter()->TargetInfo->set_x(pkt.info().x());
	GameInstance->GetMyCharacter()->TargetInfo->set_y(pkt.info().y());
	GameInstance->GetMyCharacter()->TargetInfo->set_z(pkt.info().z());

	GameInstance->Characters.Add(pkt.info().object_id(), GameInstance->GetMyCharacter());
	GameInstance->GetMyCharacter()->SetActorLocation(FVector(pkt.info().x(), pkt.info().y(), pkt.info().z()));

	if (pkt.roomtype() == Protocol::RoomType::ROOM_TYPE_DUNGEON)
	{
		UDungeonManagerSubsystem* DungeonManager = GameInstance->GetSubsystem<UDungeonManagerSubsystem>();
		if (DungeonManager == nullptr) return false;

		DungeonManager->Init();
	}

	return true;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));

	if (GameInstance == nullptr)
		return false;

	GameInstance->SpawnActorByServer(pkt);

	return true;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));

	if (GameInstance == nullptr)
		return false;

	GameInstance->DespawnActorByServer(pkt);
	return false;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));

	if (GameInstance == nullptr)
		return false;

	GameInstance->MoveByServer(pkt);

	return true;
}

bool Handle_S_SKILL(PacketSessionRef& session, Protocol::S_SKILL& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));
	uint64 id = pkt.caster().object_id();

	if (GameInstance == nullptr)
		return false;

	uint64 SkillID = pkt.skillid();

	GameInstance->SkillSpawn(pkt);

	return false;
}

bool Handle_S_ATTACK(PacketSessionRef& session, Protocol::S_ATTACK& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));

	if (GameInstance == nullptr)
		return false;

	GameInstance->AttackTarget(pkt);

	return false;
}

bool Handle_S_MONTAGE(PacketSessionRef& session, Protocol::S_MONTAGE& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));

	if (GameInstance == nullptr)
		return false;

	GameInstance->PlayMontage(pkt);

	return false;
}

bool Handle_S_DEAD(PacketSessionRef& session, Protocol::S_DEAD& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));

	if (GameInstance == nullptr)
		return false;

	GameInstance->KillCreature(pkt);

	return false;
}

bool Handle_S_PHASE(PacketSessionRef& session, Protocol::S_PHASE& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));

	if (GameInstance == nullptr)
		return false;

	GameInstance->SetPhase(pkt);
	return false;
}

bool Handle_S_DAMAGE(PacketSessionRef& session, Protocol::S_DAMAGE& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));

	if (GameInstance == nullptr)
		return false;

	GameInstance->AttackTargetByDot(pkt);
	return false;
}

bool Handle_S_PREDICTSKILL(PacketSessionRef& session, Protocol::S_PREDICTSKILL& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));

	if (GameInstance == nullptr)
		return false;

	GameInstance->PredictSkillPosition(pkt);

	return false;
}
