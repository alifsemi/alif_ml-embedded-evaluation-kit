/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef I3C_DEV_H_
#define I3C_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/* System Includes */
#include <stdint.h>
#include "RTE_Components.h"
#include CMSIS_device_header

/* Project Includes */
#include "Driver_I3C.h"

/* Maximum 8 Slave Devices are supported (\ref register DEVICE_ADDR_TABLE_POINTER) */
#define MAX_DEVS         8

/**
\brief I3C Device Transfer
*/
struct i3c_xfer
{
  uint32_t     cmd_hi;   /* value to be programmed first  into C QUEUE */
  uint32_t     cmd_lo;   /* value to be programmed second into C QUEUE */
  uint16_t     tx_len;   /* len of data to be programmed into TX_PORT  */
  const void  *tx_buf;   /* buf address where tx data resides          */
  uint16_t     rx_len;   /* len of received data                       */
  void        *rx_buf;   /* pointer where rx data needs to be kept     */
  uint8_t      error;    /* error if any for this transfer             */
  int32_t      ret;      /* return value                               */
};

/**
\brief I3C Device Control
*/
struct i3c_ctrl
{
  ARM_I3C_SignalEvent_t  cb_event;
  ARM_I3C_STATUS         status;
  uint32_t               flags;
};

/**
\brief I3C Device Resources
*/
struct i3c_dev
{
  volatile uint32_t  baseaddr;            /* i3c base address                                   */
  IRQn_Type          irq;                 /* i3c interrupt number                               */
  uint32_t           irq_priority;        /* i3c interrupt priority                             */
  uint32_t           freepos;             /* bitmask of used addresses                          */
  uint16_t           datp;                /* DAT (Device Address Table) offset                  */
  uint16_t           maxdevs;             /* maximum number of slaves supported                 */
  uint32_t           core_clk;            /* i3c core clock frequency                           */
  uint8_t            addrs[MAX_DEVS];     /* Assigned dynamic(i3c) or static address(i2c slave) */
  struct i3c_xfer    xfer;                /* i3c transfer structure                             */
  struct i3c_ctrl   *ctrl;                /* i3c control  structure                             */
};

/* FIXME confirm i3c clock rate: 100 MHz or 125 MHz? */
#define I3C0_CLK_RATE                     100000000  /* i3c clock frequency 100 MHz                  */
#define REF_CLK_RATE                      1000000000 /* ref clock frequency 1 GHz to get core-period */

#define I3C_FLAG_INIT                     (1 << 0)   /* Driver initialized */
#define I3C_FLAG_POWER                    (1 << 1)   /* Driver power on    */

/* transaction ids for tx and rx, CCC set and get, and Dynamic Address Assignment. */
#define I3C_CCC_SET_TID                   0x1
#define I3C_CCC_GET_TID                   0x2
#define I3C_TX_TID                        0x3
#define I3C_RX_TID                        0x4
#define I3C_ADDR_ASSIGN_TID               0x5

/* common helpers */
#define GENMASK(h, l) \
                                          (((~(0U)) - ((1U) << (l)) + 1) & \
                                          (~(0U) >> (32 - 1 - (h))))

#define DIV_ROUND_UP(n, d)                (((n) + (d) - 1) / (d))

#define DEVICE_CTRL                       0x0
#define DEV_CTRL_ENABLE                   BIT(31)
#define DEV_CTRL_RESUME                   BIT(30)
#define DEV_CTRL_HOT_JOIN_NACK            BIT(8)
#define DEV_CTRL_I2C_SLAVE_PRESENT        BIT(7)

#define DEVICE_ADDR                       0x4
#define DEV_ADDR_DYNAMIC_ADDR_VALID       BIT(31)
#define DEV_ADDR_DYNAMIC(x)               (((x) << 16) & GENMASK(22, 16))

#define HW_CAPABILITY                     0x8
#define COMMAND_QUEUE_PORT                0xc
#define COMMAND_PORT_TOC                  BIT(30)
#define COMMAND_PORT_READ_TRANSFER        BIT(28)
#define COMMAND_PORT_SDAP                 BIT(27)
#define COMMAND_PORT_ROC                  BIT(26)
#define COMMAND_PORT_SPEED(x)             (((x) << 21) & GENMASK(23, 21))
#define COMMAND_PORT_DEV_INDEX(x)         (((x) << 16) & GENMASK(20, 16))
#define COMMAND_PORT_CP                   BIT(15)
#define COMMAND_PORT_CMD(x)               (((x) << 7) & GENMASK(14, 7))
#define COMMAND_PORT_TID(x)               (((x) << 3) & GENMASK(6, 3))

#define COMMAND_PORT_ARG_DATA_LEN(x)      (((x) << 16) & GENMASK(31, 16))
#define COMMAND_PORT_ARG_DATA_LEN_MAX     65536
#define COMMAND_PORT_TRANSFER_ARG         0x01

