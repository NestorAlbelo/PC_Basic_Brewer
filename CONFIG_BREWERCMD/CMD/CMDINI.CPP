/*
 * cmdini.cpp
 *
 * This file contains the routines used to read and write the .INI file.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#include <ctype.h>
#include <fstream.h>
#include <string.h>
#include ".\cmd\cmdini.hpp"

// .INI filename
char    iniFileName[20] = "brewcmd.ini";

BrewerCMDINI::BrewerCMDINI(int max) : numEntries(0), maxEntries(max)
{
    if(!readINI())
    {
        createINI();
        readINI();
    }
}

BrewerCMDINI::~BrewerCMDINI(void)
{
    for(int loop = 0; loop < numEntries; loop++)
    {
        delete[] names[loop];
        delete[] values[loop];
    }
}

void BrewerCMDINI::createINI(void)
{
    ofstream outFile(iniFileName);
    if(outFile.good())
    {
        outFile << "; BrewerCMD Initialization File" << endl
                << ";" << endl
                << "; Copyright(C) 1995, SCI-TEC Instruments Inc." << endl
                << "; All rights reserved." << endl
                << endl
                << "COM Port=1" << endl
                << "Baud Rate=1200" << endl
                << "Tracing=off" << endl
                << "Log File=brewcmd.log" << endl
                << "Retries=300" << endl;
    }
    outFile.close();
}

int BrewerCMDINI::readINI(void)
{
char        temp[80];
ifstream    ifile(iniFileName);

    if(!ifile.good())
        return 0;
    while(ifile.good())
    {
        do { temp[0] = ifile.get(); } while(isspace(temp[0]));
        if(temp[0] == ';')
            ifile.getline(temp, 80, '\n');
        else
        {
            // Save the first character, which we read already
            ifile.getline(&temp[1], 79, '=');
            if(numEntries < maxEntries)
            {
                names[numEntries] = new char[strlen(temp) + 2];
                strcpy(names[numEntries], temp);
            }
            ifile.getline(temp, 80, '\n');
            if(numEntries < maxEntries)
            {
                values[numEntries] = new char[strlen(temp) + 2];
                strcpy(values[numEntries++], temp);
            }
        }
    }
    ifile.close();
    return 1;
}

char* BrewerCMDINI::getValue(const char* index)
{
    for(int loop = 0; loop < numEntries; loop++)
        if(strcmp(names[loop], index) == 0)
            return values[loop];
    return NULL;
}

