/*
 * cmdini.hpp
 *
 * This file contains the routines used to read and write the .INI file.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#ifndef __CMDINI_HPP
#define __CMDINI_HPP

#include <fstream.h>

class BrewerCMDINI
{
//
// Variables
//
protected:
    char*       names[10];
    char*       values[10];
//    int         changed;    // Has the configuration changed?
    int         numEntries; // Number of entries read from file
    int         maxEntries; // Maximum number of possible entries

//
// Methods
//
protected:
    int readINI(void);
//    int writeINI(void);
    void createINI(void);

public:
    BrewerCMDINI(int max);
            // Set up display
    ~BrewerCMDINI();
            // Clean up
    char* getValue(const char* index);
            // Look up value from table
//    void setValue(const char* index, const char* newvalue);
            // Set a new value
};

#endif /* __CMDINI_HPP */

