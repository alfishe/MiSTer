#include "path.h"

Path::Path(const string &p) : path(p)
{
}

string Path::toString() const
{
    return path;
}

Path Path::combine(const string& rhs)
{
	Path result = this->path / rhs;

	return result;
}

Path Path::combine(const string& lhs, const string& rhs)
{
	Path result = lhs / rhs;

	return result;
}

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