#define COMMAND_PORT_SDA_DATA_BYTE_3(x)   (((x) << 24) & GENMASK(31, 24))
#define COMMAND_PORT_SDA_DATA_BYTE_2(x)   (((x) << 16) & GENMASK(23, 16))
#define COMMAND_PORT_SDA_DATA_BYTE_1(x)   (((x) << 8) & GENMASK(15, 8))
#define COMMAND_PORT_SDA_BYTE_STRB_3      BIT(5)
#define COMMAND_PORT_SDA_BYTE_STRB_2      BIT(4)
#define COMMAND_PORT_SDA_BYTE_STRB_1      BIT(3)
#define COMMAND_PORT_SHORT_DATA_ARG       0x02

#define COMMAND_PORT_DEV_COUNT(x)         (((x) << 21) & GENMASK(25, 21))
#define COMMAND_PORT_ADDR_ASSGN_CMD       0x03

#define RESPONSE_QUEUE_PORT               0x10
#define RESPONSE_PORT_ERR_STATUS(x)       (((x) & GENMASK(31, 28)) >> 28)
#define RESPONSE_PORT_TID(x)              (((x) & GENMASK(27, 24)) >> 24)
#define RESPONSE_NO_ERROR                 0
#define RESPONSE_ERROR_CRC                1
#define RESPONSE_ERROR_PARITY             2
#define RESPONSE_ERROR_FRAME              3
#define RESPONSE_ERROR_IBA_NACK           4
#define RESPONSE_ERROR_ADDRESS_NACK       5
#define RESPONSE_ERROR_OVER_UNDER_FLOW    6
#define RESPONSE_ERROR_TRANSF_ABORT       8
#define RESPONSE_ERROR_I2C_W_NACK_ERR     9
#define RESPONSE_PORT_TID(x)              (((x) & GENMASK(27, 24)) >> 24)
#define RESPONSE_PORT_DATA_LEN(x)         ((x) & GENMASK(15, 0))

#define RX_TX_DATA_PORT                   0x14
#define IBI_QUEUE_STATUS                  0x18
#define QUEUE_THLD_CTRL                   0x1c
#define QUEUE_THLD_CTRL_RESP_BUF_MASK     GENMASK(15, 8)
#define QUEUE_THLD_CTRL_RESP_BUF(x)       (((x) - 1) << 8)

#define DATA_BUFFER_THLD_CTRL             0x20
#define DATA_BUFFER_THLD_CTRL_RX_BUF      GENMASK(11, 8)

#define IBI_QUEUE_CTRL                    0x24
#define IBI_MR_REQ_REJECT                 0x2C
#define IBI_SIR_REQ_REJECT                0x30
#define IBI_REQ_REJECT_ALL                GENMASK(31, 0)

#define RESET_CTRL                        0x34
#define RESET_CTRL_IBI_QUEUE              BIT(5)
#define RESET_CTRL_RX_FIFO                BIT(4)
#define RESET_CTRL_TX_FIFO                BIT(3)
#define RESET_CTRL_RESP_QUEUE             BIT(2)
#define RESET_CTRL_CMD_QUEUE              BIT(1)
#define RESET_CTRL_SOFT                   BIT(0)

#define SLV_EVENT_CTRL                    0x38
#define INTR_STATUS                       0x3c
#define INTR_STATUS_EN                    0x40
#define INTR_SIGNAL_EN                    0x44
#define INTR_FORCE                        0x48
#define INTR_BUSOWNER_UPDATE_STAT         BIT(13)
#define INTR_IBI_UPDATED_STAT             BIT(12)
#define INTR_READ_REQ_RECV_STAT           BIT(11)
#define INTR_DEFSLV_STAT                  BIT(10)
#define INTR_TRANSFER_ERR_STAT            BIT(9)
#define INTR_DYN_ADDR_ASSGN_STAT          BIT(8)
#define INTR_CCC_UPDATED_STAT             BIT(6)
#define INTR_TRANSFER_ABORT_STAT          BIT(5)
#define INTR_RESP_READY_STAT              BIT(4)
#define INTR_CMD_QUEUE_READY_STAT         BIT(3)
#define INTR_IBI_THLD_STAT                BIT(2)
#define INTR_RX_THLD_STAT                 BIT(1)
#define INTR_TX_THLD_STAT                 BIT(0)
#define INTR_ALL                          (INTR_BUSOWNER_UPDATE_STAT  |   \
                                          INTR_IBI_UPDATED_STAT       |   \
                                          INTR_READ_REQ_RECV_STAT     |   \
                                          INTR_DEFSLV_STAT            |   \
                                          INTR_TRANSFER_ERR_STAT      |   \
                                          INTR_DYN_ADDR_ASSGN_STAT    |   \
                                          INTR_CCC_UPDATED_STAT       |   \
                                          INTR_TRANSFER_ABORT_STAT    |   \
                                          INTR_RESP_READY_STAT        |   \
                                          INTR_CMD_QUEUE_READY_STAT   |   \
                                          INTR_IBI_THLD_STAT          |   \
                                          INTR_TX_THLD_STAT           |   \
                                          INTR_RX_THLD_STAT)

