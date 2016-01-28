/*
 * cmddebug.cpp
 *
 * This file contains low-level debugging routines for communication with
 *  the Brewer instrument.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#include <dos.h>
#include <strstrea.h>
#include ".\cmd\cmddebug.hpp"

// Various load mode command strings
static char* switchTo[] = {
    "\x0F\x4C", "\x0F\x43", "\x0F\x4F",     // ^OL ^OC ^OO
};

const char SetBaud = '\x1C';              // ^\
static char* SetTo1200Baud = "\x1C\x32";    // ^\2
//static char* SetTo9600Baud = "\x1C\x35";    // ^\5

// Other strings
static char *TraceModes[] = {
    "off",      "text",     "full",         "",
};

Brewer::Brewer(BrewerCMDINI* i, Display* d) : display(d)
{
char*                   temp;
SerialPort::BaudRate    baudToTry = SerialPort::baud1200;

    oldBrewer = 0;
    if(i == NULL)
        throw MissingCMDObject("INI");
    temp = i->getValue("Tracing");
    if(temp == NULL)
        tracing = QueuedPort::off;
    else
        for(int loop = 0; TraceModes[loop]; loop++)
            if(strcmp(temp, TraceModes[loop]) == 0)
                tracing = QueuedPort::Tracing(loop);
    temp = i->getValue("COM Port");
    if(temp == NULL)
        port = SerialPort::com1;
    else
        port = (atoi(temp) == 1) ? SerialPort::com1 : SerialPort::com2;
//    if(strcmp((temp = i->getValue("Force 1200 Cosmac")), "yes") == 0)

    temp = i->getValue("Baud Rate");
    if(temp != NULL)
        baudToTry = atol(temp);
    // Caution: always try cosmacMode baud rates first.  If you don't,
    //  you'll screw up old Brewers
    if(display == NULL)
        throw MissingCMDObject("Display");
    display->bossMessage("Attempting to establish communications");
    // If baud rate > 1200, assume that operating/load mode is being used
    if(baudToTry.getValue() < 2400 || !isWorking(baudToTry, operatingMode))
        // Try Cosmac mode baud rates next
        if(!isWorking(SerialPort::baud1200, cosmacMode))
            if(!isWorking(SerialPort::baud300, cosmacMode))
            {
                // We must not be in Cosmac mode; try all baud rates in opmode
                for(baudToTry = SerialPort::baud9600;
                    baudToTry.getValue() > 300 &&
                        !isWorking(baudToTry, operatingMode);)
                    baudToTry = SerialPort::BaudRateEnum(int(baudToTry) - 1);
                if(baudToTry.getValue() == 300)
                    if(!isWorking(baudToTry, operatingMode))
                        throw NoCommunications();
            }
    // Test to see if load mode or on a old Brewer
    if(testSwitch() == R_NAK)
    {
        if(mode == operatingMode)
            mode = loadMode;
        else
        {
            oldBrewer = 1;
            if(ap == NULL)
                throw MissingCMDObject("AsciiPort");
            if(ap->getBaud().getValue() == 300)
            {
                display->bossMessage("Initializing old Brewer to 1200 baud");
                command("F,0,2;V,120,1");
                destroy();      // To remove currently working 300 baud port
                if(!isWorking(SerialPort::baud1200, cosmacMode))
                    throw NoCommunications();
                display->bossMessage("Writing firmware bugfixes");
                command("L,16905,90,18041,74,16953,110,18057,64,16977,90");
                command("L,20372,137,20373,226,20374,115,20375,248,20376,22");
                command("L,20377,169,20378,52,20379,162,20380,41,20381,137");
                command("L,20382,58,20383,154,20384,18,20385,213,20386,18");
                command("L,20387,2,20388,169,20389,233,20390,192,20391,77");
                command("L,20392,101,19810,192,19811,79,19812,148");
                command("L,19413,0,19414,255");
            }
        }
    }
    showSetup();
    display->updateStatusWindow(R_OK);
}

void Brewer::destroy(void)
{
    if(ap != NULL)
        delete ap;
    ap = NULL;
    if(op != NULL)
        delete op;
    op = NULL;
    for(ModeType i = loadMode; i <= operatingMode; i++)
    {
        if(brewer[i] != NULL)
            delete brewer[i];
        brewer[i] = NULL;
    }
}

int Brewer::isWorking(SerialPort::BaudRate b, ModeType m)
{
//char        temp[200];
//ostrstream  s(temp, 200);

    mode = m;
//    s << "Attempting to communicate at " << b.getValue() << " baud ("
//      << modeStr[mode] << " mode)" << ends;
//    display->bossMessage(temp);
    ap = new BrewerAscPort(port, b);
    op = new BrewerOpPort(port, b);
    ap->trace(tracing);
    op->trace(tracing);
    brewer[loadMode] = new LoadDevice(*op);
    brewer[cosmacMode] = new AscDevice(*ap);
    brewer[operatingMode] = new OpDevice(*op);
    if(!isCommunicating(mode))
    {
        destroy();
        return 0;
    }
    return 1;
}

void Brewer::setBaudRate(SerialPort::BaudRate newBaud)
{
    if(ap == NULL)
        throw MissingCMDObject("AsciiPort");
    ap->resetPort(newBaud);
    if(op == NULL)
        throw MissingCMDObject("OpPort");
    op->resetPort(newBaud);
}

void Brewer::status(void)
{
    testDevice(mode);
    display->updateStatusWindow(brewer[mode]->status());
}

int Brewer::isCommunicating(ModeType mode)
{
int status = 1;

    testDevice(mode);
    try { brewer[mode]->waitUntilReady(); }
    catch(QSerialTimeout& err)
    {
        try { brewer[mode]->waitUntilReady(); }
        catch(QSerialTimeout& err)
        {
            status = 0;
        }
    }
    return status;
}

void Brewer::switchToCosmacMode(void)
{
    switch(mode)
    {
    case cosmacMode:
        wrongMode();
        break;
    case operatingMode:
        badSwitch(cosmacMode);
        break;
    default:
        command(SetTo1200Baud);
        command(switchTo[cosmacMode]);
        mode = cosmacMode;
        setBaudRate(SerialPort::baud1200);
        finishSwitch();
    }
}

void Brewer::switchToLoadMode(void)
{
    switch(mode)
    {
    case loadMode:
        wrongMode();
        break;
    case cosmacMode:
        if(isAllowed())
        {
            command("\x04\x04\x04\x04\x04\x04\x04");
            mode = loadMode;
            finishSwitch();
        }
        break;
    default:
        mode = loadMode;
        command(switchTo[loadMode]);    // Command must be sent to load mode level
        finishSwitch();
        break;
    }
}

void Brewer::switchToOperatingMode(SerialPort::BaudRate b)
{
    switch(mode)
    {
    case operatingMode:
        wrongMode();
        break;
    case cosmacMode:
        badSwitch(operatingMode);
        break;
    default:
        char temp[4];
        temp[0] = SetBaud;
        temp[1] = '0' + (SerialPort::BaudRateEnum(b) - SerialPort::baud300);
        temp[2] = '\0';
        command(temp);
        command(switchTo[operatingMode]);
        mode = operatingMode;
        setBaudRate(b);
        finishSwitch();
        break;
    }
}

DeviceStatus Brewer::testSwitch(void)
{
    testDevice(mode);
    // ?BREWER.ID fails in load mode and on old Brewers
    if(brewer[mode]->sendCommand("?BREWER.ID", 5) == R_NAK)
        return R_NAK;
    else if(atoi(brewer[mode]->getReply()) == 0)
        return R_NAK;
    return R_OK;
}

void Brewer::finishSwitch(void)
{
    display->updateStatusWindow(R_ACK);
    int loop = 0;
    display->write("BOSS> Waiting");
    while(!isCommunicating(mode) && loop++ < 8)
    {
        sleep(6U);
        display->write(".");
    }
    display->write("\n");
    // If we are entering operating/Cosmac mode, verify that it worked.
    //  Switch back if it did not work.  If we are in load mode, this test
    //  does nothing useful
    if(testSwitch() == R_NAK)
        mode = loadMode;
    showSetup();
    display->updateStatusWindow(R_ACK);
}

int Brewer::checkSYNStatus(void)
{
char*   reply;

    switch(mode)
    {
    case cosmacMode:
        wrongMode();
        return 0;
    default:
        reply = commandWithReply("\x16");
        break;
    }
    // (^VL noopp) SYN: status returned (oo is flash status [4-5])
    return (reply[4] - '0') * (reply[3] == '-' ? -1 : 1);
}

void Brewer::command(const char* cmd)
{
DeviceStatus    st;

    testDevice(mode);
    st = brewer[mode]->sendCommand(cmd, 20);
    display->updateStatusWindow(st);
    if(st == R_ACK)
        display->updateStatusWindow(brewer[mode]->waitUntilReady());
}

char* Brewer::commandWithReply(const char* cmd)
{
    testDevice(mode);
    display->updateStatusWindow(brewer[mode]->sendCommand(cmd, 20));
    return brewer[mode]->getReply();
}

void Brewer::showSetup(void)
{
char        temp[80];
ostrstream  s(temp, 80);

    testDevice(mode);
    s << "Using " << modeStr[mode] << " mode at ";
    if(mode == cosmacMode)
        s << (ap->getBaud()).getValue();
    else
        s << (op->getBaud()).getValue();
    s << " baud, on port " << (port == SerialPort::com1 ? '1' : '2')
      << ": tracing is " << TraceModes[tracing] << ends;
    display->bossMessage(temp);
}

void Brewer::badSwitch(ModeType toMode)
{
char        temp[80];
ostrstream  s(temp, 80);

    s << "Cannot switch to " << modeStr[toMode] << " from " << modeStr[mode]
      << " mode" << ends;
    display->bossMessage(temp);
}

void Brewer::testDevice(ModeType i)
{
    if(brewer[i] == NULL)
        throw MissingDeviceFatal(i);
    if(ap == NULL && i == cosmacMode)
        throw MissingDeviceFatal(i);
    if(op == NULL && i != cosmacMode)
        throw MissingDeviceFatal(i);
}

int Brewer::isAllowed(void)
{
    if(oldBrewer)
        display->bossMessage("Command not allowed on old Brewers");
    return !oldBrewer;
}

void Brewer::wrongMode(void)
{
char        temp[80];
ostrstream  s(temp, 80);

    s << "Command not allowed in " << modeStr[mode] << " mode" << ends;
    display->bossMessage(temp);
}
