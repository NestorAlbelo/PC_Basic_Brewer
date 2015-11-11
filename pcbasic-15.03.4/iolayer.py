"""
PC-BASIC 3.23 - iolayer.py
File and Device I/O operations 
 
(c) 2013, 2014 Rob Hagemans 
This file is released under the GNU GPL version 3. 
"""

import os
import copy
import logging
try:
    from cStringIO import StringIO
except ImportError:
    from StringIO import StringIO

import config
import oslayer
import error
import console
import util
import state
import memory
import backend
import serial_socket
import printer

# file numbers
state.io_state.files = {}
# fields are preserved on file close, so have a separate store
state.io_state.fields = {}

# maximum file number = maximum number of open files
# this is a command line option -f
max_files = 3
# maximum record length (-s)
max_reclen = 128

# buffer sizes (/c switch in GW-BASIC)
serial_in_size = 256
serial_out_size = 128

allowed_protocols = {
    # first protocol is default
    'LPT': ('FILE', 'PRINTER', 'PARPORT'),
    'COM': ('PORT', 'SOCKET')
    }

# GW-BASIC FILE CONTROL BLOCK structure:
# source: IBM Basic reference 1982 (for BASIC-C, BASIC-D, BASIC-A) appendix I-5
# byte  length  description
# 0     1       Mode 1-input 2-output 4-random 16-append
# 1     38      MSDOS FCB:
# ------------------------
# 0     1       Drive number (0=Default, 1=A, etc)
# 01h   8       blank-padded file name
# 09h   3       blank-padded file extension
# 0Ch   2       current block number
# 0Eh   2       logical record size
# 10h   4       file size
# 14h   2       date of last write (see #01666 at AX=5700h)
# 16h   2       time of last write (see #01665 at AX=5700h) (DOS 1.1+)
# 18h   8       reserved (see #01347,#01348,#01349,#01350,#01351)
# 20h   1       record within current block
# 21h   4       random access record number (if record size is > 64 bytes, high byte is omitted)
# ------------------------
# 39    2       For seq files, the number of sectors read or written. For random files, 1+the last record read or written
# 41    1       Number of bytes in sector when read or written
# 42    1       Number of bytes left in input buffer
# 43    3       (reserved)
# 46    1       Drive number 0=A, 1=B, 248=LPT3,2 250=COM2,1 252=CAS1 253=LPT1 254=SCRN 255=KYBD
# 47    1       Device width
# 48    1       Position in buffer for PRINT#
# 49    1       internal use during LOAD and SAVE
# 50    1       Output position used during tab expansion
# 51    128     Physical data buffer for transfer between DOS and BASIC. Can examine this data in seq I/O mode.
# 179   2       Variable length record size, default 128
# 181   2       Current physical record number
# 183   2       Current logical record number
# 185   1       (reserved)
# 186   2       Position for PRINT#, INPUT#, WRITE#
# 188   n       FIELD buffer, default length 128 (given by /s:n)

nullstream = open(os.devnull, 'w')

def prepare():
    """ Initialise iolayer module. """
    global max_files, max_reclen, serial_in_size
    global print_trigger
    if config.options['max-files'] != None:
        max_files = min(16, config.options['max-files'])
    if config.options['max-reclen'] != None:
        max_reclen = config.options['max-reclen']
        max_reclen = max(1, min(32767, max_reclen))
    if config.options['serial-buffer-size'] != None:
        serial_in_size = config.options['serial-buffer-size']
    print_trigger = config.options['print-trigger']
    # always defined
    backend.devices['SCRN:'] = SCRNFile()
    backend.devices['KYBD:'] = KYBDFile()
    backend.devices['LPT1:'] = create_device('LPT1:', config.options['lpt1'], nullstream) 
    # optional
    backend.devices['LPT2:'] = create_device('LPT2:', config.options['lpt2'])
    backend.devices['LPT3:'] = create_device('LPT3:', config.options['lpt3'])
    backend.devices['COM1:'] = create_device('COM1:', config.options['com1'])
    backend.devices['COM2:'] = create_device('COM2:', config.options['com2'])
    
