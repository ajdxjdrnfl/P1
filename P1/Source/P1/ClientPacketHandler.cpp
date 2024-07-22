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
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GEngine->GetWorld()));

	if (GameInstance == nullptr)
		return false;

	GameInstance->GetMyCharacter()->SetInfo(pkt.info());

	return true;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GEngine->GetWorld()));

	if (GameInstance == nullptr)
		return false;

	GameInstance->CharacterSpawn(pkt);

	return true;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(UGameplayStatics::GetGameInstance(GEngine->GetWorld()));

	if (GameInstance == nullptr || GameInstance->IsMyCharacter(pkt.info().object_id()))
		return false;

	//GameInstance->C

	return true;
}
