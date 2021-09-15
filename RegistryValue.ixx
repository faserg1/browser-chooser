module;

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <any>

#include <windows.h>


export module RegistryValue;

export class Value
{
	friend class Key;
	Value() = default;
public:
	Value(std::string val);
	std::string getString();
	std::vector<std::byte> getBinary();
private:
	DWORD type_;
	std::any value_;
private:
	static Value ConstructFrom(DWORD type, std::vector<std::byte> bytes, DWORD readed);
	std::vector<std::byte> ToBinary();
};

Value::Value(std::string val)
{
	type_ = REG_SZ;
	value_ = val;
}

std::string Value::getString()
{
	if (type_ != REG_SZ)
		throw std::runtime_error("Invalid type!");
	return std::any_cast<std::string>(value_);
}

std::vector<std::byte> Value::getBinary()
{
	if (type_ != REG_BINARY)
		throw std::runtime_error("Invalid type!");
	return std::any_cast<std::vector<std::byte>>(value_);
}

Value Value::ConstructFrom(DWORD type, std::vector<std::byte> bytes, DWORD readed)
{
	auto v = Value();
	v.type_ = type;
	bytes.resize(readed);
	switch (type)
	{
	case REG_SZ:
		v.value_ = std::string(reinterpret_cast<char*>(bytes.data()), strlen(reinterpret_cast<char*>(bytes.data())));
		break;
	case REG_BINARY:
		v.value_ = bytes;
		break;
	default:
		throw std::logic_error("Unsupported type!");
	}
	return v;
}

std::vector<std::byte> Value::ToBinary()
{
	switch (type_)
	{
	case REG_SZ:
	{
		auto str = any_cast<std::string>(value_);
		auto vec = std::vector<std::byte>(str.size());
		str.copy(reinterpret_cast<char *>(vec.data()), str.size());
		vec.push_back(std::byte{});
		return vec;
	}
	case REG_BINARY:
		return any_cast<std::vector<std::byte>>(value_);
	default:
		throw std::logic_error("Unsupported type!");
	}
	return {};
}