def open_file_or_device(number, name, mode='I', access='R', lock='', reclen=128, defext=''):
    """ Open a file or device specified by name. """
    if (not name) or (number < 0) or (number > max_files):
        # bad file number; also for name='', for some reason
        raise error.RunError(52)
    if number in state.io_state.files:
        # file already open
        raise error.RunError(55)
    name, mode = str(name), mode.upper()
    inst = None
    split_colon = name.upper().split(':')
    if len(split_colon) > 1: # : found
        dev_name = split_colon[0] + ':'
        dev_param = split_colon[1] 
        try:
            inst = device_open(dev_name, number, mode, access, lock, reclen, dev_param)
        except KeyError:    
            if len(dev_name) > 2:
                # devname could be A:, B:, C:, etc.. but anything longer is an error (bad file number, for some reason).
                raise error.RunError(52)   
    if not inst:
        # translate the file name to something DOS-ish if necessary
        if mode in ('O', 'A'):
            # don't open output or append files more than once
            check_file_not_open(name)
        if mode in ('I', 'L'):
            name = oslayer.native_path(name, defext)
        else:    
            # random files: try to open matching file
            # if it doesn't exist, create an all-caps 8.3 file name
            name = oslayer.native_path(name, defext, make_new=True)
        # obtain a lock
        request_lock(name, lock, access)
        # open the file
        fhandle = oslayer.open_file(name, mode, access)
        # apply the BASIC file wrapper
        if mode in ('S', 'L'): # save, load
            inst = BaseFile(fhandle, name, number, mode, access, lock)
        elif mode in ('I', 'O', 'A'):
            inst = TextFile(fhandle, name, number, mode, access, lock)
        else:
            inst = RandomFile(fhandle, name, number, mode, access, lock, reclen)
    return inst    
    
def get_file(num, mode='IOAR'):
    """ Get the file object for a file number and check allowed mode. """
    try:
        the_file = state.io_state.files[num]
    except KeyError:
        # bad file number
        raise error.RunError(52)
    if the_file.mode.upper() not in mode:
        raise error.RunError(54)    
    return the_file    
     
def check_file_not_open(path):
    """ Raise an error if the file is open. """
    for f in state.io_state.files:
        if oslayer.native_path(path) == state.io_state.files[f].name:
            raise error.RunError(55)

def find_files_by_name(name):
    """ Find all file numbers open to the given filename."""
    return [state.io_state.files[f] for f in state.io_state.files if state.io_state.files[f].name == name]
      
def close_all():
    """ Close all non-system files. """
    for f in list(state.io_state.files):
        if f > 0:
            state.io_state.files[f].close()

def lock_records(nr, start, stop):
    """ Try to lock a range of records in a file. """
    thefile = get_file(nr)
    if thefile.name in backend.devices:
        # permission denied
        raise error.RunError(70)
    lock_list = set()
    for f in find_files_by_name(thefile.name):
        lock_list |= f.lock_list
    if isinstance(thefile, TextFile):
        bstart, bstop = 0, -1
        if lock_list:
            raise error.RunError(70)    
    else:
        bstart, bstop = (start-1) * thefile.reclen, stop*thefile.reclen - 1
        for start_1, stop_1 in lock_list:
            if stop_1 == -1 or (bstart >= start_1 and bstart <= stop_1) or (bstop >= start_1 and bstop <= stop_1):
                raise error.RunError(70)
    thefile.lock_list.add((bstart, bstop))

def unlock_records(nr, start, stop):    
    """ Unlock a range of records in a file. """
    thefile = get_file(nr)
    if thefile.name in backend.devices:
        # permission denied
        raise error.RunError(70)
    if isinstance(thefile, TextFile):
        bstart, bstop = 0, -1
    else:
        bstart, bstop = (start-1) * thefile.reclen, stop*thefile.reclen - 1
    # permission denied if the exact record range wasn't given before
    try:
        thefile.lock_list.remove((bstart, bstop))
    except KeyError:
        raise error.RunError(70)
    
def request_lock(name, lock, access):
    """ Try to lock a file. """
    same_files = find_files_by_name(name)
    if not lock: # default mode; don't accept default mode if SHARED/LOCK present
        for f in same_files:
            if f.lock:
                raise error.RunError(70)    
    elif lock == 'RW':  # LOCK READ WRITE
        raise error.RunError(70)    
    elif lock == 'S':   # SHARED
        for f in same_files:
            if not f.lock:
                raise error.RunError(70)       
    else:               # LOCK READ or LOCK WRITE
        for f in same_files:
            if f.access == lock or lock == 'RW':
                raise error.RunError(70)


#################################################################################


