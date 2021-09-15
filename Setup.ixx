module;

#include <string>
#include <windows.h>

export module Setup;
import FastRegistry;

using namespace std::string_literals;

export class Setup
{
public:
	static bool RunSetup(std::string_view appPath);
	static bool RunDelete(bool withConfig);
private:
	static void CheckOrCreateConfigKey();
	static void CheckOrCreateShellClass(std::string appPath);
	static void CheckOrCreateStartMenuInternet(std::string appPath);
	static void CheckOrRegisterApplication();
	static bool IsElevated();
};

bool Setup::RunSetup(std::string_view appPath)
{
	if (!IsElevated())
		return false;
	CheckOrCreateConfigKey();
	CheckOrCreateShellClass(std::string(appPath));
	CheckOrCreateStartMenuInternet(std::string(appPath));
	CheckOrRegisterApplication();
	return false;
}

bool Setup::RunDelete(bool withConfig)
{
	return false;
}

void Setup::CheckOrCreateConfigKey()
{
	Key::RootCreate(RootKey::CURRENT_USER, "SOFTWARE\\BrowserChooser", Access::Read);
}

void Setup::CheckOrCreateShellClass(std::string appPath)
{
	auto shCl = Key::RootCreate(RootKey::CLASSES_ROOT, "BrowserChooser", Access::ReadWrite);
	shCl.SetValue({}, "Browser Chooser Trigger"s);
	shCl.SetValue("AppUserModelId", "BrowserChooser"s);
	auto app = shCl.Create("Application", Access::ReadWrite);
	app.SetValue("ApplicationIcon", appPath + ",0"s);
	app.SetValue("ApplicationName", "Browser Chooser"s);
	app.SetValue("AppUserModelId", "BrowserChooser"s);
	auto icon = shCl.Create("DefaultIcon", Access::ReadWrite);
	icon.SetValue({}, appPath + ",0"s);
	auto cmd = shCl.Create("shell", Access::ReadWrite).Create("open", Access::ReadWrite).Create("command", Access::ReadWrite);
	auto cmdPath = "\""s + appPath + "\" --open %1"s;
	cmd.SetValue({}, cmdPath);
}

void Setup::CheckOrCreateStartMenuInternet(std::string appPath)
{
	auto startMenu = Key::RootOpen(RootKey::LOCAL_MACHINE, "SOFTWARE\\Clients\\StartMenuInternet", Access::ReadWrite);
	auto ch = startMenu.Create("Browser Chooser");
	auto cmd = ch.Create("shell", Access::ReadWrite).Create("open", Access::ReadWrite).Create("command", Access::ReadWrite);
	auto cmdPath = "\""s + appPath + "\" --default";
	cmd.SetValue({}, cmdPath);
	auto cap = ch.Create("Capabilities", Access::ReadWrite);
	auto start = cap.Create("Startmenu", Access::ReadWrite);
	start.SetValue("StartMenuInternet", "Browser Chooser"s);
	auto url = cap.Create("URLAssociations", Access::ReadWrite);
	auto protocols = {"http", "https", "ftp", "irc", "mailto", "mms", "news", "nntp", "sms", "smsto", "snews", "tel", "urn", "webcal"};
	for (auto proto : protocols)
		url.SetValue(proto, "BrowserChooser"s);
}

void Setup::CheckOrRegisterApplication()
{
	auto key = Key::RootOpen(RootKey::LOCAL_MACHINE, "SOFTWARE\\RegisteredApplications", Access::ReadWrite);
	key.SetValue("Browser Chooser", "Software\\Clients\\StartMenuInternet\\Browser Chooser\\Capabilities"s);
}

bool Setup::IsElevated()
{
	bool isElevated;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			isElevated = (bool) Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return isElevated;
}
