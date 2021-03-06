/*
 * qserial.cpp
 *
 * Queued Serial Port Driver Source File
 *
 * This file implements all of the methods of the QueuedPort class
 *   and MessageExchange class.
 *
 * Copyright(C) 1993, 1995, SCI-TEC Instruments Inc.
 */

#include <fstream.h>
#include ".\qserial\qserial.hpp"

#ifndef _NO_VERSION_INFO
#   include ".\version\version.hpp"

	static Version ver("QueuedPort", 3, 3);
    // Based on first line of this file.  see Version.hpp
#endif

static ofstream *traceFile = NULL;

QueuedPort::QueuedPort(CommPort com, BaudRate baud, Parity parity,
				DataBits data, StopBits stop, HandShakeSelection sig)
		: SerialPort(com ,baud, parity, data, stop, sig),
		  first(NULL), last(NULL), tracing(off), status(success)
{
	traceFile = new ofstream("traceqp.txt", ios::binary|ios::app);
	*traceFile << "open: COM" << com << ", " << baud.getValue() << " baud" << endl;
	delete traceFile;
    traceFile = NULL;
}

QueuedPort::~QueuedPort()
{
    if(!traceFile)
    	traceFile = new ofstream("traceqp.txt", ios::binary|ios::app);
	*traceFile << "close" << endl;
	delete traceFile;
    traceFile = NULL;
}

void QueuedPort::enqueue(MessageExchange* msg)
{
	if(last == NULL)
	{
		first = msg;
		status = awaitingPort;
		reply = first->reply;
	}
	else
		last->next = msg;
	last = msg;
	msg->status = awaitingPort;
	msg->next = NULL;
}

void QueuedPort::dequeue(MessageExchange* msg)
{
MessageExchange *prev = NULL, **ptr;

	if(msg == first)
	{
		first->status = status;
		first = first->next;
		if(last == msg)
		{
			first = NULL;   // Just in case
			last = NULL;
			return;
		}
		status = first->status;
		reply = first->reply;
		return;
	}
	for(ptr = &first; msg != *ptr; ptr = &prev->next)
	{
		if(prev == NULL)
			prev = first;
		else
			prev = prev->next;
	}
	*ptr = msg->next;
	if(last == msg)
		last = prev;
	msg->next = NULL;
}

void QueuedPort::put(char* ch, int count)
{
	int cnt = send(ch, count);
	if(tracing == full)
	{
        if(!traceFile)
    		traceFile = new ofstream("traceqp.txt", ios::binary|ios::app);
		int i;
		for(i = 0; i < cnt; i++)
			*traceFile << "out: " << (int)ch[i] << ' ' << ch[i] << endl;
		for(; i < count; i++)
			*traceFile << "lost on output: " << (int)ch[i] << ' ' << ch[i] << endl;
		delete traceFile;
        traceFile = NULL;
	}
}

void QueuedPort::run(void)
{
	while(first != NULL)     // Port is not idle
	{
		if(status == awaitingPort)
		{
			// Clear the input buffer
			if(tracing == full && !traceFile)
				traceFile = new ofstream("traceqp.txt", ios::binary|ios::app);
			while(test())
			{
				newCharacter = SerialPort::getch();
				if (tracing == full)
					*traceFile << "lost on input: " << ((newCharacter) + 0) << ' ' << newCharacter << endl;
			}
			// lets go
			transmit();
			status = inProgress;
			if(tracing == text)
			{
                if(!traceFile)
		    		traceFile = new ofstream("traceqp.txt", ios::binary|ios::app);
				*traceFile << "sending: " << first->cmd << " (" << strlen(first->cmd) << " bytes)" << endl;
			}
		}
		// Partway through receiving characters
		try {
			if(status >= inProgress && status < awaitingPort)
			{
				if(!test())
				{
					if(time(0) > waitDoneTime)
					{
						throw QSerialTimeout();
					}
				}
				if(tracing == full && !traceFile)
					traceFile = new ofstream("traceqp.txt", ios::binary|ios::app);
				while(test() && status >= inProgress && status < awaitingPort)
				{
					newCharacter = SerialPort::getch();
					if(tracing == full)
						*traceFile << "in: " << ((newCharacter) + 0) << ' ' << newCharacter << endl;
					interpret();
				}
				if(status >= inProgress && status < awaitingPort)
                {
                    if(traceFile)
                        delete traceFile;
                    traceFile = NULL;
					return;     // Still more to come so wait till later
                }
			}
		}
		catch(BaseException& err)
		{
			status = error;
			if(tracing == text)
			{
                if(!traceFile)
		    		traceFile = new ofstream("traceqp.txt", ios::binary|ios::app);
				*traceFile << err.why() << ": " << reply << endl;
			}
			dequeue(first);
            if(traceFile)
    			delete traceFile;
            traceFile = NULL;
			throw;
		}
		// Message has been received and analyzed
		if(tracing == text)
		{
            if(!traceFile)
	    		traceFile = new ofstream("traceqp.txt", ios::binary|ios::app);
			*traceFile << "received(" << status << "): " << reply << " (" << strlen(reply) << " bytes)" << endl;
		}
		dequeue(first);
        if(traceFile)
			delete traceFile;
        traceFile = NULL;
		return;
	}
}

MessageExchange::MessageExchange(QueuedPort* p, char* cmdSpace, char* replySpace)
	: next(NULL), dest(p), cmd(cmdSpace), reply(replySpace),
	  status(QueuedPort::success)
{
	cmd[0] = reply[0] = 0;
}

void MessageExchange::exchange()
{
	send();
	while(!done())
		;
}

QSerialTimeout::QSerialTimeout(void)
	: BaseException(helpdescribe("QSerial timeout"))
{
}

