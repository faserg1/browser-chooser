module;

#include <vector>
#include <string>

export module CmdLineParser;
import Launch;

export class CmdLineParser
{
public:
	CmdLineParser(int argc, char** argv);

	LaunchType getType() const;
	std::string_view getAppPath() const;
	std::string_view getUrl() const;
private:
	std::vector<std::string_view> parseRaw(int argc, char** argv);
	void parse();
private:
	const std::vector<std::string_view> cmdline_;
	LaunchType type_;
	std::string_view url_;

};

CmdLineParser::CmdLineParser(int argc, char** argv) : cmdline_(parseRaw(argc, argv)), type_(LaunchType::Unknown)
{
	parse();
}

LaunchType CmdLineParser::getType() const
{
	return type_;
}

std::string_view CmdLineParser::getAppPath() const
{
	return cmdline_[0];
}

std::string_view CmdLineParser::getUrl() const
{
	return url_;
}

std::vector<std::string_view> CmdLineParser::parseRaw(int argc, char** argv)
{
	std::vector<std::string_view> cmdline;
	cmdline.reserve(argc);
	for (int i = 0; i < argc; i++)
		cmdline.emplace_back(argv[i]);
	return std::move(cmdline);
}

void CmdLineParser::parse()
{
	if (cmdline_.size() == 1)
	{
		type_ = LaunchType::Setup;
		return;
	}
	if (cmdline_[1] == "--unregister")
	{
		type_ = LaunchType::Delete;
		return;
	}
	if (cmdline_[1] == "--list")
	{
		type_ = LaunchType::BrowserList;
		return;
	}
	if (cmdline_[1] == "--default")
	{
		type_ = LaunchType::OpenDefault;
		return;
	}
	if (cmdline_.size() != 3)
		return;
	if (cmdline_[1] == "--open")
	{
		type_ = LaunchType::Open;
		url_ = cmdline_[2];
		return;
	}

}
