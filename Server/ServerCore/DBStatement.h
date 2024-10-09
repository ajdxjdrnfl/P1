#pragma once
#include "DBConnection.h"
#include "mysql/mysql.h"
#include <vector>
#include <any>

using namespace std;

class STATEMENT_Res : enable_shared_from_this<STATEMENT_Res>
{
	friend class DBStatement;

	struct RES_PARAM
	{
		~RES_PARAM()
		{
			free(buffer);
		}

		enum_field_types type;
		int32 buffer_length;
		char* buffer;
		unsigned long length;
	};

public:
	STATEMENT_Res(MYSQL_RES* metadata) : _metadata(metadata) { }
	~STATEMENT_Res()
	{
		if (_metadata != nullptr)
			mysql_free_result(_metadata);
		_map.clear();
	}

	bool Next();

	string getString(const string& key)
	{
		return string(_map[key].buffer);
	}
	int getInt(const string& key)
	{
		return (int)(*_map[key].buffer);
	}
	float getFloat(const string& key)
	{
		return (float)(*_map[key].buffer);
	}
	double getDouble(const string& key)
	{
		return (double)(*_map[key].buffer);
	}

private:
	map<string, RES_PARAM> _map;
	shared_ptr<DBStatement> _stmt;
	MYSQL_RES* _metadata = nullptr;
};

struct DB_PARAM
{
	~DB_PARAM()
	{

	}

	template<typename T>
	void Set(T param, unsigned long param_len)
	{
		if (std::is_same<string, std::remove_const<std::remove_reference<T>>>::value)
		{
			type = MYSQL_TYPE_VARCHAR;
		}
		else if (std::is_same<char*, std::remove_const<std::remove_reference<T>>>::value)
		{
			type = MYSQL_TYPE_VARCHAR;
		}
		else if (std::is_same<int, std::remove_const<std::remove_reference<T>>>::value)
		{
			type = MYSQL_TYPE_LONG;
		}
		else if (std::is_same<double, std::remove_const<std::remove_reference<T>>>::value)
		{
			type = MYSQL_TYPE_DOUBLE;
		}
		else if (std::is_same<float, std::remove_const<std::remove_reference<T>>>::value)
		{
			type = MYSQL_TYPE_FLOAT;
		}

		data = param;
		length = param_len;
	}

	enum_field_types type;
	std::any data;
	unsigned long length;
};

class DBStatement : public enable_shared_from_this<DBStatement>
{
public:
	DBStatement(shared_ptr<DBConnection> connection) : _connection(connection) { }
	~DBStatement();

	void Init();
	void Prepare(const string& query);
	shared_ptr<STATEMENT_Res> execute();

	void SetString(int index, const string& param);
	void SetInt(int index, const int& param);
	void SetFloat(int index, const float& param);
	void SetDouble(int index, const double& param);

public:
	void HandleError();
	MYSQL_STMT* GetStmt() { return _stmt; }

private:
	shared_ptr<DBConnection> _connection;
	vector<DB_PARAM> _params;

	MYSQL_STMT* _stmt = nullptr;
};
