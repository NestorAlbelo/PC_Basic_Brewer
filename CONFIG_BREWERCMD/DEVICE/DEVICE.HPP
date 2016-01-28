/*
 * device.hpp
 *
 * This file defines a generic device class.  This class is intended to
 *  provide a standard set of methods for communication with an instrument.
 *  It hides the low-level communication with the actual device.  A
 *  device-type object is derived from the virtual Device object, which
 *  knows the actual instrument protocol.
 *
 * Examples of usage:
 *
 *  BrewerAscPort p;                  SCITEC485Port q;
 *  AscDevice x(p);                   UVCIDevice y(q);
 *
 *  x.sendCommand("status");          y.sendCommand("test");
 *  cout << x.getReply() << endl;     y.waitUntilReady();
 *
 * Implementing new types of devices is fairly straightforward.  Refer
 *  to the UVCIDevice class for a good example of this.  Also note that
 *  using a MessageExchange instead of a BMessageExchange would make
 *  the Device class more generic, and not specific to Brewer devices.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#ifndef __DEVICE_HPP
#define __DEVICE_HPP

#include <dos.h>
#include ".\qserial\qserial.hpp"

// Typedefs
typedef enum {
    R_ERR = -2,  R_NAK = -1,  R_OK = 0,  R_MSG = 1,  R_ACK = 2,
} DeviceStatus;

class Device
{
//
// Variables
//
protected:
    QueuedPort*         port;

//
// Methods
//
protected:
    void pause(void) { delay(20U); }
            // Short time delay to prevent sending signals too quickly
    virtual void setTimeOut(void) = 0;
    virtual void setTimeOut(int timeOut) = 0;
            // Sets a timeout for the current message about to be sent
            //  (should be a minimum of two seconds to avoid timing
            //  problems)
    virtual void formatCommandString(const char* cmd) = 0;
            // Properly format a command string for sending to the device
    virtual void formatLowLevelCommandString(const char* cmd) = 0;
            // Properly format a command string for low-level transmission
    virtual DeviceStatus transmitToInstrument(void) = 0;
            // Actually transmit the message to the device

public:
    Device(QueuedPort& p) : port(&p) {}
            // Constructor:  creates a device with all relevant communication
            //  information provided.
    virtual ~Device() {}
            // Destructor
    virtual const char* changeAddress(const char* addr) { return addr; }
            // Change the device address (unless it's not addressable)
    DeviceStatus sendCommand(const char* cmd) {
            waitUntilReady();
            setTimeOut();
            formatCommandString(cmd);
            return transmitToInstrument();
    }
    DeviceStatus sendCommand(const char* cmd, int timeOut) {
            waitUntilReady();
            setTimeOut(timeOut);
            formatCommandString(cmd);
            return transmitToInstrument();
    }
            // Send a command to the device
    DeviceStatus sendLowLevelCommand(const char* cmd) {
            waitUntilReady();
            setTimeOut();
            formatLowLevelCommandString(cmd);
            return transmitToInstrument();
    }
    DeviceStatus sendLowLevelCommand(const char* cmd, int timeOut) {
            waitUntilReady();
            setTimeOut(timeOut);
            formatLowLevelCommandString(cmd);
            return transmitToInstrument();
    }
            // Send a low-level command to the device
    virtual char* getReply(void) = 0;
            // Wait for a reply and return a pointer to it
    virtual void waitUntilReady(void) = 0;
            // Wait until the device is ready (ie. no reply expected)
    virtual DeviceStatus status(void) = 0;
            // Query the current device status
};

#endif  // __DEVICE_HPP

