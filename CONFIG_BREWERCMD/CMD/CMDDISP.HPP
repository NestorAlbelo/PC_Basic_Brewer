/*
 * cmddisp.hpp
 *
 * This file contains the routines used to display and record information
 *  for the user.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#ifndef __CMDDISP_HPP
#define __CMDDISP_HPP

#include <conio.h>
#include <constrea.h>
#include <fstream.h>
#include <time.h>
#include ".\device\device.hpp"
#include ".\cmd\cmdini.hpp"

#define NO_CONFIG -999

class Display
{
//
// Variables
//
protected:
    constream   headerWin,          // Windows used for displaying information
                dataWin,            //  on the screen for the user
                statusWin;
    char        userText[250];      // Text typed by the user
    char        logFileName[250];   // Log file name
    time_t      lastClockUpdate;    // The last time the screen clock was
                                    //  updated

//
// Methods
//
public:
    Display(BrewerCMDINI* i);
            // Set up display
    ~Display();
            // Clean up
    void bossMessage(const char* msg);
    void brewerMessage(const char* msg);
            // Display a message and log it
    void write(const char* msg);
    void write(const int data);
    void write(const long data);
            // Display any message
    char* getUserInput(void);
            // This routine handles user input.
    void updateStatusWindow(DeviceStatus st);
            // Updates the status window with the new status st
    void updateSerialNumber(int sn = NO_CONFIG);
            // Updates the serial number display with the serial number (-1 if unknown)
    void updateTimeDisplay(void);
            // Updates the time display onscreen
    void wait(double sec);
            // Wait for a specified time period in seconds
};

#endif /* __CMDDISP_HPP */

