#pragma once

#include "ClientPacketHandler.h"

using PlayerRef = ::shared_ptr<class DummyPlayer>;

class ClientSession : public PacketSession
{
public:

	virtual void OnConnected() override;

	virtual void OnDisconnected() override;

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

public:
	PlayerRef player;
};

class DummyPlayer : public enable_shared_from_this<DummyPlayer>
{
public:
	DummyPlayer()
	{
		info = new Protocol::ObjectInfo();
	}
	~DummyPlayer()
	{

	}

	void Move()
	{
		Protocol::C_MOVE movePkt;
		Protocol::ObjectInfo* current = movePkt.mutable_info();
		Protocol::ObjectInfo* target = movePkt.mutable_targetinfo();

		current->CopyFrom(*info);
		target->CopyFrom(*info);

		float targetX;
		float targetY;
		// �ٽ� ���ڸ��� ���ư���
		if (_goBack)
		{
			targetX = defaultX;
			targetY = defaultY;
		}
		// �����̱�
		else
		{
			targetX = defaultX + 100.f;
			targetY = defaultY + 100.f;
		}

		current->set_state(Protocol::MOVE_STATE_RUN);
		target->set_state(Protocol::MOVE_STATE_RUN);

		target->set_x(targetX);
		target->set_y(targetY);

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
		session->Send(sendBuffer);

		info->set_x(targetX);
		info->set_y(targetY);

		_goBack = !_goBack;
	}

public:
	Protocol::ObjectInfo* info = nullptr;
	float defaultX = 0.f, defaultY = 0.f;
	bool _goBack = false; // ������ ���ٰ� �ڷ� ����
	ClientSessionRef session;
};
