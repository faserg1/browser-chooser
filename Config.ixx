module;

#include <string>
#include <map>
#include <vector>
#include <compare>

export module Config;

export struct Filter
{
	std::string hash;
	std::string protocol;
	std::string domain;
	std::string path;
	std::string browser;

	bool operator<(const Filter& f)
	{
		if (protocol == "*" && f.protocol != "*")
			return true;
		if (f.protocol == "*" && protocol != "*")
			return false;
		if (domain == "*" && f.domain != "*")
			return true;
		if (f.domain == "*" && domain != "*")
			return false;
		if (path == "*" && f.path != "*")
			return true;
		if (f.path == "*" && path != "*")
			return false;
		if (protocol != f.protocol)
			return protocol < f.protocol;
		if (domain != f.domain)
			return domain < f.domain;
		if (protocol != f.protocol)
			return path < f.path;
		return false;
	}
	auto operator==(const Filter& f)
	{
		return protocol == f.protocol && domain == f.domain && path == f.path;
	}
};

export enum class ConfigEditType
{
	List,
	Add,
	Edit,
	Delete
};

export struct ConfigEdit
{
	ConfigEditType type;
	Filter f;
	size_t deleteOrEditNum;
};

export struct ConfigData
{
	std::vector<Filter> filters;
};