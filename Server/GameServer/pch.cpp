#include "pch.h"
#include "DBConnectionPool.h"

DBConnectionPool* GDatabase = new DBConnectionPool(DB_CONNECTION_NUM);