#include "Config.h"

#include "cinder/app/App.h"
#include "cinder/Utilities.h"
#include <sstream>
#include "cinder/Log.h"

using namespace ci::app;

Config::Config()
{
	
}

Config::~Config()
{

}

void Config::load()
{
	CI_LOG_I("starting load...");
	std::string f = ci::loadString( ci::loadFile("../resources/particle.cfg"));
	std::istringstream lines(f);
	std::string s;
	while (std::getline(lines, s,'\n'))
	{
		std::istringstream line(s);
		std::string key;
		if (std::getline(line, key, '='))
		{
			CI_LOG_I(key);
			std::string value;
			std::getline(line, value);
			config[key] = value;
			CI_LOG_I(value);
		}
	}
	CI_LOG_I("ending load.");
}