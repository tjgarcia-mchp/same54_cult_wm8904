This branch is a template for implementing edge impulse audio applications on
the SAME54 Curiosity Ultra.

To add your own edge impulse model:

1. Copy your compiled edge impulse library to `firmware/libedgeimpulse.a`.
   - Instructions and tools for compiling an edge impulse library for the SAME54
     can be found in the Edge Impulse [project builder
     repository](https://github.com/MicrochipTech/ml-edgeimpulse-project-builder).

2. Extract the contents of your deployed edge impulse .zip archive directly into the `firmware` folder

3. Open `firmware/src/app_config.h` and modify the `DSP_FRAME_LENGTH_MS` and
   `DSP_FRAME_STRIDE_MS` according to your application.

Note that the UART is running at a baud rate of 921600.

