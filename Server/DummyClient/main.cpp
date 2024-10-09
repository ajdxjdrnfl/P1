#include "pch.h"
#include "main.h"
#include "SocketUtils.h"
#include "Session.h"
#include "Service.h"
#include "ClientPacketHandler.h"
#include "MemoryPool.h"
#include "ThreadManager.h"
#include "Global.h"

#define MAX_DUMMY_NUM 300

void ClientSession::OnConnected() 
{
	GDummyClients.Add(static_pointer_cast<ClientSession>(GetSessionRef()));
	Protocol::C_LOGIN loginPkt;
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
	Send(sendBuffer);
}

void ClientSession::OnDisconnected()
{

}

void ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	ClientPacketHandler::HandlePacket(session, buffer, len);

}
void ClientSession::OnSend(int32 len)
{

}

void DoDummyWork(IocpCoreRef core)
{
	while (true)
	{
		core->Dispatch(10);
	}
}


int main()
{
	//SocketUtils::Init();
	ClientPacketHandler::Init();
	vector<ClientServiceRef> services;

	IocpCoreRef core = make_shared<IocpCore>();

	for (int i = 0; i < MAX_DUMMY_NUM; i++)
	{
		ClientServiceRef service = make_shared<ClientService>(
			NetAddress(L"127.0.0.1", 7777),
			core,
			[=]() { return make_shared<ClientSession>(); },
			1);

		services.push_back(service);
		assert(service->Start());
	}

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([core]()
		{
			DoDummyWork(core);
		});
	}

	uint64 prev = GetTickCount64();

	while (true)
	{

		uint64 now = GetTickCount64();
		uint64 time = now - prev;
		
		// 3 second
		if (time >= 1000)
		{
			prev = now;
			
			auto ss = GDummyClients.Get();

			for (int i = 0; i < ss.size(); i++)
			{
				if (ss[i]->player)
				{
					ss[i]->player->Move();
				}
			}
		}
	}

	GThreadManager->Join();
}