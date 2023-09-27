#ifndef INC_DRV_PINMUX_H_
#define INC_DRV_PINMUX_H_

#include <stdint.h>

#define DRIVER_OK                               (0)
#define DRIVER_ERROR                            (-1)

#define PORT_NUMBER_0                                       (0)
#define PORT_NUMBER_1                                       (1)
#define PORT_NUMBER_2                                       (2)
#define PORT_NUMBER_3                                       (3)
#define PORT_NUMBER_4                                       (4)
#define PORT_NUMBER_5                                       (5)
#define PORT_NUMBER_6                                       (6)
#define PORT_NUMBER_7                                       (7)
#define PORT_NUMBER_8                                       (8)
#define PORT_NUMBER_9                                       (9)
#define PORT_NUMBER_10                                      (10)
#define PORT_NUMBER_11                                      (11)
#define PORT_NUMBER_12                                      (12)
#define PORT_NUMBER_13                                      (13)
#define PORT_NUMBER_14                                      (14)
#define PORT_NUMBER_15                                      (15)

#define PIN_NUMBER_0                                        (0)
#define PIN_NUMBER_1                                        (1)
#define PIN_NUMBER_2                                        (2)
#define PIN_NUMBER_3                                        (3)
#define PIN_NUMBER_4                                        (4)
#define PIN_NUMBER_5                                        (5)
#define PIN_NUMBER_6                                        (6)
#define PIN_NUMBER_7                                        (7)

#define PINMUX_ALTERNATE_FUNCTION_0                         (0)
#define PINMUX_ALTERNATE_FUNCTION_1                         (1)
#define PINMUX_ALTERNATE_FUNCTION_2                         (2)
#define PINMUX_ALTERNATE_FUNCTION_3                         (3)
#define PINMUX_ALTERNATE_FUNCTION_4                         (4)
#define PINMUX_ALTERNATE_FUNCTION_5                         (5)
#define PINMUX_ALTERNATE_FUNCTION_6                         (6)
#define PINMUX_ALTERNATE_FUNCTION_7                         (7)
#define PINMUX_ALTERNATE_FUNCTION_MASK                      (0xF)

#define PAD_FUNCTION_READ_ENABLE                            (1U << 16)
#define PAD_FUNCTION_SCHMITT_TRIGGER_ENABLE                 (1U << 17)
#define PAD_FUNCTION_SLEW_RATE_FAST                         (1U << 18)
#define PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_HIGH_Z       (0U << 19)
#define PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_PULL_UP      (1U << 19)
#define PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_PULL_DOWN    (2U << 19)
#define PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_BUS_REPEATER (3U << 19)
#define PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_02_MILI_AMPS     (0U << 21)
#define PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_04_MILI_AMPS     (1U << 21)
#define PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_08_MILI_AMPS     (2U << 21)
#define PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_12_MILI_AMPS     (3U << 21)
#define PAD_FUNCTION_DRIVER_OPEN_DRAIN                      (1U << 23)
#define PAD_FUNCTION_DISABLE                                (0)
#define PAD_FUNCTION_MASK                                   (0xFF0000)

#define PORT_MAX_PORT_NUMBER                    (15)
#define PORT_MAX_PIN_NUMBER                     (8)

typedef struct _PINMUX_RegInfo {
    volatile uint32_t pinmux[PORT_MAX_PORT_NUMBER][PORT_MAX_PIN_NUMBER];
} PINMUX_RegInfo;

int32_t PINMUX_Config (uint8_t port, uint8_t pin_no, uint8_t AF_number);
int32_t PINPAD_Config (uint8_t port, uint8_t pin_no, uint32_t function);
int32_t PINMUX_and_PAD_Config (uint8_t port, uint8_t pin_no, uint8_t AF_number, uint32_t function);

#endif /* INC_DRV_PINMUX_H_ */
