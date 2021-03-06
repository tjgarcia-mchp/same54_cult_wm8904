/* Microchip Technology Inc. and its subsidiaries.  You may use this software
 * and any derivatives exclusively with Microchip products.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
 * TERMS.
 */

/*
 * File:
 * Author:
 * Comments:
 * Revision history:
 */

// This is a guard condition so that contents of this file are not included
// more than once.
#ifndef APP_CONFIG_H
#define	APP_CONFIG_H
// *****************************************************************************
// *****************************************************************************
// Section: Build application types
// *****************************************************************************
// *****************************************************************************
#define APPLICATION_TYPE_LEVEL_METER    0
#define APPLICATION_TYPE_DATA_STREAMER  1

// *****************************************************************************
// *****************************************************************************
// Section: User config parameters
// *****************************************************************************
// *****************************************************************************
#define BUILD_APPLICATION_TYPE      APPLICATION_TYPE_DATA_STREAMER
//#define BUILD_APPLICATION_TYPE      APPLICATION_TYPE_LEVEL_METER
#define AUDIO_BLOCK_SIZE_MS         16      // Size of DMA block in ms
#define AUDIO_BUFFER_NUM_BLOCKS     8       // Number of DMA blocks to buffer;
                                            // must be a power of 2

// *****************************************************************************
// *****************************************************************************
// Section: Other global parameters
// *****************************************************************************
// *****************************************************************************
#define AUDIO_BLOCK_NUM_SAMPLES     (16*(AUDIO_BLOCK_SIZE_MS))
#define AUDIO_BUFFER_NUM_SAMPLES    ((AUDIO_BUFFER_NUM_BLOCKS)*(AUDIO_BLOCK_NUM_SAMPLES))

// I2S configured for 16-bit
#define DRV_I2S_DATA DRV_I2S_DATA16

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

typedef int16_t snsr_data_t;
    
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* APP_CONFIG_H */
