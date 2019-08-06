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
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/event.h"
#include "app/framework/include/af.h"

/*******************************************************************************
* User defined Macros
*******************************************************************************/

#define LED_RGB_CONTROL_LED_COUNT 		EMBER_AF_PLUGIN_LED_RGB_CONTROL_LED_COUNT
#define LED_RGB_CONTROL_BLINK_PERIOD	EMBER_AF_PLUGIN_LED_RGB_CONTROL_BLINK_PERIOD
#define LED_RGB_CONTROL_DEBUG			EMBER_AF_PLUGIN_LED_RGB_CONTROL_DEBUG
#define ledRGBControlOutputWRGBCallback emberAfPluginLedRGBControlOutputWRGBCallback
#define ledRgbControlBlinkEventHandler	emberAfPluginLedRgbControlBlinkEventHandler

/*******************************************************************************
* Data Type Definitions
*******************************************************************************/

/*******************************************************************************
* Structure Definitions
*******************************************************************************/

extern uint16_t ledRgbControlEventNumber;

#define ledRgbControlBlinkEventControlSetActive()				emberEventControlSetActive(emberAfPluginLedRgbControlBlinkEventControl)
#define ledRgbControlBlinkEventControlSetDelayMS(timerPeriodMs)	emberEventControlSetDelayMS(emberAfPluginLedRgbControlBlinkEventControl, timerPeriodMs)
#define ledRgbControlBlinkEventControlSetInactive()				emberEventControlSetInactive(emberAfPluginLedRgbControlBlinkEventControl)

extern EmberEventControl emberAfPluginLedRgbControlBlinkEventControl;
/*******************************************************************************
* Function Prototypes
*******************************************************************************/

void ledRgbControlPlatformInit(void);

#endif

/****************************End of File***************************************/
