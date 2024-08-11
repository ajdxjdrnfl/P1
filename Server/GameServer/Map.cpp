#include "pch.h"
#include "Map.h"

void Map::Load(const filesystem::path& path)
{
	ifstream ifs;
	ifs.open(path);

	_name = path.stem().generic_string();
	{
		int32 nodeCountX, nodeCountY;
		ifs >> _mapSize.x >> _mapSize.y >> _gridSize.x >> _gridSize.y >> _pos.x >> _pos.y >> nodeCountX >> nodeCountY;

		for (int32 i = 0; i < nodeCountY; i++)
		{
			vector<GridNode> nodes;
			for (int32 j = 0; j < nodeCountX; j++)
			{
				GridNode node;
				int32 value;
				ifs >> node.pos.x >> node.pos.y >> node.z;
				ifs >> value;
				node.value = (EGridType)value;

				if (value == 1)
				{
					for (int32 k = 0; k < 8; k++)
					{
						int32 aroundValue;
						ifs >> aroundValue;
						node.around[k] = (EGridType)aroundValue;
					}
				}
				
				nodes.push_back(node);
			}
			_nodes.push_back(nodes);
		}

	}

	ifs.close();
}

bool Map::IsValidAtGridPos(VectorInt gridPos)
{
	int32 x = gridPos.x;
	int32 y = gridPos.y;

	if (x < 0 || x >= _nodes.size() || y < 0 || y >= _nodes[0].size())
		return false;

	switch (_nodes[x][y].value)
	{
	case EGT_Walkable:
		return true;
		
	case EGT_Blocked:
		return false;
	}
}

bool Map::IsValidToDirection(VectorInt current, int32 dir)
{
	GridNode node = GetNode(current);

	if (node.value == EGT_Blocked)
		return false;
	else
	{
		if (node.around[dir] == EGT_Blocked)
			return false;
		else return true;

	}
}

bool Map::ConvertToGridPos(Vector pos, VectorInt& gridPos)
{

	// TODO : 범위를 넘어갔을때를 위한 예외처리

	Vector mapPos = _pos - Vector(_mapSize.x/2, _mapSize.y/2);

	Vector relativePos = pos - mapPos;

	if (relativePos.x < 0.f || relativePos.x > _mapSize.x || relativePos.y < 0.f || relativePos.y > _mapSize.y)
		return false;

	int32 x = relativePos.x / _gridSize.x;
	int32 y = relativePos.y / _gridSize.y;

	gridPos = { x, y };
	return true;
}
