/*
 * s485port.cpp
 *
 * This is the implementation for the SCITEC485Port class.
 *
 * Copyright(C) 1995, SCI-TEC Instruments Inc.
 */

#include <string.h>
#include ".\port\s485port.hpp"

#ifndef _NO_VERSION_INFO
#   include ".\version\version.hpp"

    static Version ver("S485Port", 1, 2);
    // See Version.hpp
#endif

class UndecodableResponse : public ExcCommError, public virtual BaseException
{
public:
    UndecodableResponse(void) :
        BaseException(helpdescribe("Undecodable response")) {}
};

class BadReturnAddress : public ExcCommError, public virtual BaseException
{
public:
    BadReturnAddress(void) :
        BaseException(helpdescribe("Bad return address")) {}
};

void SCITEC485Port::transmit(void)
{
int             loop;
unsigned char   sum;

    put("\1", 1);                                       // Send an SOH
    put(((SCITEC485MesgExchange*)first)->header,        // Send the address
        ((SCITEC485MesgExchange*)first)->headerLength);
    put(first->cmd);                                    // Send the command
    put(" ", 1);                                        // Separator for check sequence
    /* Calculate check sum */
    sum = 32;                                           // The separator
    for(loop = 0; loop < ((SCITEC485MesgExchange*)first)->headerLength; loop++)
        sum += ((SCITEC485MesgExchange*)first)->header[loop];
    for(loop = 0; first->cmd[loop]; loop++)
        sum += first->cmd[loop];
    while(sum < 130 || sum > 223)   // While 256-sum is not printable
    {
        put("^", 1);                // A check sequence character
        sum += '^';
    }
    sum = 256 - sum;                // Final character of sequence gives 0 sum
    put(&(char)sum, 1);
    put("\4", 1);                                       // Send an EOT
    mark(((SCITEC485MesgExchange*)first)->timeOut);     // Expect reply
    byteCount = 0;                  // Prepare to receive
}

void SCITEC485Port::interpret(void)
{
unsigned char   checkSum;   // Sum of characters in the message
int             loop;       // Loop variable
int             last;       // Last blank in return text

    if (byteCount >= maxReply)
    {
        throw TransmissionTooLong();
    }
    else switch (newCharacter)
    {
    case 1:     // SOH:  start of header
        byteCount = 0;
        break;
    case 4:     // EOT:  end of text
        last = byteCount - ADDRESS_LEN;
        reply[last] = '\0'; // Message is complete
        checkSum = 0;
        for (loop = 0; loop < ADDRESS_LEN; loop++)
            checkSum += addr[loop];
        for (loop = 0; loop < strlen(reply); loop++)
        {
            checkSum += reply[loop];
            if(reply[loop] == ' ')
                last = loop;        // last = offset to last blank in string
        }
        if (checkSum != 0)
            throw UndecodableResponse();
        else if(strncmp(BOSS_ADDRESS, addr, ADDRESS_LEN) == 0 ||
                strncmp(HOST_ADDRESS, addr, ADDRESS_LEN) == 0)
            status = success;
        else
            throw BadReturnAddress();
        reply[last] = '\0';
        break;
    default:
        // Stuff first (ADDRESS_LEN) bytes in the addr[] array; the
        //  rest are actual message bytes
        if(byteCount < ADDRESS_LEN)
            addr[byteCount++] = newCharacter;
        else
            reply[byteCount++ - ADDRESS_LEN] = newCharacter;
        break;
    }
}

