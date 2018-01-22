#ifndef COMMON_FILE_PATH_PATH_H_
#define COMMON_FILE_PATH_PATH_H_

#include <string>
#include <vector>

using namespace std;

class Path
{
public:
	static const char separator = '/';
	string path;

	Path(const string &p);
	string toString() const;
	Path& combine(const string& rhs);
	Path& simplify();
	string getFileName();
	string getExtension();
	string getFileNameWithoutExtension();

	static Path combine(const string& lhs, const string& rhs);
	static Path simplify(string path);
	static string getFileName(const string& path);
	static string getExtension(const string& path);
	static string getFileNameWithoutExtension(const string& path);

// Helper methods
protected:
	static void split(const string &s, char delimiter, vector<string>* out);
};

Path operator/(const Path& lhs, const Path& rhs);
Path operator/(const Path& lhs, const string& rhs);
Path operator/(const string& lhs, const string& rhs);

#endif /* COMMON_FILE_PATH_PATH_H_ */
