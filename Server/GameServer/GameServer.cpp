#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "Room.h"
#include "GameRoomManager.h"
#include "GameTickManager.h"
#include "ResourceManager.h"
#include "DBConnectionPool.h"

enum
{
	WORKER_TICK = 64
};

uint64 FrameCount = 0;

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}

void DoDBJob()
{
	int cnt = 0;
	while (cnt < 1)
	{
		auto connection = GDatabase->Pop();

		cout << "raw query version" << endl;
		{
			auto res = connection->Query("SELECT * FROM user");
			string name;
			if (res->Next() && res->getString("nickname", name))
			{
				cout << name << endl;
			}

		}

		cout << "statement version" << endl;
		{
			auto stmt = connection->PrepareStatement("SELECT nickname FROM user WHERE nickname = ?");
			stmt->SetString(1, "zelda");
			auto res = stmt->execute();

			string name;
			if (res->Next())
			{
				cout << res->getString("nickname") << endl;
			}
		}

		cout << "procedure version" << endl;
		{
			connection->MakeStoredProcedure("CREATE PROCEDURE p1( IN p_in1 VARCHAR(50), IN p_in2 VARCHAR(50), IN p_in3 INT) "
				"BEGIN "
                "INSERT INTO user (nickname, password, money) VALUES (p_in1, p_in2, p_in3);"
				"END");

			auto stmt = connection->PrepareStatement("CALL p1(?, ?, ?)");
			stmt->SetString(1, "test1");
			stmt->SetString(2, "1234");
			stmt->SetInt(3, 0);
			stmt->execute();

		}
		GDatabase->Push(connection);

		cnt++;
	}
}

int main()
{
	ServerPacketHandler::Init();
	GResourceManager.Init();
	GTickManager.Init();
	// TODO : Database 초기화 - 
	GDatabase->Init(SQLConnection{ "localhost", "root", "password", "dbname" });
	GRoomManager.Init();

	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[=]() { return make_shared<GameSession>(); }, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	GRoomManager.Update(0.f);

	for (int32 i = 0; i < THREAD_NUM; i++)
	{
		GThreadManager->Launch([&service]()
		{
			DoWorkerJob(service);
		});
	}

	for (int32 i = 0; i < 1; i++)
	{
		GThreadManager->Launch([]()
		{
			DoDBJob();
		});
	}

	while (true)
	{
		this_thread::sleep_for(1s);
	}

	GThreadManager->Join();
}