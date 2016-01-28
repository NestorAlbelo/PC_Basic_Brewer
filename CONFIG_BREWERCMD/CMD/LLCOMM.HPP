/*
 * llcomm.hpp
 *
 * This file contains low-level routines used to communicate with the Brewer
 *  instrument.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#ifndef __LLCOMM_HPP
#define __LLCOMM_HPP

#include ".\device\device.hpp"
#include ".\port\bascport.hpp"
#include ".\port\s485port.hpp"
#include ".\cmd\cmddisp.hpp"
#include ".\cmd\cmdini.hpp"

// Typedefs
typedef enum { loadMode = 0, cosmacMode, operatingMode, } ModeType;

class LowLevelBrewer
{
//
// Variables
//
protected:
    Display*                display;    // These are the actual port drivers
    BrewerAscPort*          ap;         //  for the Brewer
    SCITEC485Port*          op;
    Device*                 brewer[3];  // These are the devices used to
                                        //  communicate with the Brewer
    QueuedPort::Tracing     tracing;    // Desired trace mode
    SerialPort::BaudRate    baudToTry;  // First baud rate to be tried
    int                     retries;    // Number of retries before timeout
    int                     oldBrewer;  // Flag indicating if this is an old
                                        //  Brewer or not
    ModeType                curMode;    // Currently active mode
    SerialPort::CommPort    port;       // The low-level serial port itself
    int                     serialnum;  // The Brewer serial number (if known; -1 if not)

//
// Methods
//
protected:
	void create(SerialPort::BaudRate baud, ModeType mode);
			// Allocate ports and device drivers
	void destroy(void);
            // Deallocate ports and device drivers
    int isWorking(SerialPort::BaudRate baud, ModeType mode, int show = 1);
            // Attempt to establish communications at (baud, mode)
    DeviceStatus testSwitch(void);
            // Verify that we are in Cosmac OR operating mode
    void finishSwitch(ModeType mode, SerialPort::BaudRate baud);
            // Portion of mode switching routine common to all switches
    void badSwitch(ModeType toMode);
            // Bad switch to toMode error message
    void testDevice(ModeType i);
            // Test to see if the specified device is present
    int isCommunicating(ModeType mode);
            // Test if the desired mode is the active one

public:
    LowLevelBrewer(BrewerCMDINI* i, Display* d);
            // Constructor:  creates a Brewer device with all relevant
            //  communication information provided.
    ~LowLevelBrewer() { destroy(); }
            // Destructor
    void reset(void);
            // Attempt to reset communications
    ModeType mode(void) { return curMode; }
            // Tell user what mode we're in
    char* modeName(ModeType m);
            // Get the name of the desired mode
    void setBaudRate(SerialPort::BaudRate b);
            // Set the port to a new baud rate
    void switchTo(ModeType m, SerialPort::BaudRate b = SerialPort::baud9600);
            // These routines allow the user to switch modes
    int loadTopCode(const char* file);
            // Upload new top-level code to Brewer from file
    void save(void);
            // Save the configuration to flash
    int checkSYNStatus(void);
            // Checks the SYN status and returns the ROM code
    void warmReset(void);
            // Perform a warm reset of the Brewer
    void command(const char* cmd);
    char* commandWithReply(const char* cmd);
            // Send a command to the Brewer and wait until they are done
    void lowLevelCommand(const char* cmd);
            // Send a low-level command from Operating mode (only)
    char* lowLevelCommandWithReply(const char* cmd);
            // Send a command to the Brewer and wait until they are done
    void status(void);
            // Update status on screen
    DeviceStatus testSerialNumber(void);
            // Update serial number on screen
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
        : BaseException(helpdescribe("Device %i not allocated - fatal error", i)) {}
};

class NoCommunications : public ExcCommError, public virtual BaseException
{
public:
    NoCommunications(const int port)
        : BaseException(helpdescribe("Unable to establish communications on COM%i", port)) {}
};

class MissingCMDObject : public ExcBadParameter, public virtual BaseException
{
public:
    MissingCMDObject(const char* s)
        : BaseException(helpdescribe("Missing %s object", s)) {}
};

#endif /* __CMDCOMM_HPP */
