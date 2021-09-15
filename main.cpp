import CmdLineParser;
import Setup;
import ConfigReader;
import Launch;

#include <iostream>

int main(int argc, char **argv)
{
	CmdLineParser cmd(argc, argv);
	switch (cmd.getType())
	{
	case LaunchType::Setup:
		Setup::RunSetup(cmd.getAppPath());
		break;
	case LaunchType::Delete:
		Setup::RunDelete(false);
		break;
	case LaunchType::Config:
		ConfigReader::ApplyConfig(cmd.getConfigEdit());
		break;
	case LaunchType::Open:
	{
		Launcher l(ConfigReader::ReadConfig());
		l.launch(cmd.getUrl());
		break;
	}
	case LaunchType::BrowserList:
	{
		auto list = ConfigReader::ListBrowsers();
		std::cout << "Available browsers:\n";
		for (auto browser : list)
		{
			std::cout << browser << "\n";
		}
		std::cout << "Press any key to continue";
		std::cin.get();
		break;
	}
	default:
		break;
	}
	return 0;
}