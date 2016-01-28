/*
 * s485port.hpp
 *
 * This is the header for the SCI-TEC 485 Port class.  This protocol is used
 *  to communicate with any number of SCI-TEC 485-based instruments over a
 *  single serial port.  It is reponsible only for framing of messages.  It
 *  is not concerned with the content of messages, only their structure.
 *
 * SCITEC485MesgExchange is used to handle sending messages and awaiting the
 *  reply.  To use, simply create one for the desired port, fill in the cmd
 *  field with your message and send.  Then, periodically check for the
 *  completion of the exchange.
 *
 * Most of the functionality is found in qserial.?pp and s485exc.hpp.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#ifndef __S485PORT_HPP
#define __S485PORT_HPP

#include ".\port\s485exc.hpp"

// These are the addresses for all messages sent to the PC
#define BOSS_ADDRESS    "BOSS"
#define HOST_ADDRESS    "HOST"
#define ADDRESS_LEN     4

class SCITEC485Port : public QueuedPort
{
friend SCITEC485MesgExchange;
//
// Class Variables
//
protected:
    int     byteCount;
    char    addr[ADDRESS_LEN + 1];

//
// Methods
//
protected:
    virtual void interpret(void);   // Collect/analyze some input characters
    virtual void transmit(void);    // Format and send message at head of queue

public:
    SCITEC485Port(CommPort com=com1, BaudRate baud=baud9600,
                Parity parity=noParity,
                DataBits data=data8, StopBits stop=stop1,
                HandShakeSelection sig=noShake )
            : QueuedPort(com, baud, parity, data, stop, sig) {};
            // See serial.hpp for the types CommPorts and BaudRate etc.
};

#endif  // __S485PORT_HPP

