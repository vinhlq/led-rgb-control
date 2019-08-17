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
#include "led_rgb_control_temp_to_rgb.h"
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

static void ledRgbControlToggleRGB(uint8_t index, struct rgbOutput *on, struct rgbOutput *off)
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

static void ledRgbControlToggleRGBAll(struct rgbOutput *on, struct rgbOutput *off)
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

static void ledRgbControlStateSet(uint8_t index, uint8_t currentLevel, uint8_t outputR, uint8_t outputG, uint8_t outputB)
{
	ledOutputState[index].R = outputR;
	ledOutputState[index].G = outputG;
	ledOutputState[index].B = outputB;
	ledOutputState[index].brightness = currentLevel;
	stateValidFlags |= (1<<index);
}

static void ledRgbControlStateSetBrightness(uint8_t index, uint8_t currentLevel)
{
	ledOutputState[index].brightness = currentLevel;
	stateValidFlags |= (1<<index);
}

static void ledRgbControlStateSetRGB(uint8_t index, uint8_t outputR, uint8_t outputG, uint8_t outputB)
{
	ledOutputState[index].R = outputR;
	ledOutputState[index].G = outputG;
	ledOutputState[index].B = outputB;
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
			ledRgbControlStateRestore(blinkingIndex);
		}
		else
		{
			ledRgbControlStateRestoreAll();
		}
		blinkingIndex = INVALID_INDEX;
	}

	if(blinkingIndex < LED_RGB_CONTROL_LED_COUNT)
	{
		if(blinkingIndex >= 1 && blinkingIndex <= LED_RGB_CONTROL_LED_COUNT)
		{
			ledRgbControlToggleRGB(blinkingIndex, &blOn, &blOff);
		}
		else
		{
			ledRgbControlToggleRGB(blinkingIndex, &blOn, &blOff);
		}
	}
	else
	{
		ledRgbControlToggleRGBAll(&blOn, &blOff);
	}
//	debugPrintln("Blink");
}

void ledRgbControlStateRestore(uint8_t index)
{
	if(	index >= LED_RGB_CONTROL_LED_COUNT ||
		!(stateValidFlags & (1<<index)))
	{
		return;
	}
	ledRGBControlOutputWRGBCallback(index, ledOutputState[index].brightness, ledOutputState[index].R, ledOutputState[index].G, ledOutputState[index].B);
}

void ledRgbControlStateRestoreAll(void)
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

void ledRgbControlSet(uint8_t index, uint8_t level, uint8_t outputR, uint8_t outputG, uint8_t outputB)
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
	ledRgbControlStateSet(index, level, outputR, outputG, outputB);
}

void ledRgbControlSetRGB(uint8_t index, uint8_t outputR, uint8_t outputG, uint8_t outputB)
{
	if(index >= LED_RGB_CONTROL_LED_COUNT)
	{
		return;
	}
	if((stateValidFlags & (1<<index)) &&
		ledOutputState[index].R == outputR &&
		ledOutputState[index].G == outputG &&
		ledOutputState[index].B == outputB)
	{
//		debugPrintln("%p not set: %u", "Output", index);
		return;
	}
	ledRGBControlOutputWRGBCallback(index, ledOutputState[index].brightness, outputR, outputG, outputB);
	ledRgbControlStateSetRGB(index, outputR, outputG, outputB);
}

void ledRgbControlSetBrightness(uint8_t index, uint8_t brightness)
{
	if(index >= LED_RGB_CONTROL_LED_COUNT)
	{
		return;
	}
	if((stateValidFlags & (1<<index)) &&
		ledOutputState[index].brightness == brightness)
	{
//		debugPrintln("%p not set: %u", "Output", index);
		return;
	}
	ledRGBControlOutputWRGBCallback(index, brightness, ledOutputState[index].R, ledOutputState[index].G, ledOutputState[index].B);
	ledRgbControlStateSetBrightness(index, brightness);
}

void ledRgbControlSetAll(uint8_t level, uint8_t outputR, uint8_t outputG, uint8_t outputB)
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
		ledRgbControlStateSet(i, level, outputR, outputG, outputB);
	}
}

