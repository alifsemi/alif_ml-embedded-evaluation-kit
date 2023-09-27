#include "RTE_Components.h"
#include CMSIS_device_header
#include "drv_pinmux_new.h"

int32_t PINMUX_Config (uint8_t port, uint8_t pin_no, uint8_t AF_number) {

    if (port > PORT_NUMBER_14)                      { return DRIVER_ERROR; }
    if (pin_no > PIN_NUMBER_7)                      { return DRIVER_ERROR; }
    if (AF_number > PINMUX_ALTERNATE_FUNCTION_7)    { return DRIVER_ERROR; }

    PINMUX_RegInfo *reg_ptr = (PINMUX_RegInfo*)(PINMUX_BASE);

    uint32_t reg_data = reg_ptr->pinmux[port][pin_no];

    reg_data &= (~PINMUX_ALTERNATE_FUNCTION_MASK);
    reg_data |= (AF_number & PINMUX_ALTERNATE_FUNCTION_MASK);

    reg_ptr->pinmux[port][pin_no] = reg_data;

    return DRIVER_OK;
}

int32_t PINPAD_Config (uint8_t port, uint8_t pin_no, uint32_t function) {

    if (port > PORT_NUMBER_15)                      { return DRIVER_ERROR; }
    if (pin_no > PIN_NUMBER_7)                      { return DRIVER_ERROR; }

    if (port < PORT_NUMBER_15)
    {
		PINMUX_RegInfo *reg_ptr = (PINMUX_RegInfo*)(PINMUX_BASE);

		uint32_t reg_data = reg_ptr->pinmux[port][pin_no];

		reg_data &= (~PAD_FUNCTION_MASK);
		reg_data |= (function & PAD_FUNCTION_MASK);

		reg_ptr->pinmux[port][pin_no] = reg_data;
    }
    else
    {
        uint32_t *reg_ptr = (uint32_t*)(LPGPIO_CTRL_BASE + (0x4 * pin_no));
        *reg_ptr = (function & 0xFB0000) >> 16;
    }

    return DRIVER_OK;
}

int32_t PINMUX_and_PAD_Config (uint8_t port, uint8_t pin_no, uint8_t AF_number, uint32_t function) {

    if (port > PORT_NUMBER_14)                      { return DRIVER_ERROR; }
    if (pin_no > PIN_NUMBER_7)                      { return DRIVER_ERROR; }
    if (AF_number > PINMUX_ALTERNATE_FUNCTION_7)    { return DRIVER_ERROR; }

    PINMUX_RegInfo *reg_ptr = (PINMUX_RegInfo*)(PINMUX_BASE);

    uint32_t reg_data = reg_ptr->pinmux[port][pin_no];

    reg_data &= (~PINMUX_ALTERNATE_FUNCTION_MASK);
    reg_data |= (AF_number & PINMUX_ALTERNATE_FUNCTION_MASK);

    reg_data &= (~PAD_FUNCTION_MASK);
    reg_data |= (function & PAD_FUNCTION_MASK);

    reg_ptr->pinmux[port][pin_no] = reg_data;

    return DRIVER_OK;
}

