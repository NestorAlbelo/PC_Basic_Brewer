"""
PC-BASIC 3.23 - serial_socket.py
1) Workaround for some limitations of SocketSerial with timeout==0
2) Stream API for parallel port

Contains code from PySerial (c) 2001-2013 Chris Liechtl <cliechti(at)gmx.net>; All Rights Reserved.
as well as modifications (c) 2013-2014 Rob Hagemans.
This file is released under the GNU GPL version 3 or, at your option, the Python 2.7 licence.
"""

import logging

try:
    import serial
    from serial import SerialException, serialutil
except Exception:
    serial = None
    # serial sometimes sends ValueError, so caller needs to catch this anyway
    SerialException = ValueError
    
try:
    import parallel    
except Exception:
    parallel = None
        
import socket
import select

def parallel_port(port):
    """ Return a ParallelStream object for a given port. """
    if not parallel:
        logging.warning('Parallel module not found. Parallel port communication not available.')
        return None
    try:
        return ParallelStream(port)
    except (OSError, IOError):
        logging.warning('Could not open parallel port %s.', port) 
        return None

def serial_for_url(url):
    """ Return a Serial object for a given url. """
    if not serial:
        logging.warning('Serial module not found. Serial port and socket communication not available.')
        return None
    try:    
        stream = serial.serial_for_url(url, timeout=0, do_not_open=True)
    except ValueError as e:
        return None
    if url.split(':', 1)[0] == 'socket':
        return SocketSerialWrapper(stream)
    else:   
        return stream


class SocketSerialWrapper(object):
    """ Wrapper object for SocketSerial to work around timeout==0 issues. """
    
    def __init__(self, socketserial):
        """ initialise the wrapper. """
        self._serial = socketserial    
        self._isOpen = self._serial._isOpen
    
    def open(self):
        """ Open the serial connection. """
        self._serial.open()
        self._isOpen = self._serial._isOpen
    
    def close(self):
        """ Close the serial connection. """
        self._serial.close()
        self._isOpen = self._serial._isOpen
    
    def flush(self):
        """ No buffer to flush. """
        pass
            
    def read(self, num=1):
        """ Non-blocking read from socket. """
        # this is the raison d'etre of the wrapper.
        # SocketSerial.read always returns '' if timeout==0
        self._serial._socket.setblocking(0)
        if not self._serial._isOpen: 
            raise serialutil.portNotOpenError
        # poll for bytes (timeout = 0)
        ready, _, _ = select.select([self._serial._socket], [], [], 0)
        if not ready:
            # no bytes present after poll
            return ''
        try:
            # fill buffer at most up to buffer size  
            return self._serial._socket.recv(num)
        except socket.timeout:
            pass
        except socket.error, e:
            raise SerialException('connection failed (%s)' % e)
    
    def write(self, s):
        """ Write to socket. """
        self._serial.write(s)                    


class ParallelStream(object):
    """ Wrapper for Parallel object to implement stream-like API. """

    def __init__(self, port):
        """ Initialise the ParallelStream. """
        self.parallel = parallel.Parallel(port)
    
    def flush(self):
        """ No buffer to flush. """
        pass
        
    def write(self, s):
        """ Write to the parallel port. """
        for c in s:
            self.parallel.setData(ord(c))
    
    def close(self):
        """ Close the stream. """
        pass
                    
