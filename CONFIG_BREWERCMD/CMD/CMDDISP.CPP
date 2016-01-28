/*
 * cmddisp.cpp
 *
 * This file contains the routines used to display and record information
 *  for the user.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#include <ctype.h>
#include <dos.h>
#include <strstrea.h>
#include <time.h>
#include ".\cmd\cmddisp.hpp"

static ofstream *outFile = NULL;

// Program header
static char* progHeader[] = {
	"BrewCMD Communication Utility (V1R3)",
	"Copyright(C) November 1998, SCI-TEC Instruments Inc.  All rights reserved.",
	"Type HELP at any time for help                        Type QUIT to quit",
	"",
};

static char *statusStr[] = {
    "Error       ",
    "Not accepted",
    "Ready       ",
    "Ready       ",
    "Busy        ",
    "",
};

Display::Display(BrewerCMDINI* i)
{
char*   temp;

	lastClockUpdate = time(NULL);
	clrscr();
    temp = i->getValue("Log File");
    if(temp == NULL)
        strcpy(logFileName, "brewcmd.log");
    else
        strcpy(logFileName, temp);
	outFile = new ofstream(logFileName);
    *outFile << "; BrewerCMD Output Log File" << endl
             << ";" << endl
             << "; " << progHeader[1] << endl << endl;
    delete outFile;
    outFile = NULL;
    headerWin.window(1, 1, 80, 3);
    dataWin.window(1, 5, 80, 22);
    statusWin.window(1, 24, 80, 25);
    for(int loop = 0; *progHeader[loop]; loop++)
        headerWin << endl << progHeader[loop];
    bossMessage("Communication settings read from BREWCMD.INI");
}

Display::~Display(void)
{
    if(!outFile)
    	outFile = new ofstream(logFileName, ios::app);
    *outFile << endl << "; Log file closed" << endl;
    delete outFile;
    outFile = NULL;
}

void Display::bossMessage(const char* msg)
{
	dataWin << "HOST> " << msg << endl;
    if(!outFile)
    	outFile = new ofstream(logFileName, ios::app);
	*outFile << "HOST> " << msg << endl;
}

void Display::brewerMessage(const char* msg)
{
	dataWin << "ANY > " << msg << endl;
    if(!outFile)
    	outFile = new ofstream(logFileName, ios::app);
	*outFile << "ANY > " << msg << endl;
}

void Display::write(const char* msg)
{
	dataWin << msg;
    if(!outFile)
    	outFile = new ofstream(logFileName, ios::app);
    *outFile << msg;
}

void Display::write(const int data)
{
	dataWin << data;
    if(!outFile)
    	outFile = new ofstream(logFileName, ios::app);
    *outFile << data;
}

void Display::write(const long data)
{
	dataWin << data;
    if(!outFile)
    	outFile = new ofstream(logFileName, ios::app);
    *outFile << data;
}

char* Display::getUserInput(void)
{
    dataWin << "USER>";
    while(!kbhit())
        updateTimeDisplay();
    dataWin << " ";
    cin.getline(userText, 250);
    dataWin << endl;
    for(int loop = 0; userText[loop]; loop++)
        userText[loop] = toupper(userText[loop]);
    if(!outFile)
    	outFile = new ofstream(logFileName, ios::app);
    *outFile << "USER> " << userText << endl;
    return userText;
}

void Display::updateStatusWindow(DeviceStatus st)
{
    statusWin.rdbuf()->gotoxy(1, 1);
	statusWin << "Status: " << statusStr[st - R_ERR];
    updateTimeDisplay();
}

void Display::updateSerialNumber(int sn)
{
    statusWin.rdbuf()->gotoxy(36, 1);
    if (sn >= 0)
        statusWin << '#' << setw(3) << setfill('0') << sn;
    else if (sn == NO_CONFIG)
        statusWin << "!CFG";
    else
        statusWin << "    ";
    updateTimeDisplay();
}

void Display::updateTimeDisplay(void)
{
time_t      t_;
struct tm*  t;
char        temp[35];

	t_ = time(NULL);
	if(difftime(t_, lastClockUpdate) > 0.5)
    {
        lastClockUpdate = t_;
        t = localtime(&t_);
        strftime(temp, 35, "%a %b %d (%j) %H:%M:%S %Y", t);
        statusWin.rdbuf()->gotoxy(44, 1);
        statusWin << temp;
    }
    // Close the output log file, now that we're idle
    if(outFile)
        delete outFile;
    outFile = NULL;
}

void Display::wait(double sec)
{
time_t  s, t;

	s = time(NULL);
    do { t = time(NULL); updateTimeDisplay(); }
        while(difftime(t, s) < sec);
}

