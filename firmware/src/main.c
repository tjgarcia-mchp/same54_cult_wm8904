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


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    float dbmeter = 0;
    float alpha = 0.9;
    while ( appData.state != APP_STATE_FATAL )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        // Retrieve mic buffer
        ringbuffer_size_t rdcnt;
        snsr_data_t const *ptr = ringbuffer_get_read_buffer(&micBuffer, &rdcnt);
        if (rdcnt == 0) {
            continue;
        }

        // sqrt(1/n * sum(x^2))
        float db = 0;
        for (size_t i=0; i < AUDIO_BLOCK_SIZE; i++) {
            float x = (float) ptr[i] / 32768;
            db += x*x;
        }

        ringbuffer_advance_read_index(&micBuffer, 1);

        db /= AUDIO_BLOCK_SIZE;
        db = 10 * log10(db + 1e-9);
        dbmeter = alpha*dbmeter + (1-alpha)*db;
        int vol = (int) ((90 + db) / 9);

        printf("%6.2fdB | [", dbmeter);
        for (int i=0; i < vol; i++)
            printf("=");
        for (int i=0; i < 10-vol; i++)
            printf(" ");
        printf("]\r");
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

/*******************************************************************************
 End of File
*/

