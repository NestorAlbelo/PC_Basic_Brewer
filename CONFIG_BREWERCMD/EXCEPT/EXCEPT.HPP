/*  Except.hpp

    Copyright 1995 by Sci-Tec Inc.

            Exceptions
    These classes support a standard method of error handling.

    These classes now derive from the C++ class xmsg.

FILES
    This .hpp file is implemented in a number of .CPP files:

    ExcHelp.cpp:   One of these files is always required.  ExcHlpLt does
    ExcHlpLt.cpp:   not translate, so if your program doesn't use .INI files,
                    this results in a lot less overhead.

    One of the following files must be included, or you must write your own.
    Each one contains a different implementation of the
    BaseException::display() function.

    ExcDCmd.cpp:   For line oriented applications.
    ExcDTV.cpp:    For Turbo Vision applications.
    ExcDWin.cpp:   For Windows applications.

    One of the following files must be included, or you must write your own.
    Each one contains a different implementation of the
    BaseException::log() function.

    ExcNoLog.cpp:  For applications which do not wish to log errors.
    ExcLFile.cpp:  For applications which wish to log to a file.
    ExcLCerr.cpp:  For applications which wish to log to stderr.


OVERVIEW
    There are two sets of classes provides: BaseException and some general error
exceptions.  Derived exception classes in packages should be derived from the
general error exceptions, i.e.

BaseException       <- ExcGeneral    <- SpecificError
(the base class)    (general errors)  (specific errors)
(defined here)      (defined here)    (defined in your package
                                         except for debugging exceptiones
                                         defined in this package).

    If there is no general error exception class that is
appropriate, one should be added.  Be sure to update the revision number with all
that entails.

CREATING AND USING A SPECIFIC ERROR CLASS
    To create a new specific error class, derive a new class from one or more
of the general error classes.  Deriving a new specific error class is
relatively simple.  All that is probably needed is a constructor.

A typical specific error declaration would be:
class IniNotFound : public ExcFileNotFound, public virtual BaseException,
{
    public:
        IniNotFound(char *filename) :
            BaseException(helpdescribe("%s not found", filename)) {}
};

    Use helpdescribe() to do the work for you when creating the constructor.
It works similarly to a sprintf(), but it gets its format string
from the [Errors] section of your language file.

This class could be used in this example code snippet:
    if (!fs.good()) throw IniNotFound(filename);

The class may be inherited from more than one general class:

class QSerialTimeout : public ExcTimeout, public ExcCommError,
            public virtual BaseException  {...};

CREATING A GENERAL ERROR CLASS
    General error classes are usually empty classes, included mainly so that
it is possible to catch a class of errors.  See this file for examples.
This format must be followed, see Section r.12.6.2 of Stroustrup for
justification.

USING AND CATCHING EXCEPTIONS
Programs that include packages using exception would typically trap
BaseException in main() and specific errors where required.
display(), log() and why() are provided for your use, but they
do not have to be called.

For example:

main()
{
	try
	{
		iniStream ini;
		.
		.
		.
		try {ini.open(filen);}
		catch (IniNotFound& err)
		{
			ofstream fs(filen);
			fs << ";default ini file\non=1\n";
			fs.close();
			ini.open(filen);
		}
		.
		.
		.
	}
	catch(BaseException& err)
	{
		err.log();
		err.display();
	}
}

The outer try and catch blocks should be a part of all programs using exceptions.

        */

#ifndef __EXCEPT_HPP
#define __EXCEPT_HPP

#ifndef __cplusplus
#error  Requires C++ Compiler
#endif

#ifdef __TCPLUSPLUS__
    #if (__TCPLUSPLUS__ <= 0x300)
    #error  Requires a C++ Compiler that supports exceptions
    #else
    #define HAS_EXCEPTIONS
    #endif
#else
#define HAS_EXCEPTIONS
#endif

#include <cstring.h>
#include <iostream.h>
#include <except.h>

//  The base class
extern const string nullstring;

class BaseException : public xmsg
{
    protected:
        string helpdescribe(const char *keyword, ...);
                // looks up keyword in the ini file,
                // and then passes lookup and the parameters to sprintf

    public:
        BaseException (const string& msg) : xmsg(msg) {}
        void log();
        void display();
};

