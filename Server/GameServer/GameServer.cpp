#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "Room.h"
#include "GameRoomManager.h"
#include "GameTickManager.h"
#include "ResourceManager.h"
enum
{
	WORKER_TICK = 64
};

uint64 FrameCount = 0;

void DoWorkerJob(ServerServiceRef& service)
{
	GRoomManager.Update(0.f);

	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// ��Ʈ��ũ ����� ó�� -> �ΰ��� �������� (��Ŷ �ڵ鷯�� ����)
		service->GetIocpCore()->Dispatch(10);

		// ����� �ϰ� ó��
		ThreadManager::DistributeReservedJobs();

		// �۷ι� ť
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	ServerPacketHandler::Init();
	GResourceManager.Init();
	GTickManager.Init();

	GRoomManager.Init();

	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[=]() { return make_shared<GameSession>(); }, // TODO : SessionManager ��
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
		{
			DoWorkerJob(service);
		});
	}

	while (true)
	{
		this_thread::sleep_for(1s);
	}

	GThreadManager->Join();
}