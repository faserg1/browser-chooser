module;

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <any>

#include <windows.h>

export module FastRegistry;
import RegistryValue;

export enum class RootKey : LONG
{
	CLASSES_ROOT = (LONG) HKEY_CLASSES_ROOT,
	CURRENT_CONFIG = (LONG) HKEY_CURRENT_CONFIG,
	CURRENT_USER = (LONG) HKEY_CURRENT_USER,
	LOCAL_MACHINE = (LONG) HKEY_LOCAL_MACHINE,
	USERS = (LONG) HKEY_USERS
};

export enum class Access
{
	Read,
	ReadWrite
};

export class Key
{
public:
	Key(Key&& old);
	~Key();

	static Key RootOpen(RootKey parent, std::string path, Access = Access::Read);
	static Key RootCreate(RootKey parent, std::string sub, Access = Access::Read);

	Key Open(std::string path, Access = Access::Read);
	Key Create(std::string sub, Access = Access::Read);
	std::vector<std::string> EnumerateSubKeys();
	std::map<std::string, Value> EnumerateValues();
	Value GetValue(std::string parameter);
	void SetValue(std::string parameter, Value v);
private:
	Key(HKEY key, std::string path);
	
	Key(const Key& old) = delete;
	Key *operator=(const Key& old) = delete;
private:
	HKEY key_;
	std::string path_;
private:
	static LSTATUS InternalOpen(HKEY parent, std::string path, Access access, PHKEY result);
	static LSTATUS InternalCreate(HKEY parent, std::string sub, Access access, PHKEY result);
	static DWORD ConvertAccess(Access access);
	static std::string ConvertRoot(RootKey key);
	static std::string MakeError(std::string path, LSTATUS code);
};

Key::Key(HKEY key, std::string path) : key_(key), path_(path)
{
}

Key::Key(Key&& old) : key_(old.key_), path_(std::move(old.path_))
{
	old.key_ = NULL;
}

Key::~Key()
{
	if (key_)
		RegCloseKey(key_);
}

Key Key::Open(std::string path, Access access)
{
	HKEY result;
	auto totalpath = this->path_ + "\\" + path;
	auto stat = InternalOpen(this->key_, path, access, &result);
	if (stat != ERROR_SUCCESS)
		throw std::runtime_error(MakeError(totalpath, stat));
	return Key(result, totalpath);
}

Key Key::Create(std::string sub, Access access)
{
	HKEY result;
	auto totalpath = this->path_ + "\\" + sub;
	auto stat = InternalCreate(this->key_, sub, access, &result);
	if (stat != ERROR_SUCCESS)
		throw std::runtime_error(MakeError(totalpath, stat));
	return Key(result, totalpath);
}

std::vector<std::string> Key::EnumerateSubKeys()
{
	std::vector<std::string> subkeys;
	LSTATUS status;
	DWORD idx = 0;
	do {
		char buf[255];
		DWORD size = 255;
		status = RegEnumKeyExA(this->key_, idx++, buf, &size, NULL, NULL, NULL, NULL);
		if (status == ERROR_SUCCESS)
			subkeys.emplace_back(buf, size);
	} while (status == ERROR_SUCCESS);
	return subkeys;
}

std::map<std::string, Value> Key::EnumerateValues()
{
	auto map = std::map<std::string, Value>();
	LSTATUS status;
	DWORD idx = 0;
	do {
		constexpr const auto valueDefault = 2048;
		char bufName[16383];
		std::vector<std::byte> bufValue(valueDefault);
		DWORD sizeName = 16383, sizeValue = valueDefault;
		DWORD type;

		status = RegEnumValueA(this->key_, idx++, bufName, &sizeName, NULL, &type, reinterpret_cast<LPBYTE>(bufValue.data()), &sizeValue);
		if (status == ERROR_SUCCESS)
			map.insert(std::make_pair(std::string(bufName, sizeName), Value::ConstructFrom(type, bufValue, sizeValue)));
	} while (status == ERROR_SUCCESS);
	return map;
}

Value Key::GetValue(std::string parameter)
{
	LSTATUS status;
	LPCSTR valName = (parameter.size() == 0 ? NULL : parameter.data());
	constexpr const auto valueDefault = 2048;
	return {};
}

void Key::SetValue(std::string parameter, Value v)
{
	if (parameter.size() == 0 && v.type_ != REG_SZ)
		throw std::logic_error("Invalid type to set default value");
	LSTATUS status;
	auto data = v.ToBinary();
	LPCSTR valName = (parameter.size() == 0 ? NULL : parameter.data());
	status = RegSetKeyValueA(this->key_, NULL, valName, v.type_, reinterpret_cast<LPBYTE>(data.data()), data.size());
	if (status != ERROR_SUCCESS)
		throw std::runtime_error("Cannot set parameter \"" + parameter + "\"");
}

LSTATUS Key::InternalOpen(HKEY parent, std::string path, Access access, PHKEY result)
{
	return RegOpenKeyExA(parent, path.data(), 0, ConvertAccess(access), result);
}

LSTATUS Key::InternalCreate(HKEY parent, std::string sub, Access access, PHKEY key)
{
	DWORD result;
	return RegCreateKeyExA(parent, sub.data(), 0, NULL, REG_OPTION_NON_VOLATILE, ConvertAccess(access), NULL, key, &result);
}

DWORD Key::ConvertAccess(Access access)
{
	switch (access)
	{
	case Access::Read:
		return KEY_READ;
	case Access::ReadWrite:
		return KEY_READ | KEY_WRITE;
	default:
		return 0;
	}
}

std::string Key::ConvertRoot(RootKey key)
{
	switch (key)
	{
	case RootKey::CLASSES_ROOT:
		return "HKEY_CLASSES_ROOT";
	case RootKey::CURRENT_CONFIG:
		return "HKEY_CURRENT_CONFIG";
	case RootKey::CURRENT_USER:
		return "HKEY_CURRENT_USER";
	case RootKey::LOCAL_MACHINE:
		return "HKEY_LOCAL_MACHINE";
	case RootKey::USERS:
		return "HKEY_USERS";
	}
	return "";
}

std::string Key::MakeError(std::string path, LSTATUS code)
{
	return std::string("Unable to create or open \"" + path + "\", code: " + std::to_string(code));
}

export Key Key::RootOpen(RootKey root, std::string path, Access access)
{
	HKEY result;
	auto totalpath = ConvertRoot(root) + "\\" + path;
	auto stat = InternalOpen(reinterpret_cast<HKEY>(root), path, access, &result);
	if (stat != ERROR_SUCCESS)
		throw std::runtime_error(MakeError(totalpath, stat));
	return Key(result, totalpath);
}

export Key Key::RootCreate(RootKey root, std::string sub, Access access)
{
	HKEY result;
	auto totalpath = ConvertRoot(root) + "\\" + sub;
	auto stat = InternalCreate(reinterpret_cast<HKEY>(root), sub, access, &result);
	if (stat != ERROR_SUCCESS)
		throw std::runtime_error(MakeError(totalpath, stat));
	return Key(result, totalpath);
}

