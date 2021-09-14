module;

#include <string>
#include <map>
#include <vector>

#include <windows.h>

export module ConfigReader;

import Config;
import FastRegistry;

export class ConfigReader
{
public:
	static ConfigData ReadConfig()
	{
		auto key = Key::RootOpen(RootKey::CURRENT_USER, "SOFTWARE\\BrowserChooser");
		return {};
	}
	static std::vector<std::string> ListBrowsers()
	{
		return Key::RootOpen(RootKey::LOCAL_MACHINE, "SOFTWARE\\Clients\\StartMenuInternet").EnumerateSubKeys();
	}
};