class BaseFile(object):
    """ Base file object. """
    
    def __init__(self, fhandle, name='', number=0, mode='A', access='RW', lock=''):
        """ Setup the basic properties of the file. """
        # width=255 means line wrap
        self.fhandle = fhandle
        self.name = name
        self.number = number
        self.mode = mode.upper()
        self.access = access
        self.lock = lock
        self.lock_list = set()
        self.last_read = ''  
        if number != 0:
            state.io_state.files[number] = self
    
    # set_width
    # width
    # col
    # lof
    # loc
    # eof

    #D
    def _keep_last(self, read_str):
        """ Remember last char read. """
        self.last_read = read_str[-1:]
        return read_str

    def close(self):
        """ Close the file. """
        try:
            self.fhandle.flush()
        except (IOError, ValueError):
            # ignore errors on flushing
            pass    
        # don't close the handle - for devices
        if self.number != 0:
            del state.io_state.files[self.number]
    
    def read_chars(self, num=-1):
        """ Read num chars as a list. If num==-1, read all available. """
        return list(self._keep_last(self.fhandle.read(num))) 
        
    def read(self, num=-1):
        """ Read num chars as a string. If num==-1, read all available. """
        return self._keep_last(self.fhandle.read(num))
    
    def read_line(self):
        """ Read a single line. """
        out = bytearray('')
        while len(out) < 255:
            c = self.read(1)
            if c == '\r':
                break
            out += c
        self.last_read = c
        return out            
            
    def tell(self):
        """ Get position of file pointer. """
        return self.fhandle.tell()
        
    def seek(self, num, from_where=0):
        """ Move file pointer. """
        self.fhandle.seek(num, from_where)
    
    def write(self, s):
        """ Write string or bytearray to file. """
        self.fhandle.write(str(s))
    
    def write_line(self, s=''):
        """ Write string or bytearray and newline to file. """ 
        self.write(str(s) + '\r\n')    

    def end_of_file(self):
        """ Return whether the file pointer is at the end of file. """
        s = self.fhandle.read(1)
        self.fhandle.seek(-len(s), 1)
        return s == ''
        
    def flush(self):
        """ Write contents of buffers to file. """
        self.fhandle.flush()

    def truncate(self):
        """ Delete file from pointer position onwards. """
        self.fhandle.truncate()


class TextFile(BaseFile):
    """ Text file object. """
    
    def __init__(self, fhandle, name='', number=0, mode='A', access='RW', lock=''):
        """ Initialise text file object. """
        BaseFile.__init__(self, fhandle, name, number, mode, access, lock)
        if self.mode in ('I', 'O', 'R', 'S', 'L'):
            self.fhandle.seek(0)
        else:
            self.fhandle.seek(0, 2)
        # width=255 means unlimited
        self.width = 255
        self.col = 1
    
    def close(self):
        """ Close text file. """
        if self.mode in ('O', 'A', 'S'):
            # write EOF char
            self.fhandle.write('\x1a')
        BaseFile.close(self)
        self.fhandle.close()
        
    # read line    
    def read_line(self):
        """ Read line from text file. """
        if self.end_of_file():
            # input past end
            raise error.RunError(62)
        # readline breaks line on LF, we can only break on CR or CRLF
        s = ''
        while len(s) < 255:
            c = self.fhandle.read(1)
            if c in ('', '\x1a'):
                break
            elif c == '\n':
                s += c
                # special: allow LFCR (!) to pass
                d = self.fhandle.read(1)
                if d != '\r':
                    self.fhandle.seek(-len(c), 1)
                else:
                    c = d
                    s += '\r'
            elif c == '\r':
                # check for CR/LF
                d = self.fhandle.read(1)
                if d != '\n':
                    self.fhandle.seek(-len(c), 1)
                else:
                    c = d
                break
            else:        
                s += c 
        self.last_read = c   
        return s

    def read_chars(self, num):
        """ Read num characters as list. """
        return list(self.read(num))

    def read(self, num=-1):
        """ Read num characters as string. """
        s = ''
        l = 0
        while True:
            if num > -1 and l >= num:
                break
            l += 1
            c = self.fhandle.read(1)
            # check for \x1A (EOF char - this will actually stop further reading (that's true in files but not devices)
            if c in ('\x1a', ''):
                if num == -1:
                    break
                else:
                    # input past end
                    raise error.RunError(62)
            s += c
        self.last_read = c
        return s 
        
    def write(self, s):
        """ Write the string s to the file, taking care of width settings. """
        # only break lines at the start of a new string. width 255 means unlimited width
        s_width = 0
        newline = False
        # find width of first line in s
        for c in str(s):
            if c in ('\r', '\n'):
                newline = True
                break
            if ord(c) >= 32:
                # nonprinting characters including tabs are not counted for WIDTH
                s_width += 1
        if self.width != 255 and self.col != 1 and self.col-1 + s_width > self.width and not newline:
            self.fhandle.write('\r\n')
            self.flush()
            self.col = 1
        for c in str(s):
            # don't replace CR or LF with CRLF when writing to files
            if c in ('\n', '\r'): 
                self.fhandle.write(c)
                self.flush()
                self.col = 1
            else:    
                self.fhandle.write(c)
                # nonprinting characters including tabs are not counted for WIDTH
                if ord(c) >= 32:
                    self.col += 1

    def set_width(self, new_width=255):
        """ Set the line width of the file. """
        self.width = new_width
    
    def end_of_file(self):
        """ Check for end of file - for internal use. """
        return (util.peek(self.fhandle) in ('', '\x1a'))
    
    def eof(self):
        """ Check for end of file EOF. """
        # for EOF(i)
        if self.mode in ('A', 'O'):
            return False
        return (util.peek(self.fhandle) in ('', '\x1a'))
    
    def loc(self):
        """ Get file pointer LOC """
        # for LOC(i)
        if self.mode == 'I':
            return max(1, (127+self.fhandle.tell())/128)
        return self.fhandle.tell()/128

    def lof(self):
        """ Get length of file LOF. """
        current = self.fhandle.tell()
        self.fhandle.seek(0, 2)
        lof = self.fhandle.tell()
        self.fhandle.seek(current)
        return lof