#ifdef LED_RGB_CONTROL_COLOR_XY
// apply RGB transform to XY input
// Note:  X and Y are well-known quantities in the color space.  It comes
// from the CIE xyY color model.  Here, currentX and currentY are the ZCL
// attributes where x = currentX / 65536 and y = currentY / 65536.
// http://en.wikipedia.org/wiki/CIE_1931_color_space for more details
static uint8_t ledRgbComputeXy
(
	uint16_t currentX, uint16_t currentY,
	uint8_t *R8, uint8_t *G8, uint8_t *B8
)
{

  uint32_t scratch;
  uint32_t X32, Y32, Z32;
  int32_t R32, G32, B32;

  // compute x, y, z
  X32 = currentX;
  Y32 = currentY;

  scratch = X32 + Y32;

  if (scratch > 65536l)
  {
    debugPrintln("X and Y are too big");
    return 0;
  }

  Z32 = 65536l - (X32 + Y32);

  // now we can compute the RGB values in 65,536,000
  // these are well-known constants but are documented at:
  // http://docs-hoffmann.de/ciexyz29082000.pdf which came from
  // "Digital Color Management, Giorgianni+Madden
  R32 = (X32 * 2365) - (Y32 * 897) - (Z32 * 468);
  G32 = ((Y32 * 1426) + (Z32 * 89)) - (X32 * 515);
  B32 = (X32 * 5) + (Z32 * 1009) - (Y32 * 14);

  // Note:  it is possible the above algorithm will create a negative drive
  // value.  We need to check for that and set it to zero.
  if (R32 < 0)
  {
    R32 = 0;
  }

  if (G32 < 0)
  {
    G32 = 0;
  }

  if (B32 < 0)
  {
    B32 = 0;
  }

  R32 *= 256;
  *R8 = R32 / 65536000;
  G32 *= 256;
  *G8 = G32 / 65536000;
  B32 *= 256;
  *B8 = B32 / 65536000;
  return 1;
}

void ledRgbControlSetColorXy(uint8_t index, uint8_t currentLevel, uint16_t currentX, uint16_t currentY)
{
	uint8_t R8, G8, B8;

	if(index >= LED_RGB_CONTROL_LED_COUNT)
	{
		return;
	}

	if(!currentLevel)
	{
		currentLevel = ledOutputState[index].brightness;
	}

	debugPrintln
		  (
			  "%s: %u, %s XY: %u,%u",
			  "Level", currentLevel,
			  "Color", currentX, currentY
		  );
	if(ledRgbComputeXy(currentX, currentY, &R8, &G8, &B8))
	{
		ledRgbControlSet(index, currentLevel, R8, G8, B8);
	}
}
#endif

#ifdef LED_RGB_CONTROL_COLOR_TEMP
static uint8_t ledRgbComputeTemp
(
	uint16_t currentTemp,
	uint8_t *R8, uint8_t *G8, uint8_t *B8
)
{
  // Use precomputed values for RED, GREEN and BLUE based on algorithm by
  // Robertson.
  static uint8_t tempRedValue[]   = { RED_VALUES };
  static uint8_t tempGreenValue[] = { GREEN_VALUES };
  static uint8_t tempBlueValue[]  = { BLUE_VALUES };

  debugPrintln(	  "%s: %u, %s temperature: %u",
				  "Level", currentLevel,
				  "Color", currentTemp);

  // temperature is 1,000,000/currentTemperature.
  // I computed R,G,B from a simple algorithm from Robertson and created
  // tables.
  currentTemp = currentTemp / 10;
  if (currentTemp > 99)
  {
    currentTemp = 99;
  }

  // red, green, blue values are in 0..255.  Just add white at the max.
  *R8 = tempRedValue[currentTemp];
  *G8 = tempGreenValue[currentTemp];
  *B8 = tempBlueValue[currentTemp];
  return 1;
}

void ledRgbControlSetColorTemp(uint8_t index, uint8_t currentLevel, uint16_t currentTemp)
{
	uint8_t R8, G8, B8;

	if(index >= LED_RGB_CONTROL_LED_COUNT)
	{
		return;
	}

	if(!currentLevel)
	{
		currentLevel = ledOutputState[index].brightness;
	}

	debugPrintln
		  (
			  "%s: %u, %s XY: %u,%u",
			  "Level", currentLevel,
			  "Color", currentX, currentY
		  );
	if(ledRgbComputeTemp(currentTemp, &R8, &G8, &B8))
	{
		ledRgbControlSet(index, currentLevel, R8, G8, B8);
	}
}
#endif

#ifdef LED_RGB_CONTROL_COLOR_HSV
static uint8_t ledRgbComputeHsv
(
	uint8_t hue, uint8_t saturation,
	uint8_t *R8, uint8_t *G8, uint8_t *B8
)
{
  uint32_t min32, hue32, delta32, sat32, level32;

  uint32_t R32, G32, B32;

  // algorithm taken from wikipedia
  // http://en.wikipedia.org/wiki/CIE_1931_color_space for more details

  // note:  hue and saturation are 0..254.  level (i.e. value for HSV) is
  // 0..255.  But most of these computations assume 0..1 for saturation and
  // value, and 0..360 for hue.  This will be a little tricky to compute RGB
  // using fixed point math and not lose any bits of significance.

  // first switch to 32 bit mode.
  level32 = 255;
  sat32 = (uint32_t) saturation;

  min32 = level32 * (254 - sat32);
  min32 = min32 / 254;
  delta32 = level32 - min32;
  hue32 = ((uint32_t) hue);  // need to map it to 0..6.  really is 0..254.

  // formula is X = C * { 1 - ( H mod2 - 1) }.  Becuase the nubmers don't line
  // up, we need to do this with if's.
  // The ranges are 0, 42, 84, 127, 169, 211, 254.
  if (hue < 43) {
    R32 = level32; // 0..254
    // convert 0..42 to 0..delta32
    G32 = hue32 * delta32;
    G32 = G32 / 42;
    G32 = G32 + min32;
    B32 = min32;
  }
  else if (hue < 85)
  {
    hue32 -= 42;
    // convert 0..42 to delta32..0
    R32 = (42 - hue32) * delta32;
    R32 = R32 / 42;
    R32 = R32 + min32;
    G32 = level32;
    B32 = min32;
  }
  else if (hue < 128)
  {
    hue32 -= 84;
    R32 = min32;
    G32 = level32;
    // convert 0..43 to 0..delta32
    B32 = hue32 * delta32;
    B32 = B32 / 43;
    B32 = B32 + min32;
  }
  else if (hue < 170)
  {
    hue32 -= 127;
    R32 = min32;
    // convert 0..42 to delta32..0
    G32 = (42 - hue32) * delta32;
    G32 = G32 / 42;
    G32 = G32 + min32;
    B32 = level32;
  }
  else if (hue < 212)
  {
    hue32 -= 169;
    // convert 0..42 to 0..delta32
    R32 = hue32 * delta32;
    R32 = R32 / 42;
    R32 = R32 + min32;
    G32 = min32;
    B32 = level32;
  }
  else
  { //hue is 212..254
    hue32 -= 211;
    R32 = level32;
    G32 = min32;
    // convert 0..43 to delta32..0
    B32 = (43 - hue32) * delta32;
    B32 = B32 / 42;
    B32 = B32 + min32;
  }

  R32 *= 256;
  *R8 = R32 / 254;
  G32 *= 256;
  *G8 = G32 / 254;
  B32 *= 256;
  *B8 = B32 / 254;
  return 1;
}

void ledRgbControlSetColorHsv(uint8_t index, uint8_t currentLevel, uint8_t hue, uint8_t saturation)
{
	uint8_t R8, G8, B8;

	if(index >= LED_RGB_CONTROL_LED_COUNT)
	{
		return;
	}

	if(!currentLevel)
	{
		currentLevel = ledOutputState[index].brightness;
	}

	debugPrintln
		(
			"%s: %u, %s HSV: %u,%u",
			"Level", currentLevel,
			"Color", hue, saturation
		);
	if(ledRgbComputeHsv(hue, saturation, &R8, &G8, &B8))
	{
		ledRgbControlSet(index, currentLevel, R8, G8, B8);
	}
}
#endif


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
void ledRgbControlBlinkStart(uint8_t index, uint16_t count)
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
void ledRgbControlBlinkStartAll(uint16_t count)
{
	ledRgbControlBlinkStart(LED_RGB_CONTROL_LED_COUNT, count);
}

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlBlinkStop(uint8_t index)
{
	if(index != INVALID_INDEX && index != blinkingIndex)
	{
		return;
	}
	debugPrintln("%p stop: %u", "Blink", index);
	ledRgbControlBlinkEventControlSetInactive();
	if(blinkingIndex < INVALID_INDEX)
	{
		ledRgbControlStateRestore(blinkingIndex);
	}
	else
	{
		ledRgbControlStateRestoreAll();
	}
	blinkingIndex = INVALID_INDEX;
}

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlBlinkStopAll(void)
{
	ledRgbControlBlinkStop(INVALID_INDEX);
}



/*
 * EOF
 */

