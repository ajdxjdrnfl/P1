#include "pch.h"
#include "DBConnectionPool.h"
#include "DBStatement.h"

DBConnectionPool::DBConnectionPool(const int32& maxConnection) : _maxConnection(maxConnection)
{

}

DBConnectionPool::~DBConnectionPool()
{
	mysql_library_end();

	_connections.clear();
}

void DBConnectionPool::Init(SQLConnection detail)
{
	mysql_library_init(0, NULL, NULL);
	for (int32 i = 0; i < _maxConnection; i++)
	{
		shared_ptr<DBConnection> connection = make_shared<DBConnection>();
		connection->Connect(detail);
		_connections.push_back(connection);
	}
}

shared_ptr<DBConnection> DBConnectionPool::Pop()
{
	WRITE_LOCK;

	if (_connections.empty())
		return nullptr;

	shared_ptr<DBConnection> connection = _connections.back();
	_connections.pop_back();
	return connection;
}

void DBConnectionPool::Push(shared_ptr<DBConnection> connection)
{
	WRITE_LOCK;
	_connections.push_back(connection);
}