module;

#include <string>
#include <Windows.h>

export module Launch;
import Config;

export enum class LaunchType
{
	Unknown,
	Setup,
	Open,
	OpenDefault,
	Config,
	Delete,
	BrowserList
};

export class Launcher
{
public:
	Launcher(ConfigData cfg);
	void launch(std::string_view link);
private:
	const ConfigData cfg_;
};

Launcher::Launcher(ConfigData cfg) : cfg_(std::move(cfg))
{
}

void Launcher::launch(std::string_view link)
{
}
