/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef __BOARD_UTILS__
#define __BOARD_UTILS__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

 // <o> User BUTTON1 (JOY_SW4 - down) GPIO port number and pin number
#define BOARD_BUTTON1_GPIO_PORT                 LP
#define BOARD_BUTTON1_PIN_NO                    3

// <o> User BUTTON2 (JOY_SW5 - click) GPIO port number and pin number
#define BOARD_BUTTON2_GPIO_PORT                 LP
#define BOARD_BUTTON2_PIN_NO                    4

typedef void (*BOARD_Callback_t) (uint32_t event);

typedef enum {
	BOARD_BUTTON_ENABLE_INTERRUPT = 1,  /**<BUTTON interrupt enable>*/
	BOARD_BUTTON_DISABLE_INTERRUPT,     /**<BUTTON interrupt disable>*/
} BOARD_BUTTON_CONTROL;

typedef enum {
    BOARD_BUTTON_STATE_LOW,             /**<BUTTON state is LOW>*/
    BOARD_BUTTON_STATE_HIGH,            /**<BUTTON state is HIGH>*/
} BOARD_BUTTON_STATE;

typedef enum {
    BOARD_LED_STATE_LOW,                /**<Drive LED output state LOW>*/
    BOARD_LED_STATE_HIGH,               /**<Drive LED output state HIGH>*/
    BOARD_LED_STATE_TOGGLE,             /**<Toggle LED output state>*/
} BOARD_LED_STATE;

void BOARD_UTILS_Init(void);
void BOARD_BUTTON1_Init(BOARD_Callback_t user_cb);
void BOARD_BUTTON2_Init(BOARD_Callback_t user_cb);
void BOARD_BUTTON1_Control(BOARD_BUTTON_CONTROL control);
void BOARD_BUTTON2_Control(BOARD_BUTTON_CONTROL control);
void BOARD_BUTTON1_GetState(BOARD_BUTTON_STATE *state);
void BOARD_BUTTON2_GetState(BOARD_BUTTON_STATE *state);
void BOARD_LED1_Control(BOARD_LED_STATE state);
void BOARD_LED2_Control(BOARD_LED_STATE state);
#ifdef __cplusplus
}
#endif
#endif // __BOARD_UTILS__
