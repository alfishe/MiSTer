#ifndef COMMON_HELPERS_STRINGHELPER_H_
#define COMMON_HELPERS_STRINGHELPER_H_

#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <regex>

#include "../types.h"

using namespace std;

class StringHelper
{
public:
	static StringVector split(const string &s, char delimiter);
	static StringVector split(const string &s, char delimiter, StringVector* out);

	static const string cleanup(const string &s);

	static const bool isMatch(const string&s, const string& pattern);

	static const int getIntegerRegex(const string& s, const string& pattern);
	static const string getStringRegex(const string& s, const string& pattern);

private:
	StringHelper(); // Disable direct object creation from class
};

#endif /* COMMON_HELPERS_STRINGHELPER_H_ */
