#pragma once


class DummyClients
{
public:
	void Add(::shared_ptr<class ClientSession> session);

	vector<::shared_ptr<class ClientSession>> Get()
	{
		WRITE_LOCK;
		return sessions;
	}

public:
	USE_LOCK;
	vector<::shared_ptr<class ClientSession>> sessions;

};

extern DummyClients GDummyClients;