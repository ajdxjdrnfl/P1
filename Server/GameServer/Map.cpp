#include "pch.h"
#include "Map.h"

void Map::Load(const filesystem::path& path)
{
	ifstream ifs;
	ifs.open(path);

	_name = path.stem().generic_string();
	
	// 맵 데이터 저장
	{
		int32 nodeCountX, nodeCountY;
		ifs >> _mapSize.x >> _mapSize.y >> _gridSize.x >> _gridSize.y >> _pos.x >> _pos.y >> nodeCountY >> nodeCountX;

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

				else if (value == 0)
				{
					for (int32 k = 0; k < 8; k++)
					{
						node.around[k] = (EGridType)0;
					}
				}
				
				nodes.push_back(node);
			}
			_nodes.push_back(nodes);
		}

	}

	_bound = Bound(_pos.x - _mapSize.x/2, _pos.y - _mapSize.y/2, 
		_pos.x + _mapSize.x/2, _pos.y + _mapSize.y);


	// 몬스터 저장
	{
		int32 enemyCount;
		ifs >> enemyCount;

		for (int32 i = 0; i < enemyCount; i++)
		{
			EnemyInfo info;
			int32 type;

			ifs >> info.Location.x >> info.Location.y >> info.z >> info.yaw >> type >> info.phaseNumber;

			info.casterType = (Protocol::CasterType)type;

			_enemies.push_back(info);
		}

	}
	ifs.close();
}

GridNode Map::GetNode(VectorInt pos)
{
	if (IsValidAtGridPos(pos))
		return _nodes[pos.y][pos.x];

	return GridNode();
}

bool Map::IsValidAtGridPos(VectorInt gridPos)
{
	int32 x = gridPos.x;
	int32 y = gridPos.y;

	if (x < 0 || x >= _nodes[0].size() || y < 0 || y >= _nodes.size())
		return false;

	switch (_nodes[y][x].value)
	{
	case EGT_Walkable:
		return true;
		
	case EGT_Blocked:
		return false;
	}
}

bool Map::IsValidToDirection(VectorInt current, int32 dir)
{
	if (!IsValidAtGridPos(current))
		return false;

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
