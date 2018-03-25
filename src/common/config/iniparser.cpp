#include "iniparser.h"

#include "../logger/logger.h"

#include <fstream>
#include "../file/filemanager.h"
#include "../../3rdparty/json/json.hpp"

using json = nlohmann::json;
using namespace std;

iniparser::iniparser(char *filepath)
{

}

bool iniparser::parse()
{
	bool result = false;

	return result;
}

bool iniparser::getSection()
{
	bool result = false;

	return result;
}

bool iniparser::readJSON(string filename)
{
	bool result = false;

	if (!filename.empty())
	{
		try
		{
			ifstream ifs(filename);
			json j = json::parse(ifs);

			ifs.close();
		}
		catch(json::parse_error& e)
		{
			LOGERROR("Unable to read JSON configuration file '%s': %s", filename.c_str(), e.what());
		}
	}

	return result;
}

string iniparser::getJSONValue(string name)
{
	string result = "";

	return result;
}
