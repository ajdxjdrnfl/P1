#pragma once

class ResourceBase
{
public:
	ResourceBase() { };
	virtual ~ResourceBase() { };

	virtual void Load(const string& path) = 0;
	virtual void Save(const string& path) { }

};