class RandomBase(BaseFile):
    """ Random-access file base object. """
    
    def __init__(self, fhandle, name, number, mode, access, lock, reclen=128):
        """ Initialise random-access file. """
        BaseFile.__init__(self, fhandle, name, number, mode, access, lock)
        self.reclen = reclen
        # replace with empty field if already exists    
        try:
            self.field = state.io_state.fields[self.number]
        except KeyError:
            self.field = bytearray()
            state.io_state.fields[self.number] = self.field
        if self.number > 0:    
            self.field_address = memory.field_mem_start + (self.number-1)*memory.field_mem_offset
        else:
            self.field_address = -1    
        self.field[:] = bytearray('\x00')*reclen
        # open a pseudo text file over the buffer stream
        # to make WRITE# etc possible
        # all text-file operations on a RANDOM file number actually work on the FIELD buffer
        self.field_text_file = TextFile(ByteStream(self.field))
        self.field_text_file.col = 1
        # width=255 means line wrap
        self.field_text_file.width = 255
    
    def read_line(self):
        """ Read line from FIELD buffer. """
        # FIELD overflow happens if last byte in record is actually read
        if self.field_text_file.fhandle.tell() >= self.reclen-1:
            raise error.RunError(self.overflow_error) # FIELD overflow
        return self.field_text_file.read_line()

    def read_chars(self, num=-1):
        """ Read num characters as list. """
        return list(self.read(num))
        
    def read(self, num=-1):
        """ Read num chars as a string, from FIELD buffer. """
        if num==-1 or self.field_text_file.fhandle.tell() + num > self.reclen-1:
            raise error.RunError(self.overflow_error) # FIELD overflow
        return self._keep_last(self.field_text_file.read(num))
    
    def write(self, s):
        """ Write one or more chars to FIELD buffer. """
        ins = StringIO(s)
        while self.field_text_file.fhandle.tell() < self.reclen:
            self.field_text_file.write(ins.read(1))
        if ins.tell() < len(s):
            raise error.RunError(self.overflow_error) 
    
    def seek(self, n, from_where=0):
        """ Get file pointer location in FIELD buffer. """
        return self.field_text_file.seek(n, from_where)
        
    def truncate(self):
        """ Not implemented. """
        # this is only used when writing chr$(8)
        # not sure how to implement for random files
        pass
        
    @property
    def col(self):
        """ Get current column. """
        return self.field_text_file.col
    
    @property
    def width(self):
        """ Get file width. """
        return self.field_text_file.width
    
    def set_width(self, new_width=255):
        """ Set file width. """
        self.field_text_file.width = new_width

        
class RandomFile(RandomBase):
    """ Random-access file object. """
    
    # FIELD overflow
    overflow_error = 50

    def __init__(self, fhandle, name, number, mode, access, lock, reclen=128):
        """ Initialise random-access file. """        
        RandomBase.__init__(self, fhandle, name, number, mode, access, lock, reclen)
        # position at start of file
        self.recpos = 0
        self.fhandle.seek(0)
    
    def close(self):
        """ Close random-access file. """
        RandomBase.close(self)
        self.fhandle.close()
        
    def read_field(self, dummy=None):
        """ Read a record. """
        if self.eof():
            self.field[:] = '\x00'*self.reclen
        else:
            self.field[:] = self.fhandle.read(self.reclen)
        self.field_text_file.seek(0)    
        self.recpos += 1
        
    def write_field(self, dummy=None):
        """ Write a record. """
        current_length = self.lof()
        if self.recpos > current_length:
            self.fhandle.seek(0, 2)
            numrecs = self.recpos-current_length
            self.fhandle.write('\x00'*numrecs*self.reclen)
        self.fhandle.write(self.field)
        self.recpos += 1
        
    def set_pos(self, newpos):
        """ Set current record number. """
        # first record is newpos number 1
        self.fhandle.seek((newpos-1)*self.reclen)
        self.recpos = newpos - 1

    def loc(self):
        """ Get number of record just past, for LOC. """
        return self.recpos
        
    def eof(self):
        """ Return whether we're past currentg end-of-file, for EOF. """
        return self.recpos*self.reclen > self.lof()
            
    def lof(self):
        """ Get length of file, in bytes, for LOF. """
        current = self.fhandle.tell()
        self.fhandle.seek(0, 2)
        lof = self.fhandle.tell()
        self.fhandle.seek(current)
        return lof

