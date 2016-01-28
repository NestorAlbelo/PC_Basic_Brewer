/*
 * serial.hpp
 *
 * Generic Serial Port Interface
 *
 * Copyright(C) 1991, 1995 SCI-TEC Instruments Inc.
 */

#ifndef __SERIAL_HPP
#define __SERIAL_HPP

#include ".\except\except.hpp"

class SerialPort
{
//
// Class Constants
//
public:
    enum CommPort {nocom = 0, com1 = 1, com2 = 2};
    enum BaudRateEnum {baud110, baud150, baud300, baud600, baud1200,
                    baud2400, baud4800, baud9600, baud19200,
                    baud38400, baud57K, baud115K, lastBaud};
    class BaudRate
    {
        private: BaudRateEnum baud;
        static const long table[SerialPort::lastBaud];
        public:
            BaudRate(BaudRateEnum b = baud300) : baud(b) {}
            BaudRate(long rate);
            operator BaudRateEnum() { return baud; }
            operator int() { return baud; }
            long getValue() { return table[baud]; }
    };
#ifdef _Windows
    enum Parity {noParity=NOPARITY, oddParity=ODDPARITY, evenParity=EVENPARITY};
    enum DataBits {data7=7, data8=8};
    enum StopBits {stop1=ONESTOPBIT, stop2=TWOSTOPBITS};
#else
	enum Parity   {noParity, oddParity, evenParity};
	enum DataBits {data7, data8};
	enum StopBits {stop1, stop2};
#endif
	enum HandShakeSelection {noShake, bothShakes, onlyCTS, onlyDSR};

//
// Class Variables
//
private:
#ifdef _Windows
    int                 idComDev;
    int                 charRead;
    int                 charValid;
#else
    BaudRate            thisBaud;
    Parity              thisParity;
    DataBits            thisData;
    StopBits            thisStop;
    HandShakeSelection  thisHs;
#endif

protected:
    CommPort            thisPort;

public:

//
// Methods
//
protected:

public:
    SerialPort(CommPort port, BaudRate baud, Parity parity,
                DataBits data, StopBits stop, HandShakeSelection sig );
            // Creates an interrupt-driven port with the stated
            //	characteristics.  Note that HandShakeSelection is
            //  ignored for the Windows version of this class; this
            //  can be changed if you wish
    virtual ~SerialPort();
    void pause(void);
            // Release the serial port so other programs can use it
    void resume(void);
            // Retake the serial port
    void resetPort(BaudRate baud, Parity parity = noParity,
                DataBits data = data8, StopBits stop = stop1);
            // Modifies the characteristics of the port to the new
            //	values provided
    int brk(int control);
            // Sets (1), clears (0), or reads (2) the Line Break signal.
            //  Set and clear always returns 0; reading returns
            //      0   Line Break disabled
            //      1   Line Break enabled
    int getch(void);
            // Reads a single character from port.  Use test() before
            //	getch() to be sure that a character exists
    int send(char* ch, int count);
            // Sends count bytes from ch out the port.  Returns the
            //  number of bytes actually sent.  An exception is
            //  thrown if there is an error
    int send(char* ch) { return send(ch, strlen(ch)); }
            // Sends all bytes in string ch (and see above)
    long int test(void);
            // Returns a nonzero value if there is a byte waiting.
    BaudRate getBaud(void)
#ifdef _Windows
        ;
#else
        { return thisBaud; }
#endif
            // Returns the current baud rate.
};



class SerialCommunication
{
//
// Methods
//
public:
    SerialCommunication(int kOfBuffer = 2);
            // kOfBuffer should be at least 1K/port in use
    ~SerialCommunication();
};

class SerialNoInit : public ExcCommError, public virtual BaseException
{
public:
    SerialNoInit(void);
};

class CommPortNotPresent : public ExcCommError, public virtual BaseException
{
public:
    CommPortNotPresent(int p);
};

class InvalidCommPort : public ExcSyntaxError, public virtual BaseException
{
public:
    InvalidCommPort(int p);
};

class InvalidBaudRate : public ExcSyntaxError, public virtual BaseException
{
public:
    InvalidBaudRate(long b);
};

class SerialXMitError : public ExcCommError, public virtual BaseException
{
public:
    SerialXMitError(const char* s, int i);
};

#endif  /* __SERIAL_HPP */

