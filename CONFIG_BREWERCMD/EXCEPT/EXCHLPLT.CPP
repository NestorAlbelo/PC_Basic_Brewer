/*  ExcHelpLt.cpp

    This file contains the support routines for the Exceptions package
    This is the "Light" version -- ie, it doesn't translate, so doesn't
    have the overhead of languages or .INI files */

#include <stdarg.h>
#include <stdio.h>
#include ".\except\except.hpp"

#ifndef _NO_VERSION_INFO
#   include ".\version\version.hpp"

    static Version ver("Exceptions", 2, 1);
#endif

const string nullstring("");

string BaseException::helpdescribe(const char *keyword, ...)
{
    char outputbuf[200];    // don't want locals in our error handling.
    va_list ap;
    va_start(ap, keyword);
    if(vsprintf(outputbuf, keyword, ap) == EOF)
        strcpy(outputbuf, keyword);
    va_end(ap);
    return string(outputbuf);
}