#################################################################################

class ByteStream(object):
    """ A StringIO-like wrapper for bytearray. """

    def __init__(self, contents=''):       
        """ Create e new ByteStream. """
        self.setvalue(contents)

    def setvalue(self, contents=''):
        """ Assign a bytearray s, move location to 0. This does not create a copy, changes affect the original bytearray. """
        self._contents = contents
        self._loc = 0
    
    def getvalue(self):
        """ Retrieve the bytearray. changes will affect the bytestream. """
        return self._contents
        
    def tell(self):
        """ Get the current location. """    
        return self._loc
        
    def seek(self, n_bytes, from_where=0):
        """ Move loc by n bytes from start(w=0), current(w=1) or end(w=2). """    
        if from_where == 0:
            self._loc = n_bytes
        elif from_where == 1:
            self._loc += n_bytes
        elif from_where == 2:
            self._loc = len(self._contents)-n_bytes        
        if self._loc < 0:
            self._loc = 0
        elif self._loc > len(self._contents):
            self._loc = len(self._contents)    
    
    def read(self, n_bytes=None):
        """ Get an n-length string and move the location n forward. If loc>len, return empty string. """
        if n_bytes == None:
            n_bytes = len(self._contents) - self._loc
        if self._loc >= len(self._contents):
            self._loc = len(self._contents)
            return ''
        peeked = self._contents[self._loc:self._loc+n_bytes]
        self._loc += len(peeked)   
        return peeked
            
    def write(self, substr):
        """ Write a str or bytearray or char s to the current location. Overwrite, do not insert. """    
        if self._loc >= len(self._contents):
            self._contents += substr
            self._loc = len(self._contents)    
        else:    
            self._contents[self._loc:self._loc+len(substr)] = substr
            self._loc += len(substr)

    def truncate(self, n=None):
        """ Clip off the bytearray after position n. """
        if n == None:
            n = self._loc
        self._contents = self._contents[:n]
        if self._loc >= len(self._contents):
            self._loc = len(self._contents)

    def close(self):
        """ Close the stream. """
        pass            

######################################################
# Device files
#
#  Each device has a 'device object' with file number 0, which can be 
#  cloned into several 'device files' with a nonzero file number.
#  For example, WIDTH "SCRN:, 40 works directly on the console,
#  whereas OPEN "SCRN:" FOR OUTPUT AS 1: WIDTH #1,23 works on the wrapper file.
#  Likewise, WIDTH "LPT1:" works on lpt1 for the next time it's opened.

def create_device(name, arg, default=None):
    """ Attach a device name to a new device object. """
    if not arg:
        stream = default
    else:   
        stream = create_device_stream(arg, allowed_protocols[name[:3]])
        if not stream:
            logging.warning('Could not attach %s to %s.' % (name, arg))
            stream = default
    if stream:        
        if name[:3] == 'COM':
            return COMFile(stream, name)
        else:
            return LPTFile(stream, name, flush_trigger=print_trigger)    
    else:
        return None        

def create_device_stream(arg, allowed):
    """ Create a device stream based on protocal string. """
    argsplit = arg.split(':', 1)
    if len(argsplit) == 1:
        # use first allowed protocol as default
        addr, val = allowed[0], argsplit[0]
    elif len(argsplit) == 2:
        addr, val = argsplit[0].upper(), argsplit[1]
    else:
        return None
    if addr not in allowed:
        return None
    if addr == 'PRINTER':
        stream = printer.PrinterStream(val)
    elif addr == 'FILE':
        try:
            if not os.path.exists(val):
                open(val, 'wb').close() 
            stream = open(val, 'r+b')
        except (IOError, OSError) as e:
            return None    
    elif addr == 'PARPORT':
        # port can be e.g. /dev/parport0 on Linux or LPT1 on Windows. Just a number counting from 0 would also work.
        stream = serial_socket.parallel_port(val)
    elif addr == 'PORT':
        # port can be e.g. /dev/ttyS1 on Linux or COM1 on Windows. Or anything supported by serial_for_url (RFC 2217 etc)
        stream = serial_socket.serial_for_url(val)
    elif addr == 'SOCKET':
        stream = serial_socket.serial_for_url('socket://'+val)
    else:
        # File not found
        raise error.RunError(53)
    return stream
            
def device_open(device_name, number, mode, access, lock, reclen, param=''):
    """ Open a file on a device. """
    # check if device exists and allows the requested mode    
    # if not exists, raise KeyError to caller
    device = backend.devices[str(device_name).upper()]
    if not device:    
        # device unavailable
        raise error.RunError(68)      
    if mode not in device.allowed_modes:
        # bad file mode
        raise error.RunError(54)
    # don't lock devices
    return device.open(number, mode, access, '', reclen, param)

