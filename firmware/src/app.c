/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

// Module level globals
static DRV_I2S_DATA _codecBuffer[AUDIO_BLOCK_NUM_SAMPLES] __attribute__ ((aligned (16)));
static DRV_HANDLE drvHandle;
static snsr_data_t _micBuffer_data[AUDIO_BUFFER_NUM_BLOCKS][AUDIO_BLOCK_NUM_SAMPLES];

// Application global variables
APP_DATA appData;
ringbuffer_t micBuffer;
volatile bool micBuffer_overrun;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/
void AudioHandler(DRV_CODEC_BUFFER_EVENT event,
    DRV_CODEC_BUFFER_HANDLE bufferHandle, uintptr_t context)
{
    (void) context;
    switch(event)
    {
        case DRV_CODEC_BUFFER_EVENT_COMPLETE:
        {
            ringbuffer_size_t wrcnt;
            snsr_data_t *ptr = ringbuffer_get_write_buffer(&micBuffer, &wrcnt);
            
            if (wrcnt == 0) {
                micBuffer_overrun = true;
            }
            else {         
                for (size_t i=0; i < AUDIO_BLOCK_NUM_SAMPLES; i++) {
                    *ptr++ = (snsr_data_t) _codecBuffer[i].rightData;
                }
                ringbuffer_advance_write_index(&micBuffer, 1);
            }
            
            DRV_CODEC_BufferAddRead(drvHandle, &bufferHandle, _codecBuffer, sizeof(_codecBuffer));
            if(bufferHandle == DRV_CODEC_BUFFER_HANDLE_INVALID) {
                break;
            }
        }        
        break;

        case DRV_CODEC_BUFFER_EVENT_ABORT:
        {

        } 
        break;
        
        default:
        {
            
        }
        break;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{    
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{    
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            SYS_STATUS status;
            SYS_MODULE_OBJ sysHandle;

            sysHandle = sysObj.drvwm8904Codec0;
            drvHandle = DRV_HANDLE_INVALID;            
            
            // Wait for resource
            status = DRV_CODEC_Status(sysHandle);
            if (status != SYS_STATUS_READY) {
                break;
            }

            if (ringbuffer_init(&micBuffer,
                    _micBuffer_data,
                    sizeof(_micBuffer_data) / sizeof(_micBuffer_data[0]),
                    sizeof(_micBuffer_data[0])))
            {
                appData.state = APP_STATE_FATAL;
                break;
            }

            micBuffer_overrun = false;    

            // Open driver, set handler
            drvHandle = DRV_CODEC_Open(DRV_WM8904_INDEX_0, DRV_IO_INTENT_READ | DRV_IO_INTENT_EXCLUSIVE);
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
                
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {

            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
