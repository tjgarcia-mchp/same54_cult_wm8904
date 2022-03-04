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
#include "kb.h"
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

void init_model(void)
{
    kb_model_init();
}

/*
* This function implements inferencing for a single model knowledgepack. Visit
* the SensiML documentation to learn how to implement more complex models:
* https://sensiml.com/documentation/knowledge-packs/building-a-knowledge-pack-library.html#calling-knowledge-pack-apis-from-your-code
*/
#define KB_MODEL_Parent_INDEX 0
int run_model(SENSOR_DATA_T *data, int nsensors)
{
    int ret;

    ret = kb_run_model((SENSOR_DATA_T *) data, nsensors, KB_MODEL_Parent_INDEX);
    if (ret >= 0) {
        kb_reset_model(0);
    };

    return ret;
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main ( void )
{    
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    init_model();
    
    while ( appData.state != APP_STATE_FATAL )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        // Retrieve mic buffer
        ringbuffer_size_t rdcnt;
        snsr_data_t const *ptr = ringbuffer_get_read_buffer(&micBuffer, &rdcnt);

        while (rdcnt--) {
            /* Feed sample into the SensiML API */
            for (size_t i=0; i < AUDIO_BLOCK_NUM_SAMPLES; i++) {
                int ret = run_model((SENSOR_DATA_T *) ptr++, 1);
                if (ret >= 0) {
                    printf("Classification: %d\n", ret);
                }
            }
            ringbuffer_advance_read_index(&micBuffer, 1);
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

/*******************************************************************************
 End of File
*/

