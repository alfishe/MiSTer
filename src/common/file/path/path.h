#ifndef COMMON_FILE_PATH_PATH_H_
#define COMMON_FILE_PATH_PATH_H_

#include <string>

using namespace std;

class Path
{
public:
	static const char separator = '/';
	string path;

	Path(const string &p);
	string toString() const;
	Path combine(const string& rhs);

	static Path combine(const string& lhs, const string& rhs);
};

Path operator/(const Path& lhs, const Path& rhs);
Path operator/(const Path& lhs, const string& rhs);
Path operator/(const string& lhs, const string& rhs);

#endif /* COMMON_FILE_PATH_PATH_H_ */
