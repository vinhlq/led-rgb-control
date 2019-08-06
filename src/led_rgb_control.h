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

#ifndef LED_RGB_CONTROL_H
#define LED_RGB_CONTROL_H


void ledRgbControlBlinkEventHandler(void);

/** @brief start blink
 *
 *
 * @param index  Ver.: always
 * @param count  Ver.: always
 */
void ledRgbControlLedBlinkStart(uint8_t index, uint16_t count);

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlLedBlinkStartAll(uint16_t count);

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlLedBlinkStop(uint8_t index);

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlLedBlinkStopAll(void);

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlLedOutputAll(uint8_t level, uint8_t outputR, uint8_t outputG, uint8_t outputB);

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlLedOutput(uint8_t index, uint8_t level, uint8_t outputR, uint8_t outputG, uint8_t outputB);

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlLedStateRestore(uint8_t index);

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlLedStateRestoreAll(void);

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlBlinkSetOnValue(uint8_t outputR, uint8_t outputG, uint8_t outputB);

/** @brief start blink all led
 *
 *
 * @param count  Ver.: always
 */
void ledRgbControlBlinkSetOffValue(uint8_t outputR, uint8_t outputG, uint8_t outputB);

/** @brief LED RGB Control
 *  called from led-rgb-custom plugin, need to be implemented by app
 *  to control led RGB
 *
 *  @param index
 *  @param currentLevel 0:255
 *  @param R8 R 0:255
 *  @param R8 G 0:255
 *  @param B8 B 0:255
 */
void ledRGBControlOutputWRGBCallback(uint8_t index, uint8_t currentLevel, uint8_t R8, uint8_t G8, uint8_t B8);

#endif
