# -*- coding: utf-8 -*-
"""
Created on Tue Feb  9 17:11:15 2021

@author: toemo
"""
#%%
import sys
import serial
import numpy as np
import soundfile as sf

__usage__ = 'readserial.py <com-port> <baud-rate> <burst-size> <num-samples>'

#%%
labels = ['x']

NAXES = len(labels)

fs = 16000

# scalar data type
dtype = np.int16

# separator between burst transfers
headerbyte = 0xa5

if sys.stdin and sys.stdin.isatty():
    if len(sys.argv) < 5:
        print("usage: " + __usage__, file=sys.stderr)
        sys.exit(-1)
    comport = sys.argv[1]
    baudrate = int(sys.argv[2])
    burstsize = int(sys.argv[3])
    nrows = int(sys.argv[4])
else:
    comport = 'COM7'
    baudrate= 921600
    burstsize = 512
    nrows = fs*5

# How many bytes per item
itemsize = np.dtype(dtype).itemsize

# How many bytes per frame (2 bytes added for header)
framesize = NAXES * itemsize * burstsize + 2

#%%
def sample(com, nrows=None):
    header = bytes([headerbyte])
    footer = bytes([256 + ~headerbyte])
    frameseparator = footer + header

    if nrows is None:
        nrows = burstsize

    nframes = (nrows + burstsize - 1) // burstsize

    # Clear data before proceeding
    com.reset_input_buffer()

    # Capture nframes*framesize bytes, plus some extra for alignment
    nbytes = framesize*(nframes + 1)
    bytestr = bytearray(nbytes)
    memview = memoryview(bytestr) # memory views avoid making copies
    while nbytes > 0:
        nbytes -= com.readinto(memview[-nbytes:])

    # return bytestr
    # Find the start of the first complete frame
    idx = 0
    while True:
        idx = bytestr.find(frameseparator, idx)
        if idx == -1:
            raise Exception("Couldn't parse data; check that framesize settings and UART baud rate are set correctly")

        # make sure the header bytes are in the right place
        if bytestr[idx+framesize:idx+framesize+2] == frameseparator:
            idx = idx+1
            break

        idx = idx+2

    # Cast as byte array and split into frames
    data = np.frombuffer(memview[idx:idx+framesize*nframes], count=framesize*nframes, dtype=np.uint8).reshape((nframes, framesize))

    # Drop off the header bytes and interpret the result as the expected data type
    data = data[:, 1:-1].copy().view(dtype).reshape((-1, NAXES))[:nrows]

    return data

#%%
com_params = dict(port=comport, timeout=5, baudrate=baudrate,
                    parity=serial.PARITY_NONE,
                    stopbits=serial.STOPBITS_ONE,
                    bytesize=serial.EIGHTBITS)

with serial.Serial(**com_params) as com:
    data = sample(com, nrows)


# %%
sf.write('readserial.wav', data, fs, 'PCM_16')
