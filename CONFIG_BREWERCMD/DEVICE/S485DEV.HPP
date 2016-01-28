/*
 * s485dev.hpp
 *
 * This is the header file for the SCI-TEC 485 device class.
 *
 * Copyright(C) 1996, SCI-TEC Instruments Inc.
 */

#ifndef __S485DEV_HPP
#define __S485DEV_HPP

#include ".\device\device.hpp"
#include ".\port\s485port.hpp"

class SCITEC485Device : virtual public Device
{
//
// Variables
//
protected:
    int                     gotReply;   // Received reply status flag
    int                     attempts;   // Current number of attempts
    int                     maxAttempts;// Number of allowed attempts before
                                        //  a DeviceNotReady exception is
                                        //  raised
    SCITEC485MesgExchange   inst;

//
// Methods
//
protected:
    void setTimeOut(void) { setTimeOut(2); }
    void setTimeOut(int timeOut) { inst.timeOut = (timeOut > 2 ? timeOut : 2); }
    void formatCommandString(const char* cmd) {
        strcpy(inst.cmd, "\x02");
        strcat(inst.cmd, cmd);
        strcat(inst.cmd, "\x03");
    }
    void formatLowLevelCommandString(const char* cmd);
    DeviceStatus transmitToInstrument(void)
            { gotReply = 0; inst.exchange(); return returnResponse(); }
    DeviceStatus returnResponse(void);
    void stripFormattingChars(void);

public:
    SCITEC485Device(SCITEC485Port& p, char* addr, int retries = 200)
        : Device(p), gotReply(1), attempts(0), maxAttempts(retries), inst(&p, addr) {}
            // Constructor:  creates a device with all relevant communication
            //  information provided.  Note that an instrument address is
            //  necessary
    ~SCITEC485Device() {}
            // Destructor
    const char* changeAddress(const char* addr) {
        inst.changeAddress(addr);
        return addr;
    }
    char* getReply(void) {
        waitUntilReady();
        if(status() == R_OK)
        {
            gotReply = 1;
            return NULL;
        }
        gotReply = 1;
        stripFormattingChars();
        // Skip the response code character; it's not part of the return text
        return &inst.reply[1];
    }
    void waitUntilReady(void);
    DeviceStatus status(void);
};

class DeviceNotReady : public ExcTimeout, public ExcCommError, public virtual BaseException
{
public:
    DeviceNotReady(int retries);
};

#endif  // __S485DEV_HPP

