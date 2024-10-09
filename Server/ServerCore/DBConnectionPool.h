#pragma once

#include "DBConnection.h"
#include "DBStatement.h"

class DBConnectionPool
{
public:
	DBConnectionPool(const int32& maxConnection);
	~DBConnectionPool();

	void Init(SQLConnection detail);
	shared_ptr<DBConnection> Pop();
	void Push(shared_ptr<DBConnection> connection);

private:
	USE_LOCK;
	vector<shared_ptr<DBConnection>> _connections;
	const int32 _maxConnection;
};

