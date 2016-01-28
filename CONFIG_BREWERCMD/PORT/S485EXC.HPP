/*
 * s485exc.hpp
 *
 * SCI-TEC 485 Port Message Exchange Interface
 *
 * This is the header for the SCITEC485MesgExchange class.  It implements a
 *  timeout variable and a header variable for the name of the instrument
 *  (required for all instruments that use the SCI-TEC 485 port protocol).
 *
 * Refer to s485port.hpp for further details.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#ifndef __S485EXC_HPP
#define __S485EXC_HPP

#include ".\qserial\qserial.hpp"

const int maxCmd = 230;     // Maximum command length
const int maxReply = 200;   // Maximum reply length

class SCITEC485MesgExchange : public MessageExchange
{
friend QueuedPort;
//
// Class Variables
//
public:
    int     timeOut;
    char    header[10];    // The address for outgoing messages
    int     headerLength;  // Number of bytes in the header

//
// Methods
//
public:
    SCITEC485MesgExchange(QueuedPort* p)
            : MessageExchange(p, new char[maxCmd+3], new char[maxReply+3]),
              timeOut(10), headerLength(0)
            { strcpy(header, ""); }
    SCITEC485MesgExchange(QueuedPort* p, const char* addr)
            : MessageExchange(p, new char[maxCmd+3], new char[maxReply+3]),
              timeOut(10)
            { changeAddress(addr); }
        // Constructors
    ~SCITEC485MesgExchange() { delete cmd; delete reply; }
        // Destructor
    void changeAddress(const char* addr)
            { headerLength = strlen(addr); strcpy(header, addr); }
};

class TransmissionTooLong : public ExcTooBig, public ExcCommError, public virtual BaseException
{
public:
    TransmissionTooLong(void) :
        BaseException(helpdescribe("Transmission too long")) {}
};

#endif  /* __S485EXC_HPP */

