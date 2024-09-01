#pragma once
#include "ResourceBase.h"

struct GridNode
{
	Vector pos;
	float z;
	EGridType value;
	EGridType around[8];
};

struct EnemyInfo
{
	Vector Location;
	float z;
	float yaw;
	Protocol::CasterType casterType;
	int32 phaseNumber;
};

class Map : public ResourceBase
{
public:
	Map() { }
	virtual ~Map() { }

	virtual void Load(const filesystem::path& path) override;
	
public:
	Vector GetGridSize() { return _gridSize; }
	Vector GetMapSize() { return _mapSize; }
	Vector GetPos() { return _pos; }
	Vector GetStartPoint() { return _startPoint; }
	Bound  GetBound() { return _bound; }
	GridNode GetNode(VectorInt pos);

	vector<EnemyInfo>& GetEnemies() { return _enemies; }

	string GetName() { return _name; }
public:
	bool IsValidAtGridPos(VectorInt gridPos);
	bool IsValidToDirection(VectorInt current, int32 dir);
	bool IsWalkableAtGridPos(VectorInt gridPos);
	bool ConvertToGridPos(Vector pos, VectorInt& gridPos);
	
private:
	Vector							_gridSize;
	Vector							_mapSize;
	Vector							_pos;
	Vector							_startPoint;
	Bound							_bound;
	vector<vector<GridNode>>		_nodes;

	vector<EnemyInfo>				_enemies;

	string							_name;

private:
	EMapType						_mapType;
};

