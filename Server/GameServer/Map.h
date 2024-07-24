#pragma once
#include "ResourceBase.h"

class Map : public ResourceBase
{
public:
	Map();
	virtual ~Map();

	virtual void Load(const wstring& path);

};

