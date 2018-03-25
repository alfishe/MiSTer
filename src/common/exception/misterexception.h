#ifndef COMMON_EXCEPTION_MISTEREXCEPTION_H_
#define COMMON_EXCEPTION_MISTEREXCEPTION_H_

#include <execinfo.h>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include "../../3rdparty/backward/backward.hpp"

using namespace std;
using namespace backward;

class MiSTerException : public exception
{
private:
    string reason_;

public:
    MiSTerException()
	{
		grabBacktrace();
	}

	explicit MiSTerException(const string &reason) : reason_(reason)
	{
		reason_.append(1, '\n');
	    grabBacktrace();
	}

	virtual ~MiSTerException() throw() {};

	virtual const char* what() const throw()
	{
		return reason_.c_str();
	}

private:
    void grabBacktrace()
    {
		//void *array[20];
		//size_t size;
    	  	//size = backtrace(array, sizeof(array) / sizeof(array[0]));

    		ostringstream stream;
		StackTrace st;
		st.load_here(20);
		st.skip_n_firsts(4);

		Printer p;
		p.snippet = true;
		p.object = false;
		p.color_mode = ColorMode::never;
		p.address = false;
		p.ascending = false;
		p.print(st, stream);
		reason_ += stream.str();
    }
};

#endif /* COMMON_EXCEPTION_MISTEREXCEPTION_H_ */
