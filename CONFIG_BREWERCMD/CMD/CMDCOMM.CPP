/*
 * cmdcomm.cpp
 *
 * This file contains the routines used to communicate with the Brewer
 *  instrument.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#include <strstrea.h>
#include ".\cmd\cmdcomm.hpp"

void Brewer::showException(BaseException& err)
{
    char temp[200];
    ostrstream s(temp, 200);
    s << "An exception occurred: " << err.why() << ends;
    display->bossMessage(temp);
}

void Brewer::reset(void)
{
    try { LowLevelBrewer::destroy(); LowLevelBrewer::reset(); }
    catch(BaseException& err) { showException(err); }
}

void Brewer::status(void)
{
    try { LowLevelBrewer::status(); }
    catch(BaseException& err) { showException(err); }
}

void Brewer::testSerialNumber(void)
{
    // Note that we throw away the result here, since we don't need it
    try { LowLevelBrewer::testSerialNumber(); }
    catch(BaseException& err) { showException(err); }
}

void Brewer::switchTo(ModeType m, SerialPort::BaudRate b)
{
    try { LowLevelBrewer::switchTo(m, b); }
    catch(BaseException& err) { showException(err); }
}

int Brewer::loadTopCode(const char* file)
{
    try { return LowLevelBrewer::loadTopCode(file); }
    catch(BaseException& err) { showException(err); return 0; }
}

void Brewer::warmReset(void)
{
    try { LowLevelBrewer::warmReset(); }
    catch(BaseException& err) { showException(err); }
}

void Brewer::save(void)
{
    try { LowLevelBrewer::save(); }
    catch(BaseException& err) { showException(err); }
}

int Brewer::checkSYNStatus(void)
{
    try { return LowLevelBrewer::checkSYNStatus(); }
    catch(BaseException& err) { showException(err); return -1; }
}

void Brewer::command(const char* cmd)
{
    try { LowLevelBrewer::command(cmd); }
    catch(BaseException& err) { showException(err); }
}

char* Brewer::commandWithReply(const char* cmd)
{
    try { return LowLevelBrewer::commandWithReply(cmd); }
    catch(BaseException& err) { showException(err); return NULL; }
}

void Brewer::lowLevelCommand(const char* cmd)
{
    try { LowLevelBrewer::lowLevelCommand(cmd); }
    catch(BaseException& err) { showException(err); }
}

char* Brewer::lowLevelCommandWithReply(const char* cmd)
{
    try { return LowLevelBrewer::lowLevelCommandWithReply(cmd); }
    catch(BaseException& err) { showException(err); return NULL; }
}

