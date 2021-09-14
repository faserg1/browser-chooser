module;

#include <string>
#include <map>
#include <vector>

export module Config;

export struct Filter
{
	std::string protocol;
	std::string domain;
	std::string path;
};

export struct ConfigData
{
	
};