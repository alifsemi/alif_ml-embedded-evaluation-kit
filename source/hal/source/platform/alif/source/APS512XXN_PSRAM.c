/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/******************************************************************************
 * @file     APS512XXN_PSRAM.c
 * @author   Manoj A Murudi
 * @email    manoj.murudi@alifsemi.com
 * @version  V1.0.0
 * @date     18-Nov-2025
 * @brief    APS512XXN PSRAM init library.
 ******************************************************************************/

#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "APS512XXN_PSRAM.h"
#include "sys_ctrl_aes.h"
#include "sys_utils.h"

/**
 * @fn      void aps512xxn_global_reset(OSPI_Type *ospi, ospi_transfer_t *config)
 * @brief   APS512XXN PSRAM global reset operation.
 * @param   ospi    : Pointer to the OSPI register map.
 * @param   config  : Pointer to the RAM config struct.
 * @retval  none
 */
static void aps512xxn_global_reset(OSPI_Type *ospi, ospi_transfer_t *config)
{
    uint32_t reset_cmd;

    reset_cmd = APS512XXN_CMD_GLOBAL_RESET;

    config->addr_len       = SPI_ADDR_L_0_BIT;
    config->dummy_cycle    = APS512XXN_RESET_WAIT_CYCLES;
    config->tx_total_cnt   = 1;
    config->tx_current_cnt = 0;
    config->tx_buff        = &reset_cmd;

    ospi_send_polling(ospi, config);
}

/**
 * @fn      uint8_t aps512xxn_read_reg(OSPI_Type *ospi, uint32_t reg_addr,
                               uint8_t wait_cycles, ospi_transfer_t *config)
 * @brief   APS512XXN PSRAM reg read operation.
 * @param   ospi    : Pointer to the OSPI register map.
 * @param   reg_addr: Address of register to be read.
 * @param   wait_cycles: wait cycles needed to read reg.
 * @param   config  : Pointer to the RAM config struct.
 * @retval  reg value
 */
static uint8_t aps512xxn_read_reg(OSPI_Type *ospi, uint32_t reg_addr,
                               uint8_t wait_cycles, ospi_transfer_t *config)
{
    uint32_t cmd_addr_buff[2];
    uint16_t reg_value;

    cmd_addr_buff[0] = APS512XXN_CMD_MODE_REGISTER_READ;
    cmd_addr_buff[1] = reg_addr;

    config->addr_len       = SPI_ADDR_L_32_BIT;
    config->dummy_cycle    = wait_cycles-1;
    config->tx_total_cnt   = 2;
    config->tx_current_cnt = 0;
    config->rx_total_cnt   = 1;
    config->rx_current_cnt = 0;
    config->tx_buff        = cmd_addr_buff;
    config->rx_buff        = &reg_value;

    ospi_transfer_polling(ospi, config);

    return (uint8_t)(reg_value >> 8);
}

/**
 * @fn      void aps512xxn_write_reg(OSPI_Type *ospi, uint32_t reg_addr,
                                  uint8_t reg_val, ospi_transfer_t *config)
 * @brief   APS512XXN PSRAM reg write operation.
 * @param   ospi    : Pointer to the OSPI register map.
 * @param   reg_addr: Address of register to be read.
 * @param   reg_val : Register Value.
 * @param   config  : Pointer to the RAM config struct.
 * @retval  none
 */
static void aps512xxn_write_reg(OSPI_Type *ospi, uint32_t reg_addr,
                                  uint8_t reg_val, ospi_transfer_t *config)
{
    uint32_t write_buff[3];

    write_buff[0] = APS512XXN_CMD_MODE_REGISTER_WRITE;
    write_buff[1] = reg_addr;
    /* creating data for 16 bits as controller is in DDR mode */
    write_buff[2] = (reg_val << 8 | reg_val);

    config->addr_len       = SPI_ADDR_L_32_BIT;
    config->dummy_cycle    = APS512XXN_REG_WRITE_WAIT_CYCLES;
    config->tx_total_cnt   = 3;
    config->tx_current_cnt = 0;
    config->tx_buff        = write_buff;

    ospi_send_polling(ospi, config);
}


/**
 * @fn      int32_t aps512xxn_psram_init(OSPI_Type *ospi, AES_Type *aes)
 * @brief   APS512XXN PSRAM init function.
 * @param   ospi    : Pointer to the OSPI register map.
 * @param   aes     : Pointer to the AES register map.
 * @retval  status
 */
