// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../P1.h"
/**
 * 
 */
class P1_API PacketSession : public TSharedFromThis<PacketSession>
{
public:
	PacketSession(class FSocket* Socket);
	~PacketSession();

	void Run();

	void Recv();

	void Disconnect();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

public:
	class FSocket* Socket;

	TSharedPtr<class RecvWorker> RecvWorkerThread;
	TSharedPtr<class SendWorker> SendWorkerThread;

	TQueue<TArray<uint8>> RecvPacketQueue;
	TQueue<SendBufferRef> SendPacketQueue;
};
