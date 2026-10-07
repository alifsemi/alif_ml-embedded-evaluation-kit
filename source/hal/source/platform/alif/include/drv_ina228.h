/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef DRV_INA228_H
#define DRV_INA228_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INA228_16BREG 2
#define INA228_24BREG 3
#define INA228_40BREG 5

typedef enum {
    CONFIG = 0,
    ADC_CONFIG,
    SHUNT_CAL,
    SHUNT_TEMPCO,
    VSHUNT,
    VBUS,
    DIETEMP,
    CURRENT,
    POWER,
    ENERGY,
    CHARGE,
    DIAG_ALRT,
    SOVL,
    SUVL,
    BOVL,
    BUVL,
    TEMP_LIMIT,
    PWR_LIMIT,
    MANUFACTURER_ID = 0x3e,
    DEVICE_ID = 0x3f
} INA228_REGS;

/* Shunt selection: a supported value in mOhm (15 or 2000), or INA228_SHUNT_AUTO to have the
 * caller detect it at boot with INA228_DetectShunt(). */
#define INA228_SHUNT_AUTO 0

/* CONFIG.ADCRANGE values for INA228_SetADCRange(). */
#define INA228_ADCRANGE_163_84MV 0   /* +/-163.84 mV full scale, 312.5 nV/LSB (default) */
#define INA228_ADCRANGE_40_96MV  1   /* +/-40.96 mV full scale, 78.125 nV/LSB */

int32_t INA228_Init  (uint8_t *dev_addr, uint8_t dev_count);
int32_t INA228_SetADCRange(uint8_t dev_addr, uint8_t range);
int32_t INA228_DetectShunt(uint8_t dev_addr, uint16_t *shunt_mohm, int32_t *vshunt_nV);
int32_t INA228_ConfigureShunt(uint8_t dev_addr, uint16_t shunt_mohm);
int32_t INA228_StartConversions(uint8_t dev_addr);
int32_t INA228_StopConversions (uint8_t dev_addr);
int32_t INA228_Deinit(uint8_t *dev_addr, uint8_t dev_count);
int32_t INA228_Write (uint8_t  dev_addr, uint8_t reg_addr, uint16_t  reg_data);
int32_t INA228_Read16(uint8_t  dev_addr, uint8_t reg_addr, uint16_t *reg_data);
int32_t INA228_Read32(uint8_t  dev_addr, uint8_t reg_addr, uint32_t *reg_data);
int32_t INA228_Read64(uint8_t  dev_addr, uint8_t reg_addr, uint64_t *reg_data);
int32_t INA228_ReadVSHUNT (uint8_t dev_addr, int32_t  *reg_data);
int32_t INA228_ReadVBUS   (uint8_t dev_addr, uint32_t *reg_data);
int32_t INA228_ReadDIETEMP(uint8_t dev_addr, int32_t  *reg_data);
int32_t INA228_ReadCURRENT(uint8_t dev_addr, int32_t  *reg_data);
int32_t INA228_ReadPOWER  (uint8_t dev_addr, uint64_t *reg_data);
int32_t INA228_ReadENERGY (uint8_t dev_addr, uint64_t *reg_data);
int32_t INA228_ReadCHARGE (uint8_t dev_addr, int64_t  *reg_data);
int32_t INA228_ClearAccumulators(uint8_t dev_addr);

#ifdef __cplusplus
}
#endif
#endif
