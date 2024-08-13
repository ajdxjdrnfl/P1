#pragma once
#include "ResourceBase.h"

struct GridNode
{
	Vector pos;
	float z;
	EGridType value;
	EGridType around[8];
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
	Bound  GetBound() { return _bound; }
	GridNode GetNode(VectorInt pos) { return _nodes[pos.y][pos.x]; }

	string GetName() { return _name; }
public:
	bool IsValidAtGridPos(VectorInt gridPos);
	bool IsValidToDirection(VectorInt current, int32 dir);
	bool ConvertToGridPos(Vector pos, VectorInt& gridPos);
	
private:
	Vector							_gridSize;
	Vector							_mapSize;
	Vector							_pos;
	Bound							_bound;
	vector<vector<GridNode>>		_nodes;

	string							_name;
};

