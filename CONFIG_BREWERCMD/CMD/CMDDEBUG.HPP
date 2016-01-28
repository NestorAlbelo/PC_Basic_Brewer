/*
 * cmddebug.hpp
 *
 * This file contains debugging routines used to communicate with the Brewer
 *  instrument.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#ifndef __CMDDEBUG_HPP
#define __CMDDEBUG_HPP

#include ".\motor\device.hpp"
#include ".\cmd\cmddisp.hpp"
#include ".\cmd\cmdini.hpp"

// Typedefs
typedef enum { loadMode = 0, cosmacMode, operatingMode, } ModeType;

// Strings
static char* modeStr[] = {
    "load", "Cosmac", "operating",
};

class DebugBrewer
{
//
// Variables
//
protected:
    Display*            display;
    BrewerAscPort*      ap;
    BrewerOpPort*       op;
            // These are the actual port drivers for the Brewer
    Device*             brewer[3];
            // These are the devices used to communicate with the Brewer
    QueuedPort::Tracing tracing;
            // Desired trace mode
    int                 oldBrewer;
            // Flag indicating if this is an old Brewer

public:
    ModeType                mode;
    SerialPort::CommPort    port;

//
// Methods
//
protected:
    void destroy(void);
            // Deallocate ports and device drivers
    int isWorking(SerialPort::BaudRate baud, ModeType mode);
            // Attempt to establish communications at (baud, mode)
    DeviceStatus testSwitch(void);
            // Verify that we are in Cosmac OR operating mode
    void finishSwitch(void);
            // Portion of mode switching routine common to all switches
    void badSwitch(ModeType toMode);
            // Bad switch to toMode error message
    void testDevice(ModeType i);
            // Test to see if the specified device is present

public:
    Brewer(BrewerCMDINI* i, Display* d);
            // Constructor:  creates a Brewer device with all relevant
            //  communication information provided.
    ~Brewer() { destroy(); }
            // Destructor
    void setBaudRate(SerialPort::BaudRate b);
            // Set the port to a new baud rate
    int isCommunicating(ModeType mode);
            // Test if the desired mode is the active one
    void status(void);
            // Update status on screen
    void switchToCosmacMode(void);
    void switchToLoadMode(void);
    void switchToOperatingMode(SerialPort::BaudRate b = SerialPort::baud4800);
            // These routines allow the user to switch modes
    int checkSYNStatus(void);
            // Checks the SYN status and returns the ROM code
    void command(const char* cmd);
    char* commandWithReply(const char* cmd);
            // Send a command to the Brewer and wait until they are done
    void showSetup(void);
            // Display the current system setup to the user
    int isAllowed(void);
            // Return flag indicating whether old Brewer is present or not
    void wrongMode(void);
            // Wrong mode error message
};

class MissingDeviceFatal : public ExcCommError, public virtual BaseException
{
public:
    MissingDeviceFatal(ModeType i)
        : BaseException(helpdescribe("No %s device allocated - fatal error", modeStr[i])) {}
};

class NoCommunications : public ExcCommError, public virtual BaseException
{
public:
    NoCommunications()
        : BaseException(helpdescribe("Unable to establish communications")) {}
};

class MissingCMDObject : public ExcBadParameter, public virtual BaseException
{
public:
    MissingCMDObject(const char* s)
        : BaseException(helpdescribe("Missing %s object", s)) {}
};

#endif /* __CMDDEBUG_HPP */
