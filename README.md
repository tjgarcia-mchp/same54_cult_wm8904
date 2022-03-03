This branch is a template for implementing microphone audio applications on the
SAME54 Curiosity Ultra + wm8904 codec. Audio input is formatted as 16-bit PCM
(signed Q1.15) at 16kHz.

See `firmware/src/app_config.h` to configure buffer sizes.

By default, application will stream data over UART at a baud rate of 921600:

* Data is sent in frames containing `AUDIO_BLOCK_NUM_SAMPLES` samples
* Frames are separated by a header byte (0xa5) and footer byte (0x5a)

The `readserial.py` script provides an example of how to parse out this data and
save it to a .wav file:

* Requires python 3
* Requires a few python packages; to install, run: `pip install -r requirements.txt`
* Script can be run interactively via IPython or Jupyter notebook, or directly
  from the terminal using the following call format:
  * `readserial.py <com-port> <baud-rate> <burst-size> <num-samples>`
  * e.g.: `readserial.py COM7 921600 512 16000`
