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
#include <stdint.h>
#include "definitions.h"                // SYS function prototypes
#include "ringbuffer.h"
#include "app.h"
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"
#include "model-parameters/model_metadata.h"

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
EI_IMPULSE_ERROR ei_sleep(int32_t us) {
    /* Optional: implement sleep */
    return EI_IMPULSE_OK;
}

uint64_t ei_read_timer_ms() {
    /* Optional: implement ms timer */
    return 0;
}

uint64_t ei_read_timer_us() {
    /* Optional: implement us timer */
    return 0;
}

extern "C" int _open (const char *buf, int flags, int mode)
{
    return 0;
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
    
    /* Define the input data */
    ei::signal_t signal;
    
    while ( appData.state != APP_STATE_FATAL )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        // Retrieve mic buffer
        ringbuffer_size_t rdcnt;
        snsr_data_t const *ptr = (snsr_data_t const *) ringbuffer_get_read_buffer(&micBuffer, &rdcnt);
        if (rdcnt == 0) {
            continue;
        }

        numpy::signal_from_buffer(ptr, DSP_BLOCK_NUM_SAMPLES, &signal);
        
        /* Make inference */
        ei_impulse_result_t result;
//        EI_IMPULSE_ERROR ei_status = run_classifier(&signal, &result, false);
        EI_IMPULSE_ERROR ei_status = run_classifier_continuous(&signal, &result, false);
        if (ei_status != EI_IMPULSE_OK) {
            // printf("run_classifier returned: %d\n", ei_status);
            break;
        }        

        ringbuffer_advance_read_index(&micBuffer, 1);
        
        printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
            result.timing.dsp, result.timing.classification, result.timing.anomaly);                
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            printf("    %s: %f\r\n", result.classification[ix].label, result.classification[ix].value);
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

/*******************************************************************************
 End of File
*/

