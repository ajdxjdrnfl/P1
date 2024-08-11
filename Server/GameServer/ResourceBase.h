#pragma once

class ResourceBase
{
public:
	ResourceBase() { };
	virtual ~ResourceBase() { };

	virtual void Load(const filesystem::path& path) { }
	virtual void Save(const filesystem::path& path) { }

};

