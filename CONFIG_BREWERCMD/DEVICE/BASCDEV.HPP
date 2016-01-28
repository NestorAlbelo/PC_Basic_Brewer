/*
 * bascdev.hpp
 *
 * This is the header file for the Brewer ASCII-based device class.
 *  It defines a generic old-style Brewer, which uses a simple
 *  ASCII interface for communication.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#ifndef __BASCDEV_HPP
#define __BASCDEV_HPP

#include <dos.h>
#include ".\port\bascport.hpp"
#include ".\device\device.hpp"

const int maxBCmd = 230;    // Maximum command length
const int maxBReply = 230;  // Maximum reply length

class BrewerAscDevice : virtual public Device
{
//
// Variables
//
protected:
    MessageExchange     brewer;

//
// Methods
//
protected:
    void setTimeOut(void) {}
	void setTimeOut(int timeOut) {}
    void formatCommandString(const char* cmd) { strcpy(brewer.cmd, cmd); }
    void formatLowLevelCommandString(const char* cmd) { strcpy(brewer.cmd, cmd); }
    DeviceStatus transmitToInstrument(void) { pause(); brewer.send(); return R_OK; }
    void waitUntilDone(void);

public:
    BrewerAscDevice(BrewerAscPort& p)
        : Device(p), brewer(&p, new char[maxBCmd+3], new char[maxBReply+3]) {}
            // Constructor:  creates a Brewer device with all relevant
            //  communication information provided.
    ~BrewerAscDevice() { delete brewer.cmd; delete brewer.reply; }
            // Destructor
    char* getReply(void) { waitUntilDone(); return brewer.reply; }
    // WARNING: Do NOT use waitUntilReady() if you are waiting for a
    //          response from a command; use getReply instead
	void waitUntilReady(void);
    DeviceStatus status(void);
};

#endif  // __BASCDEV_HPP

