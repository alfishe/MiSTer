#ifndef COMMON_HELPERS_STRINGHELPER_H_
#define COMMON_HELPERS_STRINGHELPER_H_

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

using namespace std;

typedef vector<string> StringVector;

class StringHelper
{
public:
	static StringVector split(const string &s, char delimiter);
	static StringVector split(const string &s, char delimiter, StringVector* out);

private:
	StringHelper(); // Disable direct object creation from class
};

#endif /* COMMON_HELPERS_STRINGHELPER_H_ */