int32_t aps512xxn_psram_init(OSPI_Type *ospi, AES_Type *aes)
{
#if !SOC_FEAT_AES_HAS_ADDR_CTRL_SHIM
    ARG_UNUSED(aes);
#endif
    ospi_transfer_t ospi_config;
    uint8_t         reg_value, write_latency;

    switch (RTE_APS512XXN_PSRAM_WAIT_CYCLES) {
    case 3:
        write_latency = 0x0;
        break;
    case 4:
        write_latency = 0x4;
        break;
    case 5:
        write_latency = 0x2;
        break;
    case 6:
        write_latency = 0x6;
        break;
    case 7:
        write_latency = 0x1;
        break;
    default:
        return -1; /* invalid WLC */
    }

    ospi_set_dfs(ospi, APS512XXN_OSPI_REG_DFS);

    ospi_config.spi_frf        = SPI_FRF_OCTAL;
    ospi_config.ddr            = 1;
    ospi_config.inst_len       = SPI_INST_L_8_BIT;

    /* APS512XXN global reset */
    aps512xxn_global_reset(ospi, &ospi_config);

    /* APS512XXN verify ID */
    // TODO: This driver is part of the Alif DFP pack. Use the driver from pack when
    //       APS512XXN_INIT_REG_READ_WAIT_CYCLES gets corrected
    reg_value = aps512xxn_read_reg(ospi, APS512XXN_MODE_REG2_ADDR,
            5, // APS512XXN_INIT_REG_READ_WAIT_CYCLES
            &ospi_config);
    if (reg_value != APS512XXN_ID) {
        return -1;
    }

    /* config read wait cycles */
    reg_value = (0x0 << APS512XXN_MODE_REG0_LATENCY_TYPE |
                (RTE_APS512XXN_PSRAM_WAIT_CYCLES - 3) << APS512XXN_MODE_REG0_READ_LATENCY_CODE |
                 0x0 << APS512XXN_MODE_REG0_DRIVE_STR);
    aps512xxn_write_reg(ospi, APS512XXN_MODE_REG0_ADDR,
                reg_value, &ospi_config);

    /* config write wait cycles */
    reg_value = (0x0 << APS512XXN_MODE_REG4_READ_RF_RATE |
                 write_latency << APS512XXN_MODE_REG4_WRITE_LATENCY_CODE |
                 0x0 << APS512XXN_MODE_REG4_PASR);
    aps512xxn_write_reg(ospi, APS512XXN_MODE_REG4_ADDR,
                reg_value, &ospi_config);

    /* config device mode, burst len & disable hybrid burst */
    reg_value = ((RTE_APS512XXN_PSRAM_DUAL_OCTAL_MODE_ENABLE ? 0 : 1)
                     << APS512XXN_MODE_REG8_BURST_LEN |
                 0x0 << APS512XXN_MODE_REG8_BURST_TYPE |
                 0x0 << APS512XXN_MODE_REG8_RBX_READ_EN |
                 RTE_APS512XXN_PSRAM_DUAL_OCTAL_MODE_ENABLE << APS512XXN_MODE_REG8_TRANSFER_MODE);
    aps512xxn_write_reg(ospi, APS512XXN_MODE_REG8_ADDR,
            reg_value, &ospi_config);

    /* config address control shim for dual octal mode */
    if (RTE_APS512XXN_PSRAM_DUAL_OCTAL_MODE_ENABLE) {
#if SOC_FEAT_AES_HAS_ADDR_CTRL_SHIM
        aes_addr_ctrl   ram_addr_ctrl;
        /**
         *  SPI address signalling                 Corresponding HADDR/AxADDR bits in
         *
         *   --  RA12 RA4  CA7                        --  23 15  8
         *   --  RA11 RA3  CA6                        --  22 14  7
         *   --  RA10 RA2  CA5                        --  21 13  6
         *   --  RA9  RA1  CA4                        --  20 12  5
         *   --  RA8  RA0  CA3                        --  19 11  4
         *   --  RA7  --   CA2                        --  18 --  3
         *  RA14 RA6  CA9  CA1                        25  17 10  2
         *  RA13 RA5  CA8  CA0 (=0)                   24  16 9   1 (=0)
         *
         *  We massage HADDR using the shim in front of the controller to insert the gap between
         *  CA9 and RA0 - between HADDR[10] and HADDR[11].
         *  SPI controller shifts HADDR down 1 when in dual octal mode to form a 16-bit word
         *  address.
         *
         *  | Logical Byte Address | After Shift (Word Address) | Meaning            |
         *  | -------------------- | -------------------------- | ------------------ |
         *  | 0x000000             | 0x000000                   | First 16-bit word  |
         *  | 0x000002             | 0x000001                   | Second 16-bit word |
         *  | 0x000004             | 0x000002                   | Third 16-bit word  |
         *  So, instead of each address pointing to a single byte, now each address points
         *  to a 16-bit (2-byte) word.
         */

        ram_addr_ctrl.addr_lower_bits  = 11;
        ram_addr_ctrl.addr_upper_shift = 12;
        ram_addr_ctrl.ss0_array_mode_en = RTE_APS512XXN_PSRAM_OSPI_SS_LINE ? 0 : 1;
        ram_addr_ctrl.ss1_array_mode_en = RTE_APS512XXN_PSRAM_OSPI_SS_LINE ? 1 : 0;
        ram_addr_ctrl.addr_mask        = 0x7FF;

        aes_control_address(aes, &ram_addr_ctrl);
#endif
    }

    ospi_disable(ospi);
    /* make instructions length as 16 as instruction DDR is enabled */
    ospi->OSPI_XIP_INCR_INST = (APS512XXN_CMD_LINEAR_BURST_READ << 8 |
                                APS512XXN_CMD_LINEAR_BURST_READ);
    ospi->OSPI_XIP_WRAP_INST = (APS512XXN_CMD_SYNC_READ << 8 |
                                APS512XXN_CMD_SYNC_READ);
    ospi->OSPI_XIP_WRITE_INCR_INST = (APS512XXN_CMD_LINEAR_BURST_WRITE << 8 |
                                      APS512XXN_CMD_LINEAR_BURST_WRITE);
    ospi->OSPI_XIP_WRITE_WRAP_INST = (APS512XXN_CMD_SYNC_WRITE << 8 |
                                      APS512XXN_CMD_SYNC_WRITE);
    ospi->OSPI_XIP_CNT_TIME_OUT = 255;
    ospi_enable(ospi);

    return 0;
}
