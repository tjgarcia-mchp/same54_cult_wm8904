This branch is a template for implementing sensiml audio applications on the SAME54 Curiosity Ultra.

To add your own sensiml model:

* Copy your compiled sensiml library to `firmware/libsensiml.a`.
  * Instructions and tools for compiling a sensiml library for the SAME54 can be found in the [SensiML project builder repository](https://github.com/tjgarcia-mchp/ml-sensiml-project-builder).
* Add knowledgepack header files from your deployed sensiml model to the `firmware/knowledgepack/inc/` folder.
* Open `firmware/src/main.c` and implement the `run_model()` function.

Note that the UART is running at a baud rate of 921600.