def close_devices():
    """ Close all devices. """
    for d in backend.devices:
        if backend.devices[d]:
            backend.devices[d].close()


############################################################################

def clone_device(dev, number, mode, access, lock='', reclen=128):
    """ Clone device object as device file object (helper method). """
    inst = copy.copy(dev)
    inst.number = number
    inst.access = access
    inst.mode = mode
    inst.lock = lock
    inst.reclen = reclen
    if number != 0:
        state.io_state.files[number] = inst
    return inst


class NullDevice(object):
    """ Base object for devices and device files. """
    
    def __init__(self):
        """ Initialse device object. """
        self.number = 0
        self.last_read = ''

    def open(self, number, mode, access, lock, reclen, param=''):
        """ Open a file on this device. """
        if number != 0:
            state.io_state.files[number] = self
        return clone_device(self, number, mode, access, lock, reclen)
    
    def close(self):
        """ Close this device file. """
        if self.number != 0:
            del state.io_state.files[self.number]
    
    def lof(self):
        """ LOF: bad file mode. """
        raise error.RunError(54)
        
    def loc(self):
        """ LOC: bad file mode. """
        raise error.RunError(54)
        
    def eof(self):
        """ EOF: bad file mode. """
        raise error.RunError(54)
           
    def write(self, s):
        """ Write string s to device. """
        pass

    def write_line(self, s):
        """ Write string s and CR/LF to device """
        pass

    def set_width(self, new_width=255):
        """ Set device width. """
        pass
    
    def read_line(self):
        """ Read a line from device. """
        return ''    

    def read_chars(self, n):
        """ Read a list of chars from device. """
        return []

    def read(self, n):
        """ Read a string from device. """
        return ''        

    def end_of_file(self):
        """ Check for end-of-file. """
        return False    

    def set_parameters(self, param):
        """ Set device parameters. """
        pass

    #D
    def _keep_last(self, read_str):
        """ Remember last char read. """
        self.last_read = read_str[-1:]
        return read_str
        
        
class KYBDFile(NullDevice):
    """ KYBD device: keyboard. """

    input_replace = { 
        '\x00\x47': '\xFF\x0B', '\x00\x48': '\xFF\x1E', '\x00\x49': '\xFE', 
        '\x00\x4B': '\xFF\x1D', '\x00\x4D': '\xFF\x1C', '\x00\x4F': '\xFF\x0E',
        '\x00\x50': '\xFF\x1F', '\x00\x51': '\xFE', '\x00\x53': '\xFF\x7F', '\x00\x52': '\xFF\x12'
        }

    allowed_modes = 'IR'
    col = 0
    
    def __init__(self):
        """ Initialise keyboard device. """
        self.name = 'KYBD:'
        self.mode = 'I'
        self.width = 255
        NullDevice.__init__(self)
        
    def read_line(self):
        """ Read a line from the keyboard. """
        s = bytearray('')
        while len(s) < 255:
            c = self.read(1)
            if c == '\r':
                # don't check for CR/LF when reading KYBD:
                break
            else:
                s += c
        self.last_read = c
        return s

    def read_chars(self, num=1):
        """ Read a list of chars from the keyboard - INPUT$ """
        return self._keep_last(state.console_state.keyb.read_chars(num))

    def read(self, n=1):
        """ Read a string from the keyboard - INPUT and LINE INPUT. """
        word = ''
        for c in state.console_state.keyb.read_chars(n):
            if len(c) > 1 and c[0] == '\x00':
                try:
                    word += self.input_replace[c]
                except KeyError:
                    pass
            else:
                word += c        
        return word
        
    def lof(self):
        """ LOF for KYBD: is 1. """
        return 1

    def loc(self):
        """ LOC for KYBD: is 0. """
        return 0
     
    def eof(self):
        """ KYBD only EOF if ^Z is read. """
        if self.mode in ('A', 'O'):
            return False
        # blocking peek
        return (state.console_state.keyb.wait_char() == '\x1a')

    def set_width(self, new_width=255):
        """ Setting width on KYBD device (not files) changes screen width. """
        if self.number == 0:
            console.set_width(new_width)