// the general errors.  Inherit from these to get specific errors.

class ExcFileNotFound : virtual public BaseException
{
    public:
        ExcFileNotFound(const string& msg) : BaseException(msg) {}
        ExcFileNotFound() : BaseException(nullstring) {}
                // BaseException(nullstring) should never be called even though
                // ExcFileNotFound() may be called if directions for
                // deriving base classes are followed.  see r12.6.2
};

class ExcSyntaxError : virtual public BaseException
{
    public:
        ExcSyntaxError(const string& msg) : BaseException(msg) {}
        ExcSyntaxError() : BaseException(nullstring) {}
                // BaseException(nullstring) should never be called even though
                // ExcFileNotFound() may be called if directions for
                // deriving base classes are followed.  see r12.6.2
};

class ExcTimeout : virtual public BaseException
{
    public:
        ExcTimeout(const string& msg) : BaseException(msg) {}
        ExcTimeout() : BaseException(nullstring) {}
                // BaseException(nullstring) should never be called even though
                // ExcFileNotFound() may be called if directions for
                // deriving base classes are followed.  see r12.6.2
};

class ExcTooBig : virtual public BaseException
{
    public:
        ExcTooBig(const string& msg) : BaseException(msg) {}
        ExcTooBig() : BaseException(nullstring) {}
                // BaseException(nullstring) should never be called even though
                // ExcFileNotFound() may be called if directions for
                // deriving base classes are followed.  see r12.6.2
};

class ExcCommError : virtual public BaseException
{
    public:
        ExcCommError(const string& msg) : BaseException(msg) {}
        ExcCommError() : BaseException(nullstring) {}
                // BaseException(nullstring) should never be called even though
                // ExcFileNotFound() may be called if directions for
                // deriving base classes are followed.  see r12.6.2
};

class ExcPassword : virtual public BaseException
{
    public:
        ExcPassword(const string& msg) : BaseException(msg) {}
        ExcPassword() : BaseException(nullstring) {}
                // BaseException(nullstring) should never be called even though
                // ExcFileNotFound() may be called if directions for
                // deriving base classes are followed.  see r12.6.2

};

class ExcDebug : virtual public BaseException
{
    public:
        ExcDebug(const string& msg) : BaseException(msg) {}
        ExcDebug() : BaseException(nullstring) {}
                // BaseException(nullstring) should never be called even though
                // ExcFileNotFound() may be called if directions for
                // deriving base classes are followed.  see r12.6.2
};

class ExcDebugMsg : public ExcDebug, public virtual BaseException
{
    public:
        ExcDebugMsg(char* file, int line) : BaseException(helpdescribe(
            "This code at line %d in source file %s should not have been executed."
              ,line ,file)) {}
};

class ExcResources : virtual public BaseException
{
    public:
        ExcResources(const string& msg) : BaseException(msg) {}
        ExcResources() : BaseException(nullstring) {}
                // BaseException(nullstring) should never be called even though
                // ExcFileNotFound() may be called if directions for
                // deriving base classes are followed.  see r12.6.2
};

class ExcBadParameter : virtual public BaseException
{
    public:
        ExcBadParameter(const string& msg) : BaseException(msg) {}
        ExcBadParameter() : BaseException(nullstring) {}
                // BaseException(nullstring) should never be called even though
                // ExcFileNotFound() may be called if directions for
                // deriving base classes are followed.  see r12.6.2
};

class ExcCalcError : virtual public BaseException
{
    public:
        ExcCalcError(const string& msg) : BaseException(msg) {}
        ExcCalcError() : BaseException(nullstring) {}
                // BaseException(nullstring) should never be called even though
                // ExcFileNotFound() may be called if directions for
                // deriving base classes are followed.  see r12.6.2
};

class ExcWrongHardware : virtual public BaseException
{
    public:
        ExcWrongHardware(const string& msg) : BaseException(msg) {}
        ExcWrongHardware() : BaseException(nullstring) {}
};

class ExcDead : virtual public BaseException
{
    public:
        ExcDead(const string& msg) : BaseException(msg) {}
        ExcDead() : BaseException(nullstring) {}
};

#endif //__EXCEPT_HPP
