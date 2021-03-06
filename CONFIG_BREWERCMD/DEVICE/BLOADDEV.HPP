/*
 * bloaddev.hpp
 *
 * This file defines a special "load" device, which is a special
 *  case of the SCITEC485Device class.  It is used exclusively in
 *  Load mode.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#ifndef __BLOADDEV_HPP
#define __BLOADDEV_HPP

#include <dos.h>
#include ".\device\s485dev.hpp"

class LoadDevice : public SCITEC485Device
{
//
// Methods
//
protected:
    void formatCommandString(const char* cmd) { strcpy(inst.cmd, cmd); }
    void stripFormattingChars(void) {}

public:
    LoadDevice(SCITEC485Port& p, char *addr, int retries = 200)
        : Device(p), SCITEC485Device(p, addr, retries) {}
            // Constructor:  creates a Load mode-only device with all
            //  relevant communication information provided.
    ~LoadDevice() {}
            // Destructor
};

#endif  // __BLOADDEV_HPP
