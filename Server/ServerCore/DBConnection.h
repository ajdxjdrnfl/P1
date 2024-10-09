#pragma once
#include "mysql/mysql.h"
#include <string>

struct SQLConnection
{
	string server, user, password, database;

	SQLConnection(string server, string user, string password, string database) : server(server), user(user), password(password), database(database) { }
	SQLConnection() : server("localhost"), user("root"), password(""), database("") { }
};

class DB_Res : public enable_shared_from_this<DB_Res>
{
	friend class DBConnection;
	friend class DBStatement;

public:
	DB_Res() { }
	~DB_Res()
	{
		Free();
	}

	bool Next()
	{
		if ((_row = mysql_fetch_row(_res)) == NULL)
			return false;

		_map.clear();

		for (int32 i = 0; i < _fields.size(); i++)
		{
			if (_row[i] != nullptr)
				_map[_fields[i]] = _row[i];
			else _map[_fields[i]] = "";
		}

		return true;

	}

	bool getString(const string& name, string& out)
	{
		assert(_map.find(name) != _map.end());

		out = _map[name];

		return true;
	}

	bool getInt(const string& name, int32& out)
	{
		assert(_map.find(name) != _map.end());

		string temp = _map[name];

		if (temp.length() == 0)
			return false;

		out = static_cast<int32>(stoi(temp));
		return true;
	}

	bool IsSuccess()
	{
		return _success;
	}

	void SetResult(MYSQL_RES* res)
	{
		if (res == nullptr)
			return;

		_res = res;
		_fields.clear();
		int32 num = mysql_num_fields(_res);
		MYSQL_FIELD* field = mysql_fetch_field(_res);

		for (int32 i = 0; i < num; i++)
		{
			string fieldName = string(field[i].name);
			_fields.push_back(fieldName);
		}

	}

private:
	void Free()
	{
		if (_res != nullptr)
			mysql_free_result(_res);

		_res = nullptr;
	}

private:
	vector<string> _fields;
	map<string, string> _map;
	bool _success = true;
	MYSQL_RES* _res = nullptr;
	MYSQL_ROW _row = nullptr;
};

class DBConnection : public enable_shared_from_this<DBConnection>
{
public:
	DBConnection();
	~DBConnection();

	bool Connect(SQLConnection connection);

	void Free();
	shared_ptr<DB_Res> Query(const string& query);

	bool MakeStoredProcedure(const string& procedure);
	shared_ptr<class DBStatement> PrepareStatement(const string& query);

public:
	MYSQL* GetConnection() { return _connection; }

public:
	void HandleError();

private:
	MYSQL* _connection = nullptr;
	SQLConnection _details;

	DB_Res res;
};