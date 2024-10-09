#include "pch.h"
#include "Global.h"
#include "main.h"

DummyClients GDummyClients;

void DummyClients::Add(ClientSessionRef session)
{
	WRITE_LOCK;
	sessions.push_back(session);
}