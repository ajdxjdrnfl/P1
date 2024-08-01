#pragma once

class ResourceBase
{
public:
	ResourceBase() { };
	virtual ~ResourceBase() { };

	virtual void Load(const string& path) { }
	virtual void Save(const string& path) { }

};

