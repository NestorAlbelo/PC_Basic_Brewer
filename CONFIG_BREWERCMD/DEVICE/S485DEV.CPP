/*
 * s485dev.cpp
 *
 * This is the implementation for the SCITEC485Device class.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#include ".\device\s485dev.hpp"

#ifndef _NO_VERSION_INFO
#   include ".\version\version.hpp"

    static Version ver("SCITEC485Device", 2, 3);
    // See version.hpp
#endif

void SCITEC485Device::formatLowLevelCommandString(const char* cmd)
{
int     loop = -1;
int     loop2 = -1;

    do
    {
        loop++;
        loop2++;
        // Replace all ^X with control characters; replace ^^ with ^
        if(cmd[loop] == '^')
            if(cmd[++loop] == '^')
                inst.cmd[loop2] = '^';
            else
                inst.cmd[loop2] = (cmd[loop] & 31);
        else
            inst.cmd[loop2] = cmd[loop];
    } while(cmd[loop] != '\0');
}

// We don't want the ETX character in the response, so we must strip it
//  off if it is present
void SCITEC485Device::stripFormattingChars(void)
{
    char* chr = strchr(inst.reply, 3);    // \x03 = ETX
    if(chr)
        *chr = '\0';
}

// SCITEC485Devices can use the full range of status types, so simply read
//  the reply to get the current status
DeviceStatus SCITEC485Device::returnResponse(void)
{
    switch(inst.reply[0])
    {
    case '\x02':    // STX
    case '\x10':    // DLE (because it has a reply)
    case '\x16':    // SYN (because it has a reply, too)
    case '\x1D':    // GS  (because it has a reply, too)
        return R_MSG;
    case '\x06':    // ACK
        return R_ACK;
    case '\x0C':    // FF
        return R_OK;
    case '\x15':    // NAK
        return R_NAK;
    }
    return R_ERR;
}

// Here, we want to wait until the device is done whatever it is doing,
//  or insure that it is, in fact, available.  Note that this will not
//  wipe out any message currently waiting to be read.
void SCITEC485Device::waitUntilReady(void)
{
DeviceStatus    curStatus;

    // Reset attempts to zero here
    attempts = 0;
    while((curStatus = status()) != R_MSG && curStatus != R_OK)
    {
        pause();
        if(attempts++ >= maxAttempts)
            throw DeviceNotReady(maxAttempts);
    }
    // Reset attempts to zero here in case transmitToInstrument() needs it
    attempts = 0;
}

// status() will return the current system status, or prompt the system
//  for an updated status.  Note that if a message is waiting to be
//  read (ie. gotReply == 0 and curStatus == R_MSG), we deliberately
//  avoid prompting the system for an update.  This is to avoid losing
//  the reply that the user hasn't read yet.  gotReply must be set (by
//  getReply()) before a new status check can be done.
DeviceStatus SCITEC485Device::status(void)
{
DeviceStatus    curStatus;

    curStatus = returnResponse();
    if(gotReply == 1 || curStatus == R_ACK || curStatus == R_NAK || curStatus == R_ERR)
    {
//        if(attempts++ >= maxAttempts)
//            throw DeviceNotReady(maxAttempts);
        gotReply = 0;
        pause();
        setTimeOut();
        strcpy(inst.cmd, "\x05");       // ENQ
        return transmitToInstrument();
    }
    return curStatus;
}

DeviceNotReady::DeviceNotReady(int retries)
    : BaseException(helpdescribe("Device not ready after %i attempts", retries))
{
}

