module;

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <functional>
#include <algorithm>

#include <windows.h>

export module ConfigReader;

import Config;
import FastRegistry;

export class ConfigReader
{
public:
	static ConfigData ReadConfig();
	static void ApplyConfig(ConfigEdit edit);
	static std::vector<std::string> ListBrowsers();
private:
	static void PrintFilters();
	static void AddFilter(Filter filter);
	static void EditFilter(size_t num, Filter filter);
	static void DeleteFilter(size_t num);
};

constexpr const char *BrowserConfigPath = "SOFTWARE\\BrowserChooser";

template<typename T> void printColumn(T t, const int& width);

ConfigData ConfigReader::ReadConfig()
{
	ConfigData data{};
	auto key = Key::RootOpen(RootKey::CURRENT_USER, BrowserConfigPath);
	auto list = key.EnumerateSubKeys();
	for (auto sub : list)
	{
		auto subKey = key.Open(sub);
		auto map = subKey.EnumerateValues();
		Filter f;
		f.hash = sub;
		f.protocol = map.find("protocol")->second.getString();
		f.domain = map.find("domain")->second.getString();
		f.path = map.find("path")->second.getString();
		f.browser = map.find("browser")->second.getString();
		data.filters.push_back(f);
	}
	std::sort(data.filters.begin(), data.filters.end());
	return data;
}

void ConfigReader::ApplyConfig(ConfigEdit edit)
{
	
	switch (edit.type)
	{
	case ConfigEditType::List:
		PrintFilters();
		break;
	case ConfigEditType::Add:
		AddFilter(edit.f);
		break;
	case ConfigEditType::Edit:
		EditFilter(edit.deleteOrEditNum, edit.f);
		break;
	case ConfigEditType::Delete:
		DeleteFilter(edit.deleteOrEditNum);
		break;
	default:
		break;
	}
	std::cout << "Press any key to continue";
	std::cin.get();
}

std::vector<std::string> ConfigReader::ListBrowsers()
{
	return Key::RootOpen(RootKey::LOCAL_MACHINE, "SOFTWARE\\Clients\\StartMenuInternet").EnumerateSubKeys();
}

void ConfigReader::PrintFilters()
{
	constexpr auto colNum = 5, colProto = 8, colDomain = 20, colPath = 40, colBrowser = 10;
	auto cfg = ReadConfig();
	printColumn("Num", colNum);
	printColumn("Proto", colProto);
	printColumn("Domain", colDomain);
	printColumn("Path", colPath);
	printColumn("Browser", colBrowser);
	std::cout << '\n';
	int i = 0;
	for (auto& filter : cfg.filters)
	{
		printColumn(i++, colNum);
		printColumn(filter.protocol, colProto);
		printColumn(filter.domain, colDomain);
		printColumn(filter.path, colPath);
		printColumn(filter.browser, colBrowser);
		std::cout << '\n';
	}
}

void ConfigReader::AddFilter(Filter filter)
{
	auto str = filter.protocol + filter.domain + filter.path;
	auto hash = std::to_string(std::hash<std::string>{}(str));
	auto key = Key::RootOpen(RootKey::CURRENT_USER, BrowserConfigPath, Access::ReadWrite);
	auto keyFilter = key.Create(hash, Access::ReadWrite);
	keyFilter.SetValue("protocol", filter.protocol);
	keyFilter.SetValue("domain", filter.domain);
	keyFilter.SetValue("path", filter.path);
	keyFilter.SetValue("browser", filter.browser);
}

void ConfigReader::EditFilter(size_t num, Filter filter)
{
	DeleteFilter(num);
	AddFilter(filter);
}

void ConfigReader::DeleteFilter(size_t num)
{
	auto cfg = ReadConfig();
	auto hash = cfg.filters[num].hash;
	auto key = Key::RootOpen(RootKey::CURRENT_USER, BrowserConfigPath, Access::ReadWrite);
	key.Delete(hash);
}

template<typename T> void printColumn(T t, const int& width)
{
	std::cout << std::left << std::setw(width) << std::setfill(' ') << t;
}