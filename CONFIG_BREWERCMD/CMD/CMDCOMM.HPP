/*
 * cmdcomm.hpp
 *
 * This file contains the routines used to communicate with the Brewer
 *  instrument.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#ifndef __CMDCOMM_HPP
#define __CMDCOMM_HPP

#include ".\cmd\llcomm.hpp"

class Brewer : public LowLevelBrewer
{
//
// Methods
//
protected:
    void showException(BaseException& err);
            // Display exception on screen

public:
    Brewer(BrewerCMDINI* i, Display* d) : LowLevelBrewer(i, d) {}
    ~Brewer() {}
    void reset(void);
            // Redetect communications
    void status(void);
            // Update status on screen
    void testSerialNumber(void);
            // Update serial number on screen
    void switchTo(ModeType m, SerialPort::BaudRate b = SerialPort::baud9600);
            // These routines allow the user to switch modes
    int loadTopCode(const char* file);
            // Upload new top-level code to Brewer from file
    void warmReset(void);
            // Resets the Brewer
    void save(void);
            // Save the configuration to flash
    int checkSYNStatus(void);
            // Checks the SYN status and returns the ROM code
    void command(const char* cmd);
    char* commandWithReply(const char* cmd);
            // Send a command to the Brewer and wait until they are done
    void lowLevelCommand(const char* cmd);
            // Send a low-level command from Operating mode (only)
    char* lowLevelCommandWithReply(const char* cmd);
            // Send a command to the Brewer and wait until they are done
};

#endif /* __CMDCOMM_HPP */

