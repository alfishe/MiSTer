#include "stringhelper.h"

// Public methods

/*
 * Splits input string to parts using delimiter provided
 */
StringVector StringHelper::split(const string &s, char delimiter)
{
	StringVector result;

	split(s, delimiter, &result);

	return result;
}

StringVector StringHelper::split(const string &s, char delimiter, StringVector* out)
{
	for (size_t start = 0, end = 0; start < s.length(); start = end + 1)
	{
		 size_t position = s.find(delimiter, start);
		 end = position != string::npos ? position : s.length();

		 string token = s.substr(start, end - start);
		 out->push_back(token);
	}

	return *out;
}

/// Discards all non-printable characters from the string
/// Return new clean string
const string StringHelper::cleanup(const string &s)
{
	string result;
	result.resize(s.size());

	int idx = 0;
	for (const char& c : s)
	{
	    if (isprint(c))
	    {
	    		result[idx] = c;

	    		idx++;
	    }
	}

	result.resize(idx);

	return result;
}
