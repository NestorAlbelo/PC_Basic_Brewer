/*
 * brewcmd.cpp
 *
 * This utility allows a user to command and control any version or type
 *  of Brewer instrument.  It properly handles both the old (ASCII)
 *  interface and the new (Multidrop) interface.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#include <conio.h>
#include <constrea.h>
#include <ctype.h>
#include <fstream.h>
#include <setjmp.h>
#include <stdio.h>
#include <strstrea.h>
#include ".\device\device.hpp"
#include ".\cmd\cmdcomm.hpp"
#include ".\cmd\cmddisp.hpp"

// List of valid commands; MUST be in alphabetical order
static char*    cmdList[] = {
	"COSMACMODE",
    "GETCFG",
    "HELP",
    "INIT",
    "LOAD",
    "LOADMODE",     // 5
    "OPMODE",
    "PUTCFG",
    "QUIT",
    "READLOG",
    "REPEAT",       // 10
    "RESET",
    "RUN",
    "SAVE",
    "SEND",
    "STATUS",       // 15
    "SYN",
    "ZERO",
    "",             // Must be present!
};

// Brief information about each command
static char*    cmdHelp[] = {
    "COSMACMODE (<baud>):  Switch to Cosmac mode using optional baud rate",
    "GETCFG <file>:  Write configuration to <file> from the Brewer",
    "HELP (<command>):  Help on commands",
    "INIT:  Repeats the mode detection process",
    "LOAD <file>:  Load new top-level code from <file>",
    "LOADMODE (<baud>):  Switch to load mode using optional baud rate",
    "OPMODE (<baud>):  Switch to operating mode using optional baud rate",
    "PUTCFG <file>:  Read configuration from <file> into the Brewer",
    "QUIT:  Exit program",
    "READLOG:  Read all log entries from Brewer",
    "REPEAT <cmd>:  Repeat the command sequence <cmd> until stopped",
    "RESET:  Warm resets the Brewer",
    "RUN <file>:  Run commands from <file>",
    "SAVE:  Save configuration to flash",
    "SEND <command>: Send a low-level command to the Brewer",
	"STATUS:  Display program status",
    "SYN:  Test Brewer board status",
    "ZERO:  Zero the reset counter",
    "",         // Must be present!
};

// Messages
static char* missingParam = "Required parameter missing";

SerialCommunication sc(1);
    // Sets up buffer space; see Serial.hpp

jmp_buf abortBuffer;
	// used to recover from aborts

// procedure to clean up during user abort
int exit_program(void)
{
	cout << "User aborted program" << endl;
    // Explicit destructor, required to clean up correctly
	sc.SerialCommunication::~SerialCommunication();
	longjmp(abortBuffer, 1);
	return 0;
}

int findCommand(char* cmd);

int main(void)
{
	if (setjmp(abortBuffer))
		return 3;
	ctrlbrk(exit_program); // Set up to handle CTRL-Break from user
	setcbrk(1);
	// Attempt to remove the trace file, so that it doesn't grow out of control
	remove ("TRACEQP.TXT");

	try
{
BrewerCMDINI    ini(10);            // Initialization file
Display         display(&ini);      // All window functions are here
Brewer          brewer(&ini, &display); // All Brewer functions are here
char*           userCommand;        // Command typed by user
char*           parameter;          // Parameter to command typed by user
char*           result;             // Response to command from Brewer
char            temp[200];          // Buffer
ostrstream*     s;                  // Stream to fill buffer
int             index;              // Command index (-1 if not a built-in command)
int             loop, loop2;        // Loop indeces

	do
    {
        // Clear Brewer status.  VERY IMPORTANT!!
        brewer.status();
        display.updateStatusWindow(R_OK);
        userCommand = display.getUserInput();
        display.updateStatusWindow(R_ACK);
        parameter = userCommand;
		if((index = findCommand(userCommand)) != -1)
        {
            parameter += strlen(cmdList[index]);
            while(isspace(*parameter))
				parameter++;
        }
//        s = new ostrstream(temp, 200);
//        *s << "Decoded message as " << index;
//        if(index == -1)
//            *s << ": Unknown command " << userCommand;
//        else
//            *s << ": " << cmdList[index] << " with parameter " << parameter;
//        *s << ends;
//        delete s;
//        display.bossMessage(temp);
        switch(index)
        {
        case 0:     // COSMACMODE
            if(*parameter)
                brewer.switchTo(cosmacMode, SerialPort::BaudRate(atoi(parameter)));
            else
                brewer.switchTo(cosmacMode, SerialPort::baud1200);
            break;
        case 1:     // GETCFG: Write Brewer configuration
            if(brewer.mode() == loadMode)
				brewer.wrongMode();
            else if(!*parameter)
                display.bossMessage(missingParam);
            else if(brewer.isAllowed())
            {
                ifstream ifile(parameter);
				ofstream ofile("brewcmd.tmp");
                while(kbhit())   // ensure that kbhit is cleared
                    getch();
                while(ifile.good() && !kbhit())
                {
                    temp[0] = '?';  // We want to read values from the Brewer
                    do { temp[1] = ifile.get(); ofile.put(temp[1]); } while(isspace(temp[1]));
                    if(temp[1] == ';')
                    {
                        ifile.getline(temp, 200, '\n');
                        ofile << temp << endl;
                    }
					else if(ifile.good())
                    {
                        // Save the second character, which we read already
                        ifile.getline(&temp[2], 198, '=');
                        // Convert to uppercase
                        for(loop = 0; loop < strlen(temp); loop++)
							temp[loop] = toupper(temp[loop]);
                        ofile << (&temp[2]) << '=';
                        result = brewer.commandWithReply(temp);
                        ofile << result << endl;
                        ifile.getline(temp, 200, '\n');
                    }
                }
                ifile.close();
                ofile.close();
                remove("brewcmd.bak");
                rename(parameter, "brewcmd.bak");
                rename("brewcmd.tmp", parameter);
			}
            break;
        case 2:     // HELP
            if(*parameter)
            {
                if((index = findCommand(parameter)) == -1)
					display.write("  Unknown command\n");
                else
                {
                    display.write("  ");
                    display.write(cmdHelp[index]);
                    display.write("\n");
                }
            }
            else
            {
                display.write("Valid commands are:\n");
                for(loop = 0; *cmdList[loop]; loop++)
				{
                    s = new ostrstream(temp, 200);
                    *s << "  " << cmdList[loop];
                    for(loop2 = strlen(cmdList[loop]); loop2 < 12; loop2++)
                        *s << " ";
                    if((loop + 1) % 5 == 0 || !*cmdList[loop + 1])
						*s << endl;
                    *s << ends;
                    delete s;
                    display.write(temp);
                }
            }
            break;
        case 3:     // INIT: Redetect communications
            brewer.reset();
            break;
        case 4:     // LOAD: Load top code
            if(!*parameter)
                display.bossMessage(missingParam);
            else
                brewer.loadTopCode(parameter);
			break;
        case 5:     // LOADMODE
            if(*parameter)
                brewer.switchTo(loadMode, SerialPort::BaudRate(atoi(parameter)));
            else
                brewer.switchTo(loadMode, SerialPort::baud9600);
			break;
        case 6:     // OPMODE
            if(*parameter)
                brewer.switchTo(operatingMode, SerialPort::BaudRate(atoi(parameter)));
            else
                brewer.switchTo(operatingMode, SerialPort::baud9600);
            break;
        case 7:     // PUTCFG: Read Brewer configuration
            if(brewer.mode() == loadMode)
                brewer.wrongMode();
            else if(!*parameter)
                display.bossMessage(missingParam);
			else if(brewer.isAllowed())
            {
                ifstream ifile(parameter);
                while(kbhit())   // ensure that kbhit is cleared
                    getch();
                while(ifile.good() && !kbhit())
				{
                    temp[0] = '!';  // We want to write values to the Brewer
                    do { temp[1] = ifile.get(); } while(isspace(temp[1]));
                    if(temp[1] == ';')
                        ifile.getline(temp, 200, '\n');
                    else if(ifile.good())
                    {
                        // Save the second character, which we read already
                        ifile.getline(&temp[2], 198, '=');
                        strcat(temp, " ");  // Separator
                        ifile.getline(&temp[strlen(temp)], 200 - strlen(temp), '\n');
                        // Convert to uppercase
						for(loop = 0; loop < strlen(temp); loop++)
                            temp[loop] = toupper(temp[loop]);
                        brewer.command(temp);
                    }
                }
                ifile.close();
                brewer.testSerialNumber();
			}
            break;
        case 8:     // QUIT
            break;
        case 9:     // READLOG
            if(brewer.mode() == loadMode)
                brewer.wrongMode();
            else if(brewer.isAllowed())
            {
                do
                {
                    result = brewer.commandWithReply("LOGENTRY");
					display.brewerMessage(result);
                } while(strcmp(result, " All log items reported.") != 0 && !kbhit());
            }
            break;
        case 10:    // REPEAT
            if(brewer.mode() == loadMode)
				brewer.wrongMode();
            else if(!*parameter)
                display.bossMessage(missingParam);
            else do
            {
                result = brewer.commandWithReply(parameter);
                display.brewerMessage(result);
            } while(!kbhit());
            break;
        case 11:    // RESET
            brewer.warmReset();
            break;
		case 12:    // RUN
            if(brewer.mode() == loadMode)
                brewer.wrongMode();
            else if(!*parameter)
                display.bossMessage(missingParam);
            else
			{
                ifstream ifile(parameter);
                while(kbhit())   // ensure that kbhit is cleared
                    getch();
                while(ifile.good() && !kbhit())
                {
                    do { temp[0] = ifile.get(); } while(isspace(temp[0]));
                    // Save the first character, which we read already
                    ifile.getline(&temp[1], 199, '\n');
                    // Convert to uppercase
                    for(loop = 0; loop < strlen(temp); loop++)
                        temp[loop] = toupper(temp[loop]);
                    if(temp[0] == ';')
                        display.brewerMessage(temp);
					else if(strlen(temp) > 0)
                    {
                        result = brewer.commandWithReply(temp);
                        if(strlen(result) > 0)
                            display.brewerMessage(result);
                    }
                }
				ifile.close();
            }
            break;
        case 13:    // SAVE
            brewer.save();
            break;
        case 14:    // SEND
            if(!*parameter)
                display.bossMessage(missingParam);
            else
            {
                s = new ostrstream(temp, 200);
				*s << brewer.lowLevelCommandWithReply(parameter) << ends;
                display.brewerMessage(temp);
                delete s;
            }
            break;
        case 15:    // STATUS
			brewer.status();
            brewer.showSetup();
            break;
        case 16:    // SYN
            if(brewer.mode() == cosmacMode)
                brewer.wrongMode();
            else
            {
                s = new ostrstream(temp, 200);
                *s << brewer.lowLevelCommandWithReply("\x16") << ends;
                display.brewerMessage(temp);
                delete s;
			}
            break;
        case 17:    // ZERO
            if(brewer.mode() == cosmacMode)
                brewer.wrongMode();
            else
				brewer.lowLevelCommand("\x1e"); // RS (^^): zero reset counter
            break;
        default:
            if(strcmp(userCommand, "") == 0)
                brewer.status();
            else if(brewer.mode() == loadMode)
                brewer.wrongMode();
            else if(userCommand[strlen(userCommand) - 1] == 'A' &&
                    !isalpha(userCommand[strlen(userCommand) - 2]))
                display.bossMessage("Do not use the A command, use the REPEAT command instead");
            else
            {
				result = brewer.commandWithReply(userCommand);
                display.brewerMessage(result);
			}
            break;
        }
        while(kbhit())
			getch();
    } while(index != 8); // QUIT
    return 0;
}
    catch(BaseException& err)
    {
		cout << "An exception occurred" << endl;
		err.display();
		cout << "Hit a key to terminate program" << endl;
		while (!kbhit())
			continue;
		return 3;
	}
}

int findCommand(char* cmd)
{
	int index = -1;
	// Note that cmdList[..] must be in alphabetical order, otherwise
    //  this code doesn't work properly
    for(int loop = 0; *cmdList[loop]; loop++)
        if(strncmp(cmd, cmdList[loop], strlen(cmdList[loop])) == 0)
            index = loop;
    return index;
}

