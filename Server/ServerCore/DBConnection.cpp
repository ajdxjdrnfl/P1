#include "pch.h"
#include "DBConnection.h"
#include "DBStatement.h"

DBConnection::DBConnection()
{
	_connection = mysql_init(NULL);
}

DBConnection::~DBConnection()
{
	Free();
}

bool DBConnection::Connect(SQLConnection details)
{
	bool success = true;

	_details = details;

	if (!mysql_real_connect(_connection, details.server.c_str(), details.user.c_str(), details.password.c_str(),
		details.database.c_str(), 0, NULL, 0))
	{
		success = false;
		HandleError();
	}

	return success;
}

void DBConnection::Free()
{
	if (_connection != nullptr)
		mysql_close(_connection);
}

shared_ptr<DB_Res> DBConnection::Query(const string& query)
{
	// initialzie result
	shared_ptr<DB_Res> res = make_shared<DB_Res>();
	res->_success = false;

	if (_connection == nullptr)
		return res;

	if (mysql_real_query(_connection, query.c_str(), query.length()) != 0)
	{
		HandleError();
		return res;
	}

	res->_success = true;
	res->SetResult(mysql_use_result(_connection));

	return res;
}

bool DBConnection::MakeStoredProcedure(const string& procedure)
{
	if (_connection == nullptr)
		return false;

	if (mysql_real_query(_connection, procedure.c_str(), procedure.length()) != 0)
	{
		HandleError();
		return false;
	}

	return true;
}

shared_ptr<DBStatement> DBConnection::PrepareStatement(const string& query)
{
	shared_ptr<DBStatement> statement = make_shared<DBStatement>(shared_from_this());

	statement->Init();
	statement->Prepare(query);

	return statement;
}



void DBConnection::HandleError()
{
	cout << "DBConnection Error No : " << mysql_errno(_connection) << endl;
	cout << "DBConnection Error : " << mysql_error(_connection) << endl;
}