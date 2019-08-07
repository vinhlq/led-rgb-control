/***************************************************************************//**
 * @file
 * @brief Definitions for the LED RGB PWM plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdint.h>
#include <string.h>
#include "led_rgb_control_platform.h"
#include "led_rgb_control.h"

#if defined(LED_RGB_CONTROL_DEBUG_ENABLED)
#	if defined(EMBER_AF_PRINT_ENABLE)
#  		if defined(EMBER_AF_PRINT_CUSTOM1)
#  		define debugPrintln emberAfCustom1Println
#  		elif defined(EMBER_AF_PRINT_CUSTOM2)
#  		define debugPrintln emberAfCustom2Println
#  		elif defined(EMBER_AF_PRINT_CUSTOM3)
#  		define debugPrintln emberAfCustom3Println
#  		else
#  		define debugPrintln emberAfAppPrintln
#		endif
#	else
#	define debugPrintln(fmt,args...)	printf(fmt "%s", ## args, "\r\n")
#  	endif
#else
  #define debugPrintln(...)
#endif

#ifndef LED_RGB_CONTROL_LED_COUNT
#define LED_RGB_CONTROL_LED_COUNT (4)
#endif

#ifndef LED_RGB_CONTROL_BLINK_PERIOD
#define LED_RGB_CONTROL_BLINK_PERIOD (400)
#endif

#define INVALID_INDEX	LED_RGB_CONTROL_LED_COUNT

struct rgbOutput
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t brightness;
};
struct rgbOutput ledOutputState[LED_RGB_CONTROL_LED_COUNT] = {0};
#define LED_RGB_CONTROL_BLINK_ONOFF_MS	(LED_RGB_CONTROL_BLINK_PERIOD/2)
#define BLINKING_PERIOD_SEC2COUNT(sec)	(sec*1000/LED_RGB_CONTROL_BLINK_ONOFF_MS/2)
static struct rgbOutput blOn={0,200,0,80};
static struct rgbOutput blOff={0,0,0,80};

static uint8_t blinkingIndex;
static uint16_t blinkingCount = 0;
#define BLINK_FLAG_ALL	(1<<LED_RGB_CONTROL_LED_COUNT)
static uint32_t blinkFlag = 0;
static uint32_t stateValidFlags = 0;

static void ledRgbControlLedToggleRGB(uint8_t index, struct rgbOutput *on, struct rgbOutput *off)
{
	if(blinkFlag & (1<<index))
	{
		blinkFlag &= ~(1<<index);
		ledRGBControlOutputWRGBCallback(index, on->brightness, on->R, on->G, on->B);
	}
	else
	{
		blinkFlag |= (1<<index);
		ledRGBControlOutputWRGBCallback(index, off->brightness, off->R, off->G, off->B);
	}

	// set invalid state
	stateValidFlags &= ~(1<<index);
}

static void ledRgbControlLedToggleRGBAll(struct rgbOutput *on, struct rgbOutput *off)
{

	uint8_t i;
	uint8_t onOff;

	if(blinkFlag & BLINK_FLAG_ALL)
	{
		onOff = 1;
		blinkFlag &= ~BLINK_FLAG_ALL;
	}
	else
	{
		onOff = 0;
		blinkFlag |= BLINK_FLAG_ALL;
	}

	for(i=0; i<LED_RGB_CONTROL_LED_COUNT; i++)
	{
		if(onOff)
		{
			ledRGBControlOutputWRGBCallback(i, on->brightness, on->R, on->G, on->B);

		}
		else
		{
			ledRGBControlOutputWRGBCallback(i, off->brightness, off->R, off->G, off->B);
		}

		// set invalid state
		stateValidFlags &= ~(1<<i);
	}
}

static void ledRgbControlLedStateSet(uint8_t index, uint8_t currentLevel, uint8_t outputR, uint8_t outputG, uint8_t outputB)
{
	ledOutputState[index].R = outputR;
	ledOutputState[index].G = outputG;
	ledOutputState[index].B = outputB;
	ledOutputState[index].brightness = currentLevel;
	stateValidFlags |= (1<<index);
}

void ledRgbControlBlinkEventHandler(void)
{
	if(blinkingCount)
	{
		ledRgbControlBlinkEventControlSetDelayMS(LED_RGB_CONTROL_BLINK_ONOFF_MS);
		if(blinkFlag)
		{
			blinkingCount--;
		}
	}
	else
	{
		ledRgbControlBlinkEventControlSetInactive();
		if(blinkingIndex < INVALID_INDEX)
		{
			ledRgbControlLedStateRestore(blinkingIndex);
		}
		else
		{
			ledRgbControlLedStateRestoreAll();
		}
		blinkingIndex = INVALID_INDEX;
	}

	if(blinkingIndex < LED_RGB_CONTROL_LED_COUNT)
	{
		if(blinkingIndex >= 1 && blinkingIndex <= LED_RGB_CONTROL_LED_COUNT)
		{
			ledRgbControlLedToggleRGB(blinkingIndex, &blOn, &blOff);
		}
		else
		{
			ledRgbControlLedToggleRGB(blinkingIndex, &blOn, &blOff);
		}
	}
	else
	{
		ledRgbControlLedToggleRGBAll(&blOn, &blOff);
	}
//	debugPrintln("Blink");
}

void ledRgbControlLedStateRestore(uint8_t index)
{
	if(	index >= LED_RGB_CONTROL_LED_COUNT ||
		!(stateValidFlags & (1<<index)))
	{
		return;
	}
	ledRGBControlOutputWRGBCallback(index, ledOutputState[index].brightness, ledOutputState[index].R, ledOutputState[index].G, ledOutputState[index].B);
}

void ledRgbControlLedStateRestoreAll(void)
{
	uint8_t i;

	for(i=0; i<LED_RGB_CONTROL_LED_COUNT; i++)
	{
		if(stateValidFlags & (1<<i))
		{
			ledRGBControlOutputWRGBCallback(i, ledOutputState[i].brightness, ledOutputState[i].R, ledOutputState[i].G, ledOutputState[i].B);
		}
	}
}

void ledRgbControlLedOutput(uint8_t index, uint8_t level, uint8_t outputR, uint8_t outputG, uint8_t outputB)
{
	if(index >= LED_RGB_CONTROL_LED_COUNT)
	{
		return;
	}
	if((stateValidFlags & (1<<index)) &&
		ledOutputState[index].brightness == level &&
		ledOutputState[index].R == outputR &&
		ledOutputState[index].G == outputG &&
		ledOutputState[index].B == outputB)
	{
//		debugPrintln("%p not set: %u", "Output", index);
		return;
	}
	ledRGBControlOutputWRGBCallback(index, level, outputR, outputG, outputB);
	ledRgbControlLedStateSet(index, level, outputR, outputG, outputB);
}

void ledRgbControlLedOutputAll(uint8_t level, uint8_t outputR, uint8_t outputG, uint8_t outputB)
{
	uint8_t i;

	for(i=0; i<LED_RGB_CONTROL_LED_COUNT; i++)
	{
		if((stateValidFlags & (1<<i)) &&
			ledOutputState[i].brightness == level &&
			ledOutputState[i].R == outputR &&
			ledOutputState[i].G == outputG &&
			ledOutputState[i].B == outputB)
		{
			continue;
		}
		ledRGBControlOutputWRGBCallback(i, level, outputR, outputG, outputB);
		ledRgbControlLedStateSet(i, level, outputR, outputG, outputB);
	}
}

void ledRgbControlBlinkSetOnValue(uint8_t outputR, uint8_t outputG, uint8_t outputB)
{
	blOn.R = outputR;
	blOn.G = outputG;
	blOn.B = outputB;
}

void ledRgbControlBlinkSetOffValue(uint8_t outputR, uint8_t outputG, uint8_t outputB)
{
	blOff.R = outputR;
	blOff.G = outputG;
	blOff.B = outputB;
}

/** @brief start blink
 *
 *
 * @param index  Ver.: always
 * @param count  Ver.: always
 */
void ledRgbControlLedBlinkStart(uint8_t index, uint16_t count)
{
	if(index > LED_RGB_CONTROL_LED_COUNT)
	{
		return;
	}
	debugPrintln("%p start: %u:%u", "Blink", index, count);
	ledRgbControlBlinkEventControlSetActive();
	blinkingIndex = index;
	blinkingCount = count;
	blinkFlag = 0;
}

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlLedBlinkStartAll(uint16_t count)
{
	ledRgbControlLedBlinkStart(LED_RGB_CONTROL_LED_COUNT, count);
}

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlLedBlinkStop(uint8_t index)
{
	if(index != INVALID_INDEX && index != blinkingIndex)
	{
		return;
	}
	debugPrintln("%p stop: %u", "Blink", index);
	ledRgbControlBlinkEventControlSetInactive();
	if(blinkingIndex < INVALID_INDEX)
	{
		ledRgbControlLedStateRestore(blinkingIndex);
	}
	else
	{
		ledRgbControlLedStateRestoreAll();
	}
	blinkingIndex = INVALID_INDEX;
}

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlLedBlinkStopAll(void)
{
	ledRgbControlLedBlinkStop(INVALID_INDEX);
}



/*
 * EOF
 */

