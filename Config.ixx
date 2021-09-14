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

	int operator<(const Filter& f)
	{
		if (protocol == "*" && f.protocol != "*")
			return 1;
		if (f.protocol == "*" && protocol != "*")
			return -1;
		if (domain == "*" && f.domain != "*")
			return 1;
		if (f.domain == "*" && domain != "*")
			return -1;
		if (path == "*" && f.path != "*")
			return 1;
		if (f.path == "*" && path != "*")
			return -1;
		if (protocol != f.protocol)
			return protocol < f.protocol;
		if (domain != f.domain)
			return domain < f.domain;
		if (protocol != f.protocol)
			return path < f.path;
		return 0;
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