/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdio.h>
#include <math.h>
#include "definitions.h"                // SYS function prototypes
#include "ringbuffer.h"
#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Globals
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Helpers and handlers
// *****************************************************************************
// *****************************************************************************

size_t __attribute__(( unused )) UART_Write(uint8_t *ptr, const size_t nbytes) {
    return SERCOM1_USART_Write(ptr, nbytes) ? nbytes : 0;
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main ( void )
{
#if BUILD_APPLICATION_TYPE == APPLICATION_TYPE_LEVEL_METER    
    float dbmeter = 0;
    float alpha = 0.96; // recursive smoothing constant
#endif
    
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    while ( appData.state != APP_STATE_FATAL )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        // Retrieve mic buffer
        ringbuffer_size_t rdcnt;
        snsr_data_t const *ptr = ringbuffer_get_read_buffer(&micBuffer, &rdcnt);

        while (rdcnt--) {
#if BUILD_APPLICATION_TYPE == APPLICATION_TYPE_DATA_STREAMER
            uint8_t headerbyte = 0xA5U;
            UART_Write(&headerbyte, 1);
            UART_Write((uint8_t *) ptr, micBuffer.itemsize);
            headerbyte = ~headerbyte;
            UART_Write(&headerbyte, 1);
#elif BUILD_APPLICATION_TYPE == APPLICATION_TYPE_LEVEL_METER
            // Compute RMS level: sqrt(1/n * sum(x^2))
            float db = 0;
            
            // sum(x^2)
            for (size_t i=0; i < AUDIO_BLOCK_NUM_SAMPLES; i++) {
                float x = (float) ptr[i] / 32768; // Convert 16-bit PCM to [-1,1)
                db += x*x;
            }
            // 1/n
            db /= AUDIO_BLOCK_NUM_SAMPLES;
            
            // convert to decibel
            db = 10 * log10(db + 1e-9); // Floor level at -90dB
            dbmeter = alpha*dbmeter + (1-alpha)*db;
            
            // Convert to 1-10 integer volume level
            int vol = (int) ((90 + db) / 9);

            // e.g.: -60 dB | [===       ]
            printf("%3.0fdB | [", dbmeter);
            for (int i=0; i < vol; i++)
                printf("=");
            for (int i=0; i < 10-vol; i++)
                printf(" ");
            printf("]\r");
#endif
            ptr += AUDIO_BLOCK_NUM_SAMPLES;
            ringbuffer_advance_read_index(&micBuffer, 1);
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

/*******************************************************************************
 End of File
*/

