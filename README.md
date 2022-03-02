This branch is a template for implementing microphone audio applications on the
SAME54 Curiosity Ultra + wm8904 codec. Audio input is formatted as 16-bit PCM
(signed Q1.15).

See `firmware/src/app_config.h` to configure buffer sizes.

By default, application will stream data over UART at a baud rate of 921600.

