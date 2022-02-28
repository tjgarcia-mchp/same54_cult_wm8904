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


// *****************************************************************************
// *****************************************************************************
// Section: Globals
// *****************************************************************************
// *****************************************************************************
#define DRV_I2S_DATA DRV_I2S_DATA16
#define CODEC_BUFFER_NUM_SAMPLES    512    // Fix chunk size at 32ms
#define AUDIO_BUFFER_NUM_BLOCKS     2
#define AUDIO_BUFFER_NUM_SAMPLES    (CODEC_BUFFER_NUM_SAMPLES*AUDIO_BUFFER_NUM_BLOCKS)

typedef int16_t snsr_data_t;

typedef enum APP_STATE {
    INIT=0,
    READING,
    READY,
    FATAL
} state_t;

static DRV_I2S_DATA _codecBuffer[AUDIO_BUFFER_NUM_SAMPLES] __attribute__ ((aligned (32)));
static snsr_data_t _micBuffer_data[AUDIO_BUFFER_NUM_SAMPLES];
static ringbuffer_t micBuffer;
static DRV_HANDLE drvHandle;

volatile state_t appState;
static volatile bool micBuffer_overrun;

// *****************************************************************************
// *****************************************************************************
// Section: Helpers and handlers
// *****************************************************************************
// *****************************************************************************
void AudioHandler(DRV_CODEC_BUFFER_EVENT event,
    DRV_CODEC_BUFFER_HANDLE bufferHandle, uintptr_t context)
{
    switch(event)
    {
        case DRV_CODEC_BUFFER_EVENT_COMPLETE:
        {
            ringbuffer_size_t wrcnt;
            snsr_data_t *ptr = ringbuffer_get_write_buffer(&micBuffer, &wrcnt);
            
            if (wrcnt < CODEC_BUFFER_NUM_SAMPLES) {
                micBuffer_overrun = true;
            }
            else {         
                for (size_t i=0; i < CODEC_BUFFER_NUM_SAMPLES; i++) {
                    *ptr++ = (snsr_data_t) _codecBuffer[i].rightData;
                }
                ringbuffer_advance_write_index(&micBuffer, CODEC_BUFFER_NUM_SAMPLES);
            }
            
            DRV_CODEC_BufferAddRead(drvHandle, &bufferHandle, _codecBuffer, sizeof(_codecBuffer));
            if(bufferHandle == DRV_CODEC_BUFFER_HANDLE_INVALID) {
                break;
            }
            
            appState = READY;
        }        
        break;

        case DRV_CODEC_BUFFER_EVENT_ABORT:
        {
            appState = READY; 
        } 
        break;
        
        default:
        {
            appState = FATAL; 
        }
        break;
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    SYS_MODULE_OBJ sysHandle;
    SYS_STATUS status;
    
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    sysHandle = sysObj.drvwm8904Codec0;
    drvHandle = DRV_HANDLE_INVALID;
    appState = INIT;
    
    float dbmeter = 0;
    while ( appState != FATAL )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        switch (appState) {
            case INIT:
            {
                if (ringbuffer_init(&micBuffer,
                        _micBuffer_data,
                        sizeof(_micBuffer_data) / sizeof(_micBuffer_data[0]),
                        sizeof(_micBuffer_data[0])))
                {
                    appState = FATAL;
                    break;
                }
                micBuffer_overrun = false;
                        
                // Wait for resource
                status = DRV_CODEC_Status(sysHandle);
                if (status != SYS_STATUS_READY) {
                    break;
                }
                
                // Open driver, set handler
                drvHandle = DRV_CODEC_Open(DRV_WM8904_INDEX_0, DRV_IO_INTENT_READWRITE | DRV_IO_INTENT_EXCLUSIVE);
                if (drvHandle != DRV_HANDLE_INVALID)
                {
                    DRV_CODEC_BufferEventHandlerSet(drvHandle, 
                        (DRV_CODEC_BUFFER_EVENT_HANDLER) AudioHandler, 
                        0);                    
                    
                    DRV_CODEC_BUFFER_HANDLE bufferHandle;
                    DRV_CODEC_BufferAddRead(drvHandle, &bufferHandle, _codecBuffer, sizeof(_codecBuffer));
                    if(bufferHandle == DRV_CODEC_BUFFER_HANDLE_INVALID) {
                        break;
                    }
                    appState = READY;
                }
            }
            break;
                
            case READY:
            {
                ringbuffer_size_t rdcnt;
                snsr_data_t const *ptr = ringbuffer_get_read_buffer(&micBuffer, &rdcnt);
                if (rdcnt < AUDIO_BUFFER_NUM_SAMPLES) {
                    continue;
                }

                // sqrt(1/n * sum(x^2))
                float db = 0;
                for (size_t i=0; i < AUDIO_BUFFER_NUM_SAMPLES; i++) {
                    float x = (float) ptr[i] / 32768;
                    db += x*x;
                }
                
                ringbuffer_advance_read_index(&micBuffer, AUDIO_BUFFER_NUM_SAMPLES);
                
                db /= AUDIO_BUFFER_NUM_SAMPLES;
                db = 10 * log10(db + 1e-9);
                dbmeter = 0.96*dbmeter + 0.04*db;
                int vol = (int) ((90 + db) / 9);
                
                printf("%6.2fdB | [", dbmeter);
                for (int i=0; i < vol; i++)
                    printf("=");
                for (int i=0; i < 10-vol; i++)
                    printf(" ");
                printf("]\r");                
            }
            break;
            
            case READING:
            {
                
            }
            break;
            
            case FATAL:
            {
                
            }
            
            default:
                break;
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

/*******************************************************************************
 End of File
*/

