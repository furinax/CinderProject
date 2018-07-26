#pragma once

#include <map>
#include <string>

class Config{
public:
	Config();
	~Config();

	void load();

	std::map<std::string, std::string> config;
};