/*
 * qserial.hpp
 *
 * Queued Serial Port Driver Interface
 *
 * QueuedPort is used to communicate with device(s) over a single
 *   serial port.  It is designed to handle the sending and
 *   receiving of messages, as provided by one or more
 *   MessageExchanges.
 *   Since QueuedPort must understand the protocol used on its
 *   port, the implementation must define two functions to handle
 *   the content of messages:
 *      a. transmit()  must be defined.  It must initialize the
 *                     character buffer provided, properly format
 *                     the message (based on the protocol to be
 *                     used) and then call mark() to specify a
 *                     timeout.  A default timeout length is
 *                     provided.
 *      b. interpret() must be defined.  It must decide what to
 *                     do with the single character just received,
 *                     and add it to the provided character buffer
 *                     if necessary.  Error checking, such as
 *                     special headers and checksums, are the
 *                     implementation's job to handle.  If needed,
 *                     mark() can also be used here for timeouts.
 *
 * MessageExchange is used to handle sending messages and awaiting
 *   the reply.  It is normally the base class for a class specific
 *   to the port protocol.  To use a MessageExchange simply fill in the cmd
 *   field and call the send() method.  Periodically use the done()
 *   method to check for completion of the exchange.
 *
 * Errors are thrown when they are received.
 *  The errors thrown are not specified here as the errors thrown
 *  by transmit and interpret are unknown at this point.  Actually, the only
 *  error QSerial itself throws is QSerialTimeout.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */


#ifndef __QSERIAL_HPP
#define __QSERIAL_HPP

#include <time.h>
#include ".\serial\serial.hpp"
#include ".\except\except.hpp"

class MessageExchange;

class QueuedPort : public SerialPort
{
friend MessageExchange;

public:
	enum ExchangeResult {
		inProgress = -128,
            // All errors between -128 and -64 are inProgress status values
		awaitingPort = -64,
        error = -1,
		success = 0                 	// no error on the exchange
	};
    enum    Tracing { off, text, full };

//
// Class Variables
//
protected:
    MessageExchange*    first;          // Queue of pending messages
    MessageExchange*    last;
    Tracing             tracing;
    char                newCharacter;   // The latest byte of the reply
    char*               reply;          // Buffer to store the reply in
    time_t              waitDoneTime;   // Timeout on next character

public:
    ExchangeResult      status;         // Status flag

//
// Methods
//
protected:
    void            enqueue(MessageExchange* msg);
            // Queue to send cmd and receive reply
    void            dequeue(MessageExchange* msg);
            // Remove it from the queue
    void            run();
            // Provides some CPU time to the port
    virtual void    interpret() = 0;
            // Analyze input character, and status, and decide what to do with it
    virtual void    transmit() = 0;
            // Format and send message at head of queue
    void            mark(int delay = 2) { waitDoneTime = time(0) + delay; }
            // Set up to timeout delay seconds from now
    void            put(char* ch, int count);
            // Sends count bytes from ch out the port.  Returns when
            //  transmission is complete.  An exception is thrown if
            //  an error occurs
    void            put(char* ch) { put(ch, strlen(ch)); }
            // Sends all bytes in string ch (and see above)

public:
    QueuedPort(CommPort com=com1, BaudRate baud=baud9600,
                Parity parity=noParity,
                DataBits data=data8, StopBits stop=stop1,
				HandShakeSelection sig=noShake );
			// Constructor.  See SERIAL.HPP for the types CommPorts
			//  and BaudRate
	~QueuedPort();
			// Destructor
            // Reset port characteristics
    void            trace(Tracing t) { tracing = t; }
            // Set type of tracing to perform.  With tracing in effect
            //	each exchange is logged in TRACEQP.TXT
    int             isIdle(void) { return (last == NULL); }
            // Indicate whether there is outstanding data to be sent or not
};

class MessageExchange
{
friend QueuedPort;

public:

//
// Class Variables
//
protected:
    MessageExchange*    next;
            // Queue of waiting messages
    QueuedPort*         dest;
            // The destination of the exchange

public:
    char*               cmd;
            // While status=inProgress, the reply (below) may be modified
    char*               reply;
    QueuedPort::ExchangeResult  status;
            // Obtained from the QueuedPort object

//
// Methods
//
protected:

public:
    MessageExchange(QueuedPort* p, char* cmdSpace, char* replySpace);
    ~MessageExchange() {}
    void            abort() { dest->dequeue(this); dest->run(); }
            // Terminate exchange prematurely
    void            send() { dest->enqueue(this); dest->run(); }
    int             done() { dest->run(); return status > QueuedPort::awaitingPort; }
            // 1 iff exchange is finished
    int             succeeded() { return status == QueuedPort::success; }
            // 1 iff the exchange succeeded
    char*           explain();
            // String explaining the current status code
    void            exchange();
            // Sends cmd and receives reply. Returns when exchange
            //  is complete.
};

class QSerialTimeout : public ExcTimeout, public ExcCommError,
            public virtual BaseException
{
public:
    QSerialTimeout(void);
};

#endif  /* __QSERIAL_HPP */

