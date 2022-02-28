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
#include "definitions.h"                // SYS function prototypes
#include <math.h>

// *****************************************************************************
// *****************************************************************************
// Section: Globals
// *****************************************************************************
// *****************************************************************************
#define DRV_I2S_DATA DRV_I2S_DATA16
#define AUDIO_BUFFER_NUM_SAMPLES 512

typedef enum APP_STATE {
    INIT=0,
    READING,
    READY,
    FATAL
} state_t;


DRV_I2S_DATA codecBuffer[2][AUDIO_BUFFER_NUM_SAMPLES] __attribute__ ((aligned (32)));

volatile state_t appState = INIT;

// *****************************************************************************
// *****************************************************************************
// Section: Helpers and handlers
// *****************************************************************************
// *****************************************************************************
void AudioHandler(DRV_CODEC_BUFFER_EVENT event,
    DRV_CODEC_BUFFER_HANDLE handle, uintptr_t context)
{
    switch(event)
    {
        case DRV_CODEC_BUFFER_EVENT_COMPLETE:
        {
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
    uint8_t cBufferIndex = 0;
    SYS_MODULE_OBJ sysHandle;
    DRV_HANDLE drvHandle;
    DRV_CODEC_BUFFER_HANDLE bufferHandle;
    
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    sysHandle = sysObj.drvwm8904Codec0;
    drvHandle = DRV_HANDLE_INVALID;
    
    appState = INIT;
    SYS_STATUS status;
    
    float dbmeter = 0;
    while ( appState != FATAL )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        switch (appState) {
            case INIT:
            {
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
                    appState = READY;
                }
            }
            break;
                
            case READY:
            {
                appState = READING;
                
                // Set up new transfer and swap consumer-producer buffers
                DRV_CODEC_BufferAddRead(drvHandle, &bufferHandle, codecBuffer[cBufferIndex], sizeof(codecBuffer[0]));
                if(bufferHandle == DRV_CODEC_BUFFER_HANDLE_INVALID) {
                    continue;
                }
                cBufferIndex ^= 1;
                
                DRV_I2S_DATA *ptr = codecBuffer[cBufferIndex];

                // sqrt(1/n * sum(x^2))
                float db = 0;
                for (size_t i=0; i < AUDIO_BUFFER_NUM_SAMPLES; i++) {
                    float x = (float) ptr[i].rightData / 32768;
                    db += x*x;
                }
                
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