class SCRNFile(NullDevice):
    """ SCRN: device, allows writing to the screen as a text file. 
        SCRN: device *files* work as a wrapper text file. """

    allowed_modes = 'OR'
    
    def __init__(self):
        """ Initialise screen device. """
        self.name = 'SCRN:'
        self.mode = 'O'
        self._width = state.console_state.screen.mode.width
        self._col = state.console_state.col
        NullDevice.__init__(self)
    
    def write(self, s):
        """ Write string s to SCRN: """
        # writes to SCRN files should *not* be echoed 
        do_echo = (self.number == 0)
        self._col = state.console_state.col
        # take column 80+overflow int account
        if state.console_state.overflow:
            self._col += 1
        # only break lines at the start of a new string. width 255 means unlimited width
        s_width = 0
        newline = False
        # find width of first line in s
        for c in str(s):
            if c in ('\r', '\n'):
                newline = True
                break
            if c == '\b':
                # for lpt1 and files, nonprinting chars are not counted in LPOS; but chr$(8) will take a byte out of the buffer
                s_width -= 1
            elif ord(c) >= 32:
                # nonprinting characters including tabs are not counted for WIDTH
                s_width += 1
        if (self.width != 255 
                and self.col != 1 and self.col-1 + s_width > self.width and not newline):
            console.write_line(do_echo=do_echo)
            self._col = 1
        cwidth = state.console_state.screen.mode.width
        for c in str(s):
            if self.width <= cwidth and self.col > self.width:
                console.write_line(do_echo=do_echo)
                self._col = 1
            if self.col <= cwidth or self.width <= cwidth:
                console.write(c, do_echo=do_echo)
            if c in ('\n', '\r'):
                self._col = 1
            else:
                self._col += 1
                
                
            
    def write_line(self, inp=''):
        """ Write a string to the screen and follow by CR. """
        self.write(inp)
        console.write_line(do_echo=(self.number==0))
            
    @property
    def col(self):  
        """ Return current (virtual) column position. """
        if self.number == 0:    
            return state.console_state.col
        else:
            return self._col
        
    @property
    def width(self):
        """ Return (virtual) screen width. """
        if self.number == 0:    
            return state.console_state.screen.mode.width
        else:
            return self._width
    
    def set_width(self, new_width=255):
        """ Set (virtual) screen width. """
        if self.number == 0:
            console.set_width(new_width)
        else:    
            self._width = new_width


class LPTFile(BaseFile):
    """ LPTn: device - line printer or parallel port. """
    
    allowed_modes = 'OR'
    
    def __init__(self, stream, name, flush_trigger='close'):
        """ Initialise LPTn. """
        # width=255 means line wrap
        self.width = 255
        self.col = 1
        self.output_stream = stream
        BaseFile.__init__(self, StringIO(), name)
        self.flush_trigger = flush_trigger

    def open(self, number, mode, access, lock, reclen, param=''):
        """ Open a file on LPTn. """
        f = clone_device(self, number, mode, access, lock, reclen)
        # don't trigger flushes on LPT files, just on the device directly
        f.flush_trigger = 'close'
        return f

    def flush(self):
        """ Flush the printer buffer to the underlying stream. """
        try:
            self.output_stream.write(self.fhandle.getvalue())
            self.fhandle.truncate(0)
        except ValueError:
            # already closed, ignore
            pass
        
    def set_width(self, new_width=255):
        """ Set the width for LPTn. """
        self.width = new_width

    def write(self, s):
        """ Write a string to the printer buffer. """
        for c in str(s):
            if self.col >= self.width and self.width != 255:  # width 255 means wrapping enabled
                self.fhandle.write('\r\n')
                self.flush()
                self.col = 1
            if c in ('\n', '\r', '\f'): 
                # don't replace CR or LF with CRLF when writing to files
                self.fhandle.write(c)
                self.flush()
                self.col = 1
                # do the actual printing if we're on a short trigger
                if (self.flush_trigger == 'line' and c == '\n') or (self.flush_trigger == 'page' and c == '\f'):
                    self.output_stream.flush()
            elif c == '\b':   # BACKSPACE
                if self.col > 1:
                    self.col -= 1
                    self.seek(-1, 1)
                    self.truncate()  
            else:    
                self.fhandle.write(c)
                # nonprinting characters including tabs are not counted for WIDTH
                # for lpt1 and files , nonprinting chars are not counted in LPOS; but chr$(8) will take a byte out of the buffer
                if ord(c) >= 32:
                    self.col += 1
        
    def lof(self):
        """ LOF: bad file mode """
        raise error.RunError(54)

    def loc(self):
        """ LOC: bad file mode """
        raise error.RunError(54)

    def eof(self):
        """ EOF: bad file mode """
        raise error.RunError(54)
    
    def close(self):
        """ Close the printer device and actually print the output. """
        self.flush()
        try:
            self.output_stream.flush()
        except ValueError:
            # already closed, ignore
            pass    
        BaseFile.close(self)
        
        
