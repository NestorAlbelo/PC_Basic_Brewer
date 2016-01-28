/*
 * serial.cpp
 *
 * Generic Serial Port Source File
 *
 * Copyright(C) 1992, 1995 SCI-TEC Instruments Inc.
 *
 * Usage notes:  You can #define _FAKE_SERIAL_PORT if you wish to
 *               test code that uses this object without data from
 *               a serial port available.
 */

extern "C" {
    #include <xc.h>
    #include <eclproto.h>
};

#include <iostream.h>
#include ".\serial\serial.hpp"

#ifdef _FAKE_SERIAL_PORT
#include <time.h>
#endif

#ifndef _NO_VERSION_INFO
#   include ".\version\version.hpp"

    static Version ver("Serial", 1, 6);
#endif
//static int oldKOfBuffer;

const long SerialPort::BaudRate::table[SerialPort::lastBaud] = {
    110, 150, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400L, 57400L, 115200L
};

SerialCommunication::SerialCommunication(int kOfBuffer)
{
#ifdef _FAKE_SERIAL_PORT
    cout << "Constructing " << kOfBuffer << " kB serial port" << endl;
#else
//    oldKOfBuffer = kOfBuffer;
    int value = xc_entr(kOfBuffer);
    if (value < -1 || value > 0)
    {
        throw SerialNoInit();
    }
#endif
}

SerialCommunication::~SerialCommunication()
{
#ifdef _FAKE_SERIAL_PORT
    cout << "Destroying serial port" << endl;
#else
    xc_exit();
#endif
}

SerialPort::BaudRate::BaudRate(long rate)
{
    for(BaudRateEnum x=baud110; x<lastBaud; x = BaudRateEnum(x+1))
        if(table[x] == rate) { baud = x; return; }
    throw (InvalidBaudRate(rate));
}

SerialPort::SerialPort(CommPort port, BaudRate baud, Parity parity,
                DataBits data, StopBits stop, HandShakeSelection sig )
    : thisBaud(baud), thisParity(parity), thisData(data),
      thisStop(stop), thisHs(sig), thisPort(port)
{
#ifdef _FAKE_SERIAL_PORT
    cout << "Initializing serial port to COM" << (port - com1 + 1) << ", "
         << baud.getValue() << " baud, " << (data == data8 ? '8' : '7')
         << (parity == noParity ? 'N' : (parity == evenParity ? 'E' : 'O'))
         << (stop == stop1 ? '1' : '2')
         << ", handshaking set to " << sig << endl;
    cout << "Parameter call list to ECL is xc_init(" << (port - com1) << ", "
         << (baud) << ", " << parity << ", " << data << ", " << stop << ")" << endl;
    randomize();
#else
    if(port != com1 && port != com2)
        throw InvalidCommPort(port);
    if(xc_tport(port - com1))
        throw CommPortNotPresent(port);
    xc_link(port - com1, sig);
    xc_init(port - com1, baud, parity, data, stop);
    xc_flush(thisPort - com1);
#endif
}

SerialPort::~SerialPort()
{
#ifdef _FAKE_SERIAL_PORT
    cout << "Uninitializing serial port" << endl;
#else
    xc_flush(thisPort - com1);
    xc_unlk(thisPort - com1);
#endif
}

void SerialPort::pause()
{
#ifdef _FAKE_SERIAL_PORT
    cout << "Suspending serial port" << endl;
#else
    xc_unlk(thisPort - com1);
#endif
}

void SerialPort::resume()
{
#ifdef _FAKE_SERIAL_PORT
    cout << "Reconnecting to serial port" << endl;
#else
    xc_link(thisPort - com1, thisHs);
	xc_init(thisPort - com1, thisBaud, thisParity, thisData, thisStop);
#endif
}

void SerialPort::resetPort(BaudRate baud, Parity parity,
                DataBits data, StopBits stop )
{
    thisBaud = baud;
    thisParity = parity;
    thisData = data;
    thisStop = stop;
#ifdef _FAKE_SERIAL_PORT
    cout << "Resetting serial port to COM" << (thisPort - com1 + 1) << ", "
         << baud.getValue() << " baud, " << (data == data8 ? '8' : '7')
         << (parity == noParity ? 'N' : (parity == evenParity ? 'E' : 'O'))
         << (stop == stop1 ? '1' : '2')
         << ", handshaking set to " << thisHs << endl;
    cout << "Parameter call list to ECL is xc_init(" << (thisPort - com1) << ", "
         << baud << ", " << parity << ", " << data << ", " << stop << ")" << endl;
#else
    xc_init(thisPort - com1, baud, parity, data, stop);
#endif
}

int SerialPort::brk(int control)
{
#ifdef _FAKE_SERIAL_PORT
    cout << "Attempt to send break signal as brk(" << control << ")" << endl;
    return 0;
#else
    return xc_brk(thisPort - com1, control);
#endif
}

#ifdef _FAKE_SERIAL_PORT
    /* states are as follows:
     *   0  haven't transmitted yet
     *   1  need to send ASCII SOH
     *   2  send standard ASCII (printable) characters
     *   3  need to send ASCII EOT
     */
static int state = 0;
#endif

int SerialPort::getch()
{
#ifdef _FAKE_SERIAL_PORT
    int x;
    switch(state)
    {
    case 0:
    case 1:
        x = 1;
        state = 2;
        break;
    case 2:
        x = random(64) + 32;
        break;
    case 3:
        x = 4;
        state = 0;
        break;
    }
    return x;
#else
    return xc_getc(thisPort - com1);
#endif
}

int SerialPort::send(char* ch, int count)
{
#ifdef _FAKE_SERIAL_PORT
    cout << "Sent sequence ";
    for(int loop = 0; loop < count; loop++)
        cout << ch[loop];
    cout << endl;
    state = 1;
    return count;
#else
int     temp, sent = count;

    // Flush any unpended transmissions on the port.  Since we always
    //  use pended transmissions, unpended transmissions can only occur
    //  in exceptional circumstances.  Flushing these unpended
    //  transmissions will increase reliability of the software as a
    //  whole.
    xc_uflush(thisPort - com1);
    temp = xc_put(thisPort - com1, ch, &sent);
    if(temp)
        throw SerialXMitError(ch, temp);
    return sent;
#endif
}

long int SerialPort::test()
{
#ifdef _FAKE_SERIAL_PORT
    long int x;
    switch(state)
    {
    case 0:
        x = 0L;
        break;
    case 1:
    case 3:
        x = 1L;
        break;
    case 2:
        if(random(8))
            { x = 1L; }
        else
            { x = 0L; state = 3; }
        break;
    }
    return x;
#else
    return xc_test(thisPort - com1);
#endif
}

SerialNoInit::SerialNoInit(void)
    : BaseException(
        helpdescribe("Could not initialize serial communications"))
{
}

CommPortNotPresent::CommPortNotPresent(int p)
    : BaseException(helpdescribe("COM port %i is not present", p))
{
}

InvalidCommPort::InvalidCommPort(int p)
    : BaseException(helpdescribe("%i is not a valid COM port", p))
{
}

InvalidBaudRate::InvalidBaudRate(long b)
    : BaseException(helpdescribe("%li is not a valid baud rate", b))
{
}

SerialXMitError::SerialXMitError(const char* s, int i)
    : BaseException(
        helpdescribe("Error %i occurred while transmitting string %s", i, s))
{
}

