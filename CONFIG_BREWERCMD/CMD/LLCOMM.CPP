/*
 * llcomm.cpp
 *
 * This file contains low-level routines used to communicate with the Brewer
 *  instrument.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#include <strstrea.h>
#include <time.h>
#include ".\device\bloaddev.hpp"
#include ".\device\bascdev.hpp"
#include ".\port\s485port.hpp"
#include ".\cmd\llcomm.hpp"

// Strings
static char brewerAddr[] = "ANY";

static char* modeStr[] = {
    "load", "Cosmac", "operating",
};

// Various load mode command strings
static char* SwitchTo[] = {
    "\x0F\x4C", "\x0F\x43", "\x0F\x4F",     // ^OL ^OC ^OO
};

const char SetBaud = '\x1C';              // ^\
//static char* SetTo1200Baud = "\x1C\x32";    // ^\2
//static char* SetTo9600Baud = "\x1C\x35";    // ^\5

// Other strings
static char *TraceModes[] = {
    "off",      "text",     "full",         "",
};

LowLevelBrewer::LowLevelBrewer(BrewerCMDINI* i, Display* d) : display(d)
{
char*  temp;

    serialnum = -1;
    oldBrewer = 0;
    if(i == NULL)
        throw MissingCMDObject("INI");
    temp = i->getValue("Tracing");
	tracing = QueuedPort::off;
	if(temp != NULL)
		for(int loop = 0; loop < 3; loop++)
			if(strcmp(temp, TraceModes[loop]) == 0)
				tracing = QueuedPort::Tracing(loop);
	temp = i->getValue("COM Port");
	if(temp == NULL)
		port = SerialPort::com1;
	else
		port = (atoi(temp) == 1) ? SerialPort::com1 : SerialPort::com2;
	temp = i->getValue("Baud Rate");
	if(temp != NULL)
		baudToTry = atol(temp);
	temp = i->getValue("Retries");
	if(temp != NULL)
		retries = atoi(temp);
	else
		retries = 200;
	// Caution: always try cosmacMode baud rates first.  If you don't,
	//  you'll screw up old Brewers
	if(display == NULL)
		throw MissingCMDObject("Display");
    reset();
}

void LowLevelBrewer::reset(void)
{
	display->write("HOST> Attempting to establish communications on COM");
    display->write(port);
    display->write("... please wait\n");
	// If baud rate > 1200, assume that operating/load mode is being used
    if(baudToTry.getValue() < 2400 || !isWorking(baudToTry, operatingMode))
        // Try Cosmac mode baud rates next
        if(!isWorking(SerialPort::baud1200, cosmacMode))
            if(!isWorking(SerialPort::baud300, cosmacMode))
                if(!isWorking(SerialPort::baud9600, cosmacMode))
                {
                    // We must not be in Cosmac mode; try all baud rates in opmode
		    		for(baudToTry = SerialPort::baud9600;
                        baudToTry.getValue() > 300 &&
				    		!isWorking(baudToTry, operatingMode);)
                        baudToTry = SerialPort::BaudRateEnum(int(baudToTry) - 1);
                    if(baudToTry.getValue() == 300)
                        if(!isWorking(baudToTry, operatingMode))
                            throw NoCommunications((port == SerialPort::com1) ? 1 : 2);
                }
    display->bossMessage("Communications established");
    // Test to see if load mode or on a old Brewer
    if(testSwitch() == R_NAK)
    {
        if(curMode == operatingMode)
            curMode = loadMode;
        else
        {
            display->bossMessage("Detected old Brewer");
			oldBrewer = 1;
            if(ap == NULL)
                throw MissingCMDObject("AsciiPort");
            if(ap->getBaud().getValue() == 300)
			{
                display->bossMessage("Initializing old Brewer to 1200 baud");
                command("F,0,2;V,120,1");
                destroy();      // To remove currently working 300 baud port
                if(!isWorking(SerialPort::baud1200, cosmacMode))
                    throw NoCommunications((port == SerialPort::com1) ? 1 : 2);
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

void LowLevelBrewer::create(SerialPort::BaudRate b, ModeType m)
{
	curMode = m;
	ap = new BrewerAscPort(port, b);
	op = new SCITEC485Port(port, b);
	ap->trace(tracing);
	op->trace(tracing);
	brewer[loadMode] = new LoadDevice(*op, brewerAddr, retries);
	brewer[cosmacMode] = new BrewerAscDevice(*ap);
	brewer[operatingMode] = new SCITEC485Device(*op, brewerAddr, retries);
}

void LowLevelBrewer::destroy(void)
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

int LowLevelBrewer::isWorking(SerialPort::BaudRate b, ModeType m, int show)
{
    if(show)
    {
        display->write("HOST> Trying ");
        display->write(modeName(m));
        display->write(" mode at ");
        display->write(b.getValue());
        display->write(" baud\n");
    }
	create(b, m);
	if(!isCommunicating(curMode))
	{
		destroy();
		return 0;
	}
	return 1;
}

char* LowLevelBrewer::modeName(ModeType m)
{
    return modeStr[m];
}

void LowLevelBrewer::setBaudRate(SerialPort::BaudRate newBaud)
{
	if(ap == NULL)
        throw MissingCMDObject("BrewerAscPort");
    ap->resetPort(newBaud);
    if(op == NULL)
        throw MissingCMDObject("SCITEC485Port");
	op->resetPort(newBaud);
}

void LowLevelBrewer::status(void)
{
    testDevice(curMode);
    display->updateStatusWindow(brewer[curMode]->status());
}

DeviceStatus LowLevelBrewer::testSerialNumber(void)
{
ModeType    m = curMode;
char       *reply;

    // We want Cosmac or Operating mode for testing purposes
	if(m == loadMode)
		m = operatingMode;
	// ?BREWER.ID != 0 fails in load mode and on old Brewers
    if(brewer[m]->sendCommand("?BREWER.ID", 3) == R_NAK)
    {
        // We might be in operating mode with no config; check SYN status
        reply = lowLevelCommandWithReply("\x16");
        if(toupper(reply[0]) != 'O')
            return R_NAK;
        display->updateSerialNumber(); // No config
        return R_OK;
    }
    serialnum = atoi(brewer[m]->getReply());
    if(serialnum <= 0)
    {
        if (m == operatingMode)
        {
            // We might be in operating mode with no config; check SYN status
            reply = lowLevelCommandWithReply("\x16");
            if(toupper(reply[0]) != 'O')
                return R_NAK;
            display->updateSerialNumber(); // No config
            return R_OK;
        }
        display->updateSerialNumber(-1); // No serial number
        // Assume that result of 0 means an old Brewer (Cosmac)
        return ((serialnum == 0) ? R_NAK : R_OK);
    }
    display->updateSerialNumber(serialnum); // Good config
    return R_OK;
}

int LowLevelBrewer::isCommunicating(ModeType mode)
{
int status = 1;

    testDevice(mode);
    try { brewer[mode]->waitUntilReady(); }
    catch(QSerialTimeout& err)
	{
        try { brewer[mode]->waitUntilReady(); }
        catch(QSerialTimeout& err)
        {
				try { brewer[mode]->waitUntilReady(); }
                catch(QSerialTimeout& err)
				{
					status = 0;
				}
        }
	}
	return status;
}

void LowLevelBrewer::switchTo(ModeType m, SerialPort::BaudRate b)
{
	SerialPort::BaudRate nb = b;    // New baud rate

    // Cannot switch to mode we are already in (except load mode)
    if(m == curMode && m != loadMode)
        wrongMode();
    // Cannot switch from operating <-> Cosmac mode
    else if((curMode == operatingMode && m == cosmacMode) ||
            (curMode == cosmacMode && m == operatingMode))
        badSwitch(m);
    // Are we switching from Cosmac to load mode?
    else if(curMode == cosmacMode)
    {
        // Must be on a new Brewer to do this
		if(isAllowed())
        {
			command("\x04\x04\x04\x04\x04\x04\x04");
            nb = (ap->getBaud()).getValue();
            destroy();
            finishSwitch(m, nb);
        }
    }
	else
	{
		switch(curMode)
		{
		case loadMode:
			// Send baud rate command (Load Mode -> other mode only)
			char temp[4];
			temp[0] = SetBaud;
			temp[1] = '0' + (SerialPort::BaudRateEnum(nb) - SerialPort::baud300);
			temp[2] = '\0';
			command(temp);
			break;
		case cosmacMode:
			nb = (ap->getBaud()).getValue();
			break;
		default:
			nb = (op->getBaud()).getValue();
			break;
		}
        // Send the switch to new mode command
        lowLevelCommand(SwitchTo[m]);
        // Now finish up
        destroy();
        finishSwitch(m, nb);
	}
}

DeviceStatus LowLevelBrewer::testSwitch(void)
{
ModeType     m = curMode;
char        *reply;

    // We want Cosmac or Operating mode for testing purposes
	if(m == loadMode)
		m = operatingMode;
	testDevice(m);
	if (m == cosmacMode)
	{
		if ((reply = brewer[m]->getReply()) != NULL && reply[0] != '\0')
		{
			display->brewerMessage(reply);
			display->wait(2.0);
            command("");
        }
    }
    return testSerialNumber();
}

void LowLevelBrewer::finishSwitch(ModeType mode, SerialPort::BaudRate baud)
{
int   loop = 0;
char *reply = NULL;

	// Throw away the first Brewer reset message (if any), ignore otherwise
	if (mode == cosmacMode)
	{
		// Recreate the ports; otherwise, this code can't possibly work!
		create(baud, mode);
		// Read off and display the (unprompted) reset message strings
		display->bossMessage("Reading Cosmac mode reset messages");
		try
		{
			brewer[mode]->sendCommand(NO_CMD);
			reply = brewer[mode]->getReply();
			if (reply)
				display->brewerMessage(reply);
			brewer[mode]->sendCommand(NO_CMD);
			reply = brewer[mode]->getReply();
			if (reply)
				display->brewerMessage(reply);
		}
		catch(QSerialTimeout& err)
		{
            // Ignore the "missing" message
		}
    	destroy();
	}
	display->write("HOST> Waiting for mode change to complete");
	while(!isWorking(baud, mode, 0) && loop++ < 14)
	{
		display->write(".");
		display->wait(6.0);
	}
	display->write("\n");
	if(loop >= 14)
		reset();
	// If we are entering operating/Cosmac mode, verify that it worked.
	//  Switch back if it did not work.  If we are in load mode, this test
	//  does nothing useful
	if(testSwitch() == R_NAK)
		curMode = loadMode;
	showSetup();
}

int LowLevelBrewer::loadTopCode(const char* file)
{
char          temp[200];
int           loop, loop2, loop3, st = 0;
unsigned long blocksize;  // The block size to send binary data
ifstream      ifile(file, ios::binary);

    if(curMode != loadMode)
		wrongMode();
    else if(!ifile.good())
    {
        display->write("HOST> Error opening file ");
        display->write(file);
        display->write(" for loading\n");
    }
	else
	{
		display->bossMessage("Erasing flash memory");
		// EM (^Y): Erase flash memory
		command("\x19");
		do { loop = checkSYNStatus(); } while (loop == 0 && !kbhit());
		if (loop != -1 && loop != 2)
			display->brewerMessage("Erase failed");
		else if ((loop = atoi(lowLevelCommandWithReply("\x10"))) < 2 || loop > 7)
			display->brewerMessage("Bad block size");
		else
		{
			blocksize = 1 << loop;
			// Skip first 8K of file
			for(loop = 0; loop < 8192; loop++)
				ifile.get();
			if(ifile.good())
			{
				display->write("HOST> Writing file ");
				display->write(file);
				display->write(" to flash");
				// Initialize values
				loop = -1;
				while (!ifile.eof() && ifile.peek() != EOF && !kbhit())
				{
					// One dot per eight successful DLE commands
					if(++loop % 8 == 0)
						display->write(".");
                    // Send a DLE <addr> SP <data0> ... <dataNN> ETB
					ostrstream s(temp, 200);
                    s << "\x10" << setbase(16) << setw(5) << setfill('0')
                      << (8192 + blocksize * loop) << ' ';
                    for (loop2 = 0; loop2 < blocksize / 4; loop2++)
					{
                        // Read four bytes, write five ASCII base 88 chars
						char strng[5];
                        unsigned long t;
                        t = 65536L * 256L * ifile.get();
                        t += 65536L * ifile.get();
						t += 256L * ifile.get();
                        t += ifile.get();
                        for (loop3 = 4; loop3 >= 0; loop3--)
                        {
                            strng[loop3] = (char)(t % 88) + 32;
							t /= 88;
                        }
                        for (loop3 = 0; loop3 < 5; loop3++)
                            s << strng[loop3];
					}
                    s << "\x17" << ends;
                    command(temp);
                }
                display->write("\n");
            }
            else
            {
                display->write("HOST> Error reading file ");
                display->write(file);
                display->write(" during loading\n");
            }
			do { loop = checkSYNStatus(); } while (loop == 0 && !kbhit());
			if(loop > 0)
            {
                display->brewerMessage("Top code successfully loaded");
                st = 1;
            }
			else
                display->brewerMessage("Error loading top code");
		}
    }
    ifile.close();
    return st;
}

void LowLevelBrewer::warmReset(void)
{
    if(curMode == cosmacMode)
        wrongMode();
    else
    {
        lowLevelCommand("\x0e");    // SO (^N): reset Brewer
        display->wait(5.0);
		brewer[curMode]->waitUntilReady();
        display->updateStatusWindow(R_OK);
    }
}

void LowLevelBrewer::save(void)
{
    if(curMode == loadMode)
        wrongMode();
    else
    {
        brewer[curMode]->sendCommand("SAVE", 20);
        display->wait(5.0);
        display->brewerMessage(brewer[curMode]->getReply());
	}
}

int LowLevelBrewer::checkSYNStatus(void)
{
char*   reply;

    switch(curMode)
    {
    case cosmacMode:
		wrongMode();
        return 0;
    default:
        reply = lowLevelCommandWithReply("\x16");
        break;
	}
	// (^VMRR) SYN: status returned (RR is flash status [1-2])
	return (reply[2] - '0') * (reply[1] == '-' ? -1 : 1);
}

void LowLevelBrewer::command(const char* cmd)
{
	testDevice(curMode);
	if(brewer[curMode]->sendCommand(cmd, 20) == R_ACK)
		brewer[curMode]->waitUntilReady();
}

char* LowLevelBrewer::commandWithReply(const char* cmd)
{
	testDevice(curMode);
    brewer[curMode]->sendCommand(cmd, 20);
    return brewer[curMode]->getReply();
}

void LowLevelBrewer::lowLevelCommand(const char* cmd)
{
    testDevice(curMode);
    if(brewer[curMode]->sendLowLevelCommand(cmd, 20) == R_ACK)
        brewer[curMode]->waitUntilReady();
}

char* LowLevelBrewer::lowLevelCommandWithReply(const char* cmd)
{
    testDevice(curMode);
    brewer[curMode]->sendLowLevelCommand(cmd, 20);
    return brewer[curMode]->getReply();
}

void LowLevelBrewer::showSetup(void)
{
char        temp[80];
ostrstream  s(temp, 80);

    testDevice(curMode);
    s << "Using " << modeName(curMode) << " mode at ";
    if(curMode == cosmacMode)
        s << (ap->getBaud()).getValue();
    else
		s << (op->getBaud()).getValue();
    s << " baud, on COM" << ((port == SerialPort::com1) ? '1' : '2')
      << ": tracing is " << TraceModes[tracing] << ends;
    display->bossMessage(temp);
}

void LowLevelBrewer::badSwitch(ModeType toMode)
{
char        temp[80];
ostrstream  s(temp, 80);

    s << "Cannot switch to " << modeStr[toMode] << " from " << modeStr[curMode]
      << " mode" << ends;
    display->bossMessage(temp);
}

void LowLevelBrewer::testDevice(ModeType i)
{
    if(brewer[i] == NULL)
        throw MissingDeviceFatal(i);
    if(ap == NULL && i == cosmacMode)
        throw MissingDeviceFatal(i);
    if(op == NULL && i != cosmacMode)
        throw MissingDeviceFatal(i);
}

int LowLevelBrewer::isAllowed(void)
{
    if(oldBrewer)
		display->bossMessage("Command not allowed on old Brewers");
    return !oldBrewer;
}

void LowLevelBrewer::wrongMode(void)
{
char        temp[80];
ostrstream  s(temp, 80);

    s << "Command not allowed in " << modeName(curMode) << " mode" << ends;
    display->bossMessage(temp);
}

