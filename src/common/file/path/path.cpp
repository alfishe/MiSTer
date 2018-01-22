#include "path.h"

#include <sstream>

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

string Path::getFileName()
{
	string result = getFileName(path);

	return result;
}

string Path::getExtension()
{
	string result = getExtension(path);

	return result;
}

string Path::getFileNameWithoutExtension()
{
	string result = getFileNameWithoutExtension(path);

	return result;
}

// Static methods
Path Path::combine(const string& lhs, const string& rhs)
{
	Path result = lhs / rhs;

	return result;
}

Path Path::simplify(string path)
{
	Path result(path);
	result.simplify();

	return result;
}

string Path::getFileName(const string& path)
{
	string result;

	vector<string> parts;
	split(path, '/', &parts);

	if (parts.size() > 0 && path.back() != '/')
	{
		result = parts.back();
	}

	return result;
}

string Path::getExtension(const string& path)
{
	string result;

	vector<string> parts;
	split(getFileName(path), '.', &parts);

	if (parts.size() > 1)
	{
		result = parts.back();
	}

	return result;
}

string Path::getFileNameWithoutExtension(const string& path)
{
	string result;

	vector<string> parts;
	split(getFileName(path), '.', &parts);

	if (parts.size() > 0)
	{
		result = parts.front();
	}

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

// ================= Helper methods ==============
void Path::split(const string &s, char delimiter, vector<string>* out)
{
	for (size_t start = 0, end = 0; start < s.length(); start = end + 1)
	{
		 size_t position = s.find(delimiter, start);
		 end = position != string::npos ? position : s.length();

		 string token = s.substr(start, end - start);
		 out->push_back(token);
	}
}
