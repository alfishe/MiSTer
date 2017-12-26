#include "path.h"

#include <sstream>
#include <vector>

Path::Path(const string &p) : path(p)
{
}

string Path::toString() const
{
    return path;
}

Path& Path::combine(const string& rhs)
{
	this->path = (this->path / rhs).toString();

	return *this;
}

Path& Path::simplify()
{
	string result;

	stringstream ss(this->path);
	string part;
	vector<string> stack;

	// Split path into parts and push into stack
	while (getline(ss, part, separator))
	{
		if (part != "" && part != ".")
		{
			if (part == ".." and !stack.empty())
				stack.pop_back();
			else if (part != "..")
				stack.push_back(part);
		}
	}

	// Re-generate path from the stack
	for (auto str : stack)
		result += "/" + str;

	if (result.empty())
		result = "/";

	this->path = result;

	return *this;
}

Path Path::combine(const string& lhs, const string& rhs)
{
	Path result = lhs / rhs;

	return result;
}

// =============== Operators =====================

Path operator /(const Path& lhs, const Path& rhs)
{
	Path result = lhs.toString() / rhs.toString();

	return result;
}

Path operator /(const Path& lhs, const string& rhs)
{
	Path result = lhs.toString() / rhs;

	return result;
}

Path operator /(const string& lhs, const string& rhs)
{
	string res(lhs);
	if (!res.empty() && res.back() != Path::separator)
		res += Path::separator;
	res += rhs;

	Path result(res);

	return result;
}

