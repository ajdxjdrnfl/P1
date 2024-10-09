#include "pch.h"
#include "DBStatement.h"
#include "DBConnection.h"

DBStatement::~DBStatement()
{
	mysql_stmt_free_result(_stmt);
	mysql_stmt_close(_stmt);
	_params.clear();
}

void DBStatement::Init()
{
	_stmt = mysql_stmt_init(_connection->GetConnection());
}

void DBStatement::Prepare(const string& query)
{
	mysql_stmt_prepare(_stmt, query.c_str(), query.size());
	int param_count = mysql_stmt_param_count(_stmt);

	_params = vector<DB_PARAM>(param_count);
}

shared_ptr<STATEMENT_Res> DBStatement::execute()
{
	MYSQL_BIND* bind = new MYSQL_BIND[_params.size()]();

	for (int i = 0; i < _params.size(); i++)
	{
		bind[i].buffer_type = _params[i].type;
		bind[i].buffer = (void*)&_params[i].data;
		bind[i].length = &_params[i].length;
	}

	if (mysql_stmt_bind_param(_stmt, bind))
	{
		HandleError();
		return nullptr;
	}

	delete[] bind;

	if (mysql_stmt_execute(_stmt))
	{
		HandleError();
		return nullptr;
	}

	MYSQL_RES* metadata = mysql_stmt_result_metadata(_stmt);

	shared_ptr<STATEMENT_Res> res = make_shared<STATEMENT_Res>(metadata);
	res->_stmt = shared_from_this();

	int32 num_Fields = mysql_stmt_field_count(_stmt);

	MYSQL_FIELD* fields = mysql_fetch_fields(metadata);
	MYSQL_BIND* res_bind = new MYSQL_BIND[num_Fields]();
	
	for (int32 i = 0; i < num_Fields; i++)
	{
		string key = string(fields[i].name);
		
		res->_map[key].type = fields[i].type;
		res_bind[i].buffer_type = fields[i].type;

		switch (fields[i].type)
		{
		case MYSQL_TYPE_LONG:
			res->_map[key].buffer = (char*)(new int());
			res->_map[key].buffer_length = sizeof(double);
			break;
		case MYSQL_TYPE_FLOAT:
			res->_map[key].buffer = (char*)(new float());
			res->_map[key].buffer_length = sizeof(double);
			break;
		case MYSQL_TYPE_DOUBLE:
			res->_map[key].buffer = (char*)(new double());
			res->_map[key].buffer_length = sizeof(double);
			break;
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
			res->_map[key].buffer = new char[256]();
			res->_map[key].buffer_length = 256;
			res_bind[i].buffer_length = 256;
			break;
		default:
			cout << "ERROR: unexpected type: " << fields[i].type << endl;
			break;
		}
		res_bind[i].buffer = (void*)(res->_map[key].buffer);
		res_bind[i].length = &res->_map[key].length;
	}

 	int result = mysql_stmt_bind_result(_stmt, res_bind);
	if (result != 0) cout << "bind result failed" << endl;

	delete[] res_bind;

	int status = 0;

	return res;
}


void DBStatement::SetString(int index, const string& param)
{
	if (index <= 0 || index > _params.size())
	{
		cout << "Out Of Index" << endl;
		return;
	}

	index -= 1;

	_params[index].Set(param, param.size());

}

void DBStatement::SetInt(int index, const int& param)
{
	if (index <= 0 || index > _params.size())
	{
		cout << "Out Of Index" << endl;
		return;
	}

	index -= 1;

	_params[index].Set(param, 0);
}

void DBStatement::SetFloat(int index, const float& param)
{
	if (index <= 0 || index > _params.size())
	{
		cout << "Out Of Index" << endl;
		return;
	}

	index -= 1;
	_params[index].Set(param, 0);
}

void DBStatement::SetDouble(int index, const double& param)
{
	if (index <= 0 || index > _params.size())
	{
		cout << "Out Of Index" << endl;
		return;
	}
	index -= 1;
	_params[index].Set(param, 0);

}


void DBStatement::HandleError()
{
	cout << "Statement failed Error num : " << mysql_stmt_errno(_stmt) << endl;
	cout << "Statement Error : " << mysql_stmt_error(_stmt) << endl;
}

bool STATEMENT_Res::Next()
{
	int status = mysql_stmt_fetch(_stmt->GetStmt());

	if (status == 1 || status == MYSQL_NO_DATA)
		return false;

	for (auto& p : _map)
	{
		auto& result = p.second;
		if (result.type == MYSQL_TYPE_VARCHAR || result.type == MYSQL_TYPE_STRING || result.type == MYSQL_TYPE_VAR_STRING)
		{
			result.buffer[result.length] = '\0';
		}
	}

	return true;
}
