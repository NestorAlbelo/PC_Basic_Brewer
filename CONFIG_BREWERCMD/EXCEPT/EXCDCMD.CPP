/*
    ExcDCmd.cpp     (Exceptions package, display function, Command-line version)

    This file implements BaseException::display() for those applications
    that work on a line by line basis and wish to display errors on cerr.   */


#include <iostream.h>
#include ".\except\except.hpp"

void BaseException::display()
{
    cerr << "Error! " << why() << endl;
}


