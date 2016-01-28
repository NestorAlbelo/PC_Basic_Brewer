/*
 * bascport.hpp
 *
 * Brewer ASCII Port Serial Interface
 *
 * This is the header for the BrewerAscPort class.  BrewerAscPort is used
 *  to communicate with any version Brewer over a single serial port.  It
 *  is reponsible only for framing of messages.  It is not concerned with
 *  the content of messages, only their structure.
 *
 * MessageExchange is used to handle sending messages and awaiting the
 *  reply.  To use, simply create one for the desired BrewerAscPort, fill
 *  in the cmd field with your message and send.  Then, periodically check
 *  for the completion of the exchange.
 *
 * Most of the functionality is found in qserial.?pp.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#ifndef __BASCPORT_HPP
#define __BASCPORT_HPP

#include ".\qserial\qserial.hpp"

#define NO_CMD  "--" // Special command for retrieving an "unprompted" reply
					 //  (Example: reset message!)

class BrewerAscPort : public QueuedPort
{
friend MessageExchange;
//
// Class Variables
//
protected:
	int     charIndex;

//
// Methods
//
protected:
	virtual void interpret();   // Collect/analyze some input characters
	virtual void transmit();    // Format and send message at head of queue

public:
	BrewerAscPort(CommPort com=com1, BaudRate baud=baud1200, Parity parity=noParity,
					DataBits data=data8, StopBits stop=stop1,
					HandShakeSelection sig=noShake )
				: QueuedPort(com, baud, parity, data, stop, sig) {}
		// Constructor.  See SERIAL.HPP for the types CommPorts
		//   and BaudRate etc.
};

#endif  /* __BASCPORT_HPP */

