#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "P1.h"
#include "P1GameInstance.h"
#include "P1GameMode.h"
#include "P1Character.h"
#include "Kismet/GameplayStatics.h"

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

	GameInstance->GetMyCharacter()->SetInfo(pkt.info());
	GameInstance->GetMyCharacter()->SetActorLocation(FVector(pkt.info().x(), pkt.info().y(), pkt.info().z()));

	return true;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));

	if (GameInstance == nullptr)
		return false;

	GameInstance->CharacterSpawn(pkt);

	return true;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));
	uint64 id = pkt.info().object_id();

	if (GameInstance == nullptr || GameInstance->IsMyCharacter(id))
		return false;

	GameInstance->Characters[id]->SetMoveValueByServer(pkt);

	return true;
}

bool Handle_S_SKILL(PacketSessionRef& session, Protocol::S_SKILL& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GWorld));
	uint64 id = pkt.caster().object_id();

	if (GameInstance == nullptr || GameInstance->IsMyCharacter(id))
		return false;

	if (!GameInstance->Skills.Contains(pkt.skillinfo().skill_id())) 
		return false;

	GameInstance->SkillSpawn(pkt);

	return false;
}
