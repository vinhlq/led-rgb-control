/*****************************************************************************
* File Name: capsense_platform_config.h
*
* Version 1.00
*
* Description:
*   This file contains the declarations of all the high-level APIs.
*
* Note:
*   N/A
*
* Owner:
*   vinhlq
*
* Related Document:
*
* Hardware Dependency:
*   N/A
*
* Code Tested With:
*
******************************************************************************
* Copyright (2019), vinhlq.
******************************************************************************
* This software is owned by vinhlq and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* (vinhlq) hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* (vinhlq) Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a (vinhlq) integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of (vinhlq).
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* (vinhlq) reserves the right to make changes without further notice to the
* materials described herein. (vinhlq) does not assume any liability arising out
* of the application or use of any product or circuit described herein. (vinhlq)
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of (vinhlq)' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies (vinhlq) against all charges. Use may be
* limited by and subject to the applicable (vinhlq) software license agreement.
*****************************************************************************/

#if !defined(LED_RGB_CONTROL_PLATFORM_CONFIG_H)
#define LED_RGB_CONTROL_PLATFORM_CONFIG_H
    
/*******************************************************************************
* Included headers
*******************************************************************************/
#include<stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "event-control-platform.h"
#include "event-control.h"

/*******************************************************************************
* User defined Macros
*******************************************************************************/

#ifndef CAPSENSE_INT_GPIO
#define CAPSENSE_INT_GPIO	GPIO_NUM_0
#endif

#ifndef CAPSENSE_I2C_NUM
#define CAPSENSE_I2C_NUM              I2C_NUM_0        /*!< I2C port number for master dev */
#endif

#ifndef CAPSENSE_CY8CMBR3XXX_NUMBER_OF_BUTTON
#define CAPSENSE_CY8CMBR3XXX_NUMBER_OF_BUTTON (4)
#endif

#ifndef CAPSENSE_CY8CMBR3XXX_BUTTON_MASK
#define CAPSENSE_CY8CMBR3XXX_BUTTON_MASK	(0xE4)
#endif

#ifndef CAPSENSE_CY8CMBR3XXX_SLAVE_ADDRESS
#define CAPSENSE_CY8CMBR3XXX_SLAVE_ADDRESS	(0x37)
#endif

/*******************************************************************************
* Data Type Definitions
*******************************************************************************/

/*******************************************************************************
* Structure Definitions
*******************************************************************************/

extern uint16_t ledRgbControlEventNumber;

#define ledRgbControlBlinkEventControlSetActive()				eventControlSetActive(ledRgbControlEventNumber)
#define ledRgbControlBlinkEventControlSetDelayMS(timerPeriodMs)	eventControlSetDelayMS(ledRgbControlEventNumber, timerPeriodMs)
#define ledRgbControlBlinkEventControlSetInactive()				eventControlSetInactive(ledRgbControlEventNumber)
#define ledRgbControlBlinkEventControlDebugEnable()				eventControlDebugEnable(ledRgbControlEventNumber);
/*******************************************************************************
* Function Prototypes
*******************************************************************************/

void ledRgbControlPlatformInit(void);

#endif

/****************************End of File***************************************/