#define INTR_MASTER_MASK                  (INTR_TRANSFER_ERR_STAT     |   \
                                          INTR_RESP_READY_STAT)

#define QUEUE_STATUS_LEVEL                0x4c
#define QUEUE_STATUS_IBI_STATUS_CNT(x)    (((x) & GENMASK(28, 24)) >> 24)
#define QUEUE_STATUS_IBI_BUF_BLR(x)       (((x) & GENMASK(23, 16)) >> 16)
#define QUEUE_STATUS_LEVEL_RESP(x)        (((x) & GENMASK(15, 8)) >> 8)
#define QUEUE_STATUS_LEVEL_CMD(x)         ((x) & GENMASK(7, 0))

#define DATA_BUFFER_STATUS_LEVEL          0x50
#define DATA_BUFFER_STATUS_LEVEL_TX(x)    ((x) & GENMASK(7, 0))

#define PRESENT_STATE                     0x54
#define CCC_DEVICE_STATUS                 0x58

#define DEVICE_ADDR_TABLE_POINTER         0x5c
#define DEVICE_ADDR_TABLE_DEPTH(x)        (((x) & GENMASK(31, 16)) >> 16)
#define DEVICE_ADDR_TABLE_ADDR(x)         ((x) & GENMASK(7, 0))

#define DEV_CHAR_TABLE_POINTER            0x60
#define VENDOR_SPECIFIC_REG_POINTER       0x6c
#define SLV_PID_VALUE                     0x74
#define SLV_CHAR_CTRL                     0x78
#define SLV_MAX_LEN                       0x7c
#define MAX_READ_TURNAROUND               0x80
#define MAX_DATA_SPEED                    0x84
#define SLV_DEBUG_STATUS                  0x88
#define SLV_INTR_REQ                      0x8c
#define DEVICE_CTRL_EXTENDED              0xb0
#define SCL_I3C_OD_TIMING                 0xb4
#define SCL_I3C_PP_TIMING                 0xb8
#define SCL_I3C_TIMING_HCNT(x)            (((x) << 16) & GENMASK(23, 16))
#define SCL_I3C_TIMING_LCNT(x)            ((x) & GENMASK(7, 0))
#define SCL_I3C_TIMING_CNT_MIN            5

#define SCL_I2C_FM_TIMING                 0xbc
#define SCL_I2C_FM_TIMING_HCNT(x)         (((x) << 16) & GENMASK(31, 16))
#define SCL_I2C_FM_TIMING_LCNT(x)         ((x) & GENMASK(15, 0))

#define SCL_I2C_FMP_TIMING                0xc0
#define SCL_I2C_FMP_TIMING_HCNT(x)        (((x) << 16) & GENMASK(23, 16))
#define SCL_I2C_FMP_TIMING_LCNT(x)        ((x) & GENMASK(15, 0))

#define SCL_EXT_LCNT_TIMING               0xc8
#define SCL_EXT_LCNT_4(x)                 (((x) << 24) & GENMASK(31, 24))
#define SCL_EXT_LCNT_3(x)                 (((x) << 16) & GENMASK(23, 16))
#define SCL_EXT_LCNT_2(x)                 (((x) << 8) & GENMASK(15, 8))
#define SCL_EXT_LCNT_1(x)                 ((x) & GENMASK(7, 0))

#define SCL_EXT_TERMN_LCNT_TIMING         0xcc
#define BUS_FREE_TIMING                   0xd4
#define BUS_I3C_MST_FREE(x)               ((x) & GENMASK(15, 0))

#define BUS_IDLE_TIMING                   0xd8
#define I3C_VER_ID                        0xe0
#define I3C_VER_TYPE                      0xe4
#define EXTENDED_CAPABILITY               0xe8
#define SLAVE_CONFIG                      0xec

#define DEV_ADDR_TABLE_LEGACY_I2C_DEV     BIT(31)
#define DEV_ADDR_TABLE_DYNAMIC_ADDR(x)    (((x) << 16) & GENMASK(23, 16))
#define DEV_ADDR_TABLE_STATIC_ADDR(x)     ((x) & GENMASK(6, 0))
#define DEV_ADDR_TABLE_LOC(start, idx)    ((start) + ((idx) << 2))

#define I3C_BUS_SDR1_SCL_RATE             8000000
#define I3C_BUS_SDR2_SCL_RATE             6000000
#define I3C_BUS_SDR3_SCL_RATE             4000000
#define I3C_BUS_SDR4_SCL_RATE             2000000
#define I3C_BUS_I2C_SS_TLOW_MIN_NS        4700
#define I3C_BUS_I2C_FM_TLOW_MIN_NS        1300
#define I3C_BUS_I2C_FMP_TLOW_MIN_NS       500
#define I3C_BUS_THIGH_MAX_NS              41

#ifdef __cplusplus
}
#endif

#endif /* I3C_DEV_H_ */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
