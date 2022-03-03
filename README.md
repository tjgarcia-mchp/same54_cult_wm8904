# Overview
This branch is a template for implementing microphone audio applications on the
SAME54 Curiosity Ultra + wm8904 codec. Audio input is formatted as 16-bit PCM
(signed Q1.15) at 16kHz.

# Firmware
The firmware can be configured for two different build types: (1) a simple level
meter and (2) a UART data streamer; this selection can be made by modifying the
`BUILD_APPLICATION_TYPE` value in `app_config.h`

### APPLICATION_TYPE_LEVEL_METER
This application just outputs the measured signal level over UART at a baud rate
of 921600.

### APPLICATION_TYPE_DATA_STREAMER
This application will stream data over UART:

* Transmits at a baud rate of 921600
* Data is sent in frames containing `AUDIO_BLOCK_NUM_SAMPLES` samples (see `app_config.h`)
* Frames are separated by a header byte (0xa5) and footer byte (0x5a)
* Frame size can be configured by modifying `AUDIO_BLOCK_SIZE_MS` in `app_config.h`.

The `readserial.py` script described below can be used to capture the audio data.

# Capturing Streaming Audio
The `readserial.py` script provides an example of how to parse out this data and
save it to a .wav file:

* Requires python 3
* Requires a few python packages: to install, run:
  > `pip install -r requirements.txt`
* Script can be run interactively via IPython or Jupyter notebook, or directly
  from the terminal using the following call format:
  > `readserial.py <com-port> <baud-rate> <burst-size> <num-samples>`

  For example:
  > `readserial.py COM7 921600 512 16000`
* Running the script will stream audio from the device and save it to a wav file
named `readserial.wav`.

# Known Issues

* Occasionally I've found that the board needs to be reset after programming for
  the program to work. Adding delay after reset seemed to fix this (*Project
  Properties -> EDBG -> Diagnostics -> Additional delay after reset*).