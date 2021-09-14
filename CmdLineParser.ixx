module;

#include <vector>
#include <string>
#include <ranges>
#include <algorithm>

export module CmdLineParser;
import Launch;
import Config;

export class CmdLineParser
{
public:
	CmdLineParser(int argc, char** argv);

	LaunchType getType() const;
	std::string_view getAppPath() const;
	std::string_view getUrl() const;
	ConfigEdit getConfigEdit() const;
private:
	std::vector<std::string_view> parseRaw(int argc, char** argv);
	void parse();
	Filter parseFilter(std::string rawFilter);
private:
	const std::vector<std::string_view> cmdline_;
	LaunchType type_;
	std::string_view url_;
	ConfigEdit cfg_;

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

ConfigEdit CmdLineParser::getConfigEdit() const
{
	return cfg_;
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
	if (cmdline_[1] == "--cfg")
	{
		type_ = LaunchType::Config;
		cfg_ = {};
		std::string rawcfg = std::string(cmdline_[2]);
		if (rawcfg == "@")
		{
			cfg_.type = ConfigEditType::List;
			return;
		}
		if (rawcfg.starts_with("+") )
		{
			cfg_.type = ConfigEditType::Add;
			auto filterView = cmdline_[2] | std::views::drop_while([](char n) -> bool {return n != '#'; }) | std::views::drop(1);
			auto filter = std::string(filterView.begin(), filterView.end());
			cfg_.f = parseFilter(filter);
			return;
		}
		if (rawcfg.starts_with("-") || rawcfg.starts_with("="))
		{
			cfg_.type = rawcfg.starts_with("-") ? ConfigEditType::Delete : ConfigEditType::Edit;
			
			auto numView = rawcfg | std::views::drop(1) | std::views::take_while([](char n) -> bool {return n != '#'; }) | std::views::common;
			auto filterView = rawcfg | std::views::drop_while([](char n) -> bool {return n != '#'; }) | std::views::drop(1);
			auto filter = std::string(filterView.begin(), filterView.end());
			cfg_.deleteOrEditNum = std::stoul(std::string(numView.begin(), numView.end()));
			cfg_.f = parseFilter(filter);
			return;
		}
		return;
	}
}

Filter CmdLineParser::parseFilter(std::string rawFilter)
{
	auto f = Filter();
	auto listView = rawFilter | std::views::split('`') | std::views::transform([](auto&& str) { return std::string_view(&*str.begin(), std::ranges::distance(str)); });
	std::vector <std::string_view> list(listView.begin(), listView.end());
	f.protocol = list[0];
	f.domain = list[1];
	f.path = list[2];
	f.browser = list[3];
	return f;
}
