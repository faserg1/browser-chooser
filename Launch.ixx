module;

#include <string>
#include <ranges>
#include <algorithm>
#include <regex>
#include <Windows.h>
#include <shellapi.h>

export module Launch;
import Config;
import FastRegistry;
import Utils;

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
private:
	std::string chooseBrowser(std::string_view link);
	std::string_view getProtocol(std::string_view link);
	std::string_view getDomain(std::string_view link);
	std::string_view getPath(std::string_view link);
	std::string getBrowserType(std::string_view protocol, std::string browser);
	std::string getExecLink(std::string type, std::string_view link);
	void startApp(std::string execLink);
};

Launcher::Launcher(ConfigData cfg) : cfg_(std::move(cfg))
{
}

void Launcher::launch(std::string_view link)
{
	auto browser = chooseBrowser(link);
	auto protocol = getProtocol(link);
	auto domain = getDomain(link);
	auto path = getPath(link);
	auto type = getBrowserType(protocol, browser);
	auto execLink = getExecLink(type, link);
 	startApp(execLink);
}

std::string Launcher::chooseBrowser(std::string_view link)
{
	// Stub
	return "Google Chrome";
}

std::string_view Launcher::getProtocol(std::string_view link)
{
	auto protoView = link | std::views::take_while([](char n) -> bool {return n != ':'; }) | std::views::common;
	return std::string_view(&*protoView.begin(), std::ranges::distance(protoView));
}

std::string_view Launcher::getDomain(std::string_view link)
{
	auto domainView = link 
		| std::views::drop_while([](char n) -> bool {return n != ':'; })
		| std::views::drop_while([](char n) -> bool {return n == ':' || n == '/'; })
		| std::views::common;
	return std::string_view(&*domainView.begin(), std::ranges::distance(domainView));
}

std::string_view Launcher::getPath(std::string_view link)
{
	auto pathView = link
		| std::views::drop_while([](char n) -> bool {return n != ':'; })
		| std::views::drop_while([](char n) -> bool {return n == ':' || n == '/'; })
		| std::views::drop_while([](char n) -> bool {return n != '/'; })
		| std::views::take_while([](char n) -> bool {return n != '?'; })
		| std::views::common;
	return std::string_view(&*pathView.begin(), std::ranges::distance(pathView));
}

std::string Launcher::getBrowserType(std::string_view protocol, std::string browser)
{
	auto key = Key::RootOpen(RootKey::LOCAL_MACHINE, "SOFTWARE\\Clients\\StartMenuInternet\\" + browser + "\\Capabilities\\URLAssociations");
	return key.GetValue({ protocol.data(), protocol.size() }).getString();
}

std::string Launcher::getExecLink(std::string type, std::string_view link)
{
	using namespace std::string_literals;
	auto path = type + "\\shell\\open\\command"s;
	auto key = Key::RootOpen(RootKey::CLASSES_ROOT, path);
	auto formatString = key.GetValue({}).getString();
	return std::regex_replace(formatString, std::regex("%1"), std::string{link.data(), link.size()});
}

void Launcher::startApp(std::string execLink)
{
	std::wstring wCmdLine(execLink.begin(), execLink.end());
	int argc;
	auto argv = CommandLineToArgvW(wCmdLine.c_str(), &argc);
	auto cmdFileW = std::wstring(argv[0]);
	LocalFree(argv);
	auto cmdFileQ1 = utils::bake<char>(execLink
		| std::views::take(cmdFileW.size())
		| std::views::common);
	auto cmdFileQ2 = utils::bake<char>(execLink
		| std::views::drop(cmdFileW.size())
		| std::views::take_while([](char n) {return n != ' '; })
		| std::views::common);
	auto cmdFileQ = cmdFileQ1 + cmdFileQ2;
	auto params = execLink.substr(cmdFileQ.size());
	auto cmdFileView = cmdFileQ
		| std::views::filter([](char n) {return n != '\"'; })
		| std::views::common;
	auto cmdFile = std::string(&*cmdFileView.begin(), std::ranges::distance(cmdFileView));
	auto instance = ShellExecuteA(NULL, "open", cmdFile.c_str(), params.c_str(), NULL, 0);
}