class COMFile(RandomBase):
    """ COMn: device - serial port. """
    
    allowed_modes = 'IOAR'
    
    # communications buffer overflow
    overflow_error = 69

    def __init__(self, stream, name):
        """ Initialise COMn: device """
        self._in_buffer = bytearray()
        RandomBase.__init__(self, stream, name, 0, 'R', 'RW', '', serial_in_size)

    def open(self, number, mode, access, lock, reclen, param=''):
        """ Open a file on COMn """
        # open the COM port
        if self.fhandle._isOpen:
            # file already open
            raise error.RunError(55)
        else:
            try:
                self.fhandle.open()
            except serial_socket.SerialException:
                # device timeout
                raise error.RunError(24)
        self.linefeed = False
        try:
            self.set_parameters(param)
        except Exception:
            self.close()
            raise
        return clone_device(self, number, mode, access, lock, reclen)   
    
    def check_read(self):
        """ Fill buffer at most up to buffer size; non blocking. """
        try:
            self._in_buffer += self.fhandle.read(serial_in_size - len(self._in_buffer))
        except (serial_socket.SerialException, ValueError):
            # device I/O
            raise error.RunError(57)
        
    def read(self, num=1):
        """ Read num characters from the port as a string; blocking """
        out = ''
        while len(out) < num:
            # non blocking read
            self.check_read()
            to_read = min(len(self._in_buffer), num - len(out))
            out += str(self._in_buffer[:to_read])
            del self._in_buffer[:to_read]
            # allow for break & screen updates
            backend.wait()
        self.last_read = out[-1:]
        return out
        
    def read_chars(self, num=1):
        """ Read num characters from the port as a list; blocking """
        return list(self.read(num))
    
    def read_line(self):
        """ Blocking read line from the port (not the FIELD buffer!). """
        out = bytearray('')
        while len(out) < 255:
            c = self.read(1)
            if c == '\r':
                if self.linefeed:
                    c = self.read(1)
                    if c == '\n':
                        break
                    out += ''.join(c)
                else:
                    break
            out += ''.join(c)
        self.last_read = out[-1]
        return out
    
    def char_waiting(self):
        """ Whether a char is present in buffer. For ON COM(n). """
        return self._in_buffer != ''

    def write_line(self, s=''):
        """ Write string or bytearray and newline to port. """ 
        self.write(str(s) + '\r')    

    def write(self, s):
        """ Write string to port. """
        try:
            if self.linefeed:
                s = s.replace('\r', '\r\n')    
            self.fhandle.write(s)
        except (serial_socket.SerialException, ValueError):
            # device I/O
            raise error.RunError(57)
    
    def read_field(self, num):
        """ Read a record - GET. """
        # blocking read of num bytes
        self.field[:] = self.read(num)
        
    def write_field(self, num):
        """ Write a record - PUT. """
        self.write(self.field[:num])
        
    def loc(self):
        """ LOC: Returns number of chars waiting to be read. """ 
        # don't use inWaiting() as SocketSerial.inWaiting() returns dummy 0    
        # fill up buffer insofar possible
        self.check_read()
        return len(self._in_buffer) 
            
    def eof(self):
        """ EOF: no chars waiting. """
        # for EOF(i)
        return self.loc() <= 0
        
    def lof(self):
        """ Returns number of bytes free in buffer. """
        return serial_in_size - self.loc()
    
    def close(self):
        """ Close the COMn device. """
        self.fhandle.close()
        RandomBase.close(self)

    def set_parameters(self, param):
        """ Set serial port connection parameters """
        max_param = 10
        param_list = param.upper().split(',')
        if len(param_list) > max_param:
            # Bad file name
            raise error.RunError(64)
        param_list += ['']*(max_param-len(param_list))
        speed, parity, data, stop, RS, CS, DS, CD, LF, PE = param_list
        # set speed
        if speed not in ('75', '110', '150', '300', '600', '1200', 
                          '1800', '2400', '4800', '9600', ''):
            # Bad file name
            raise error.RunError(64)
        speed = int(speed) if speed else 300
        self.fhandle.baudrate = speed
        # set parity
        if parity not in ('S', 'M', 'O', 'E', 'N', ''):
            raise error.RunError(64)
        parity = parity or 'E'
        self.fhandle.parity = parity
        # set data bits
        if data not in ('4', '5', '6', '7', '8', ''):
            raise error.RunError(64)
        data = int(data) if data else 7
        bytesize = data + (parity != 'N')
        if bytesize not in range(5, 9):
            raise error.RunError(64)
        self.fhandle.bytesize = bytesize
        # set stopbits
        if stop not in ('1', '2', ''):
            raise error.RunError(64)
        if not stop:
            stop = 2 if (speed in (75, 110)) else 1
        else:
            stop = int(stop)
        self.fhandle.stopbits = stop
        if (RS not in ('RS', '') or CS[:2] not in ('CS', '')
            or DS[:2] not in ('DS', '') or CD[:2] not in ('CD', '')
            or LF not in ('LF', '') or PE not in ('PE', '')):
            raise error.RunError(64)
        # set LF
        self.linefeed = (LF != '')

prepare()

