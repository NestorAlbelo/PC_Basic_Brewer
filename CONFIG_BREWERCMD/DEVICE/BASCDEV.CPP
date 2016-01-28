/*
 * bascdev.cpp
 *
 * This is the implementation for the BrewerAscDevice class.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#include ".\device\bascdev.hpp"

#ifndef _NO_VERSION_INFO
#   include ".\version\version.hpp"

    static Version ver("Device", 1, 2);
    // See version.hpp
#endif

// Here, we intend to wait until the device is done whatever it is doing,
//  or insure that it is, in fact, available.  If the status is R_OK
//  (idle), then we should prompt the device to test its current status.
//  Note that this will wipe out any message currently waiting to be
//  read.
void BrewerAscDevice::waitUntilReady(void)
{
    if(status() == R_OK)
    {
        pause();
        strcpy(brewer.cmd, "");
        brewer.send();
    }
    waitUntilDone();
    pause();
}

// This routine could also be called waitUntilNotBusy(), because that is
//  what is does
void BrewerAscDevice::waitUntilDone(void)
{
    while(status() == R_ACK)
        pause();
}

// BrewerAscDevices can't detect system status directly, so we fake it by
//  only using R_OK and R_ACK status types.  (Either the device is busy or
//  it isn't)
DeviceStatus BrewerAscDevice::status(void)
{
    if(brewer.done())
        return R_OK;
    return R_ACK;
}

