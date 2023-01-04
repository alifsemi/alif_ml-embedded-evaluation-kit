/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef DRIVER_I3C_H_
#define DRIVER_I3C_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include "Driver_Common.h"

#define ARM_I3C_API_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0)     /* API version */

/****** I3C Control Codes *****/

/* I3C Control Codes: Bus mode */
#define I3C_MASTER_SET_BUS_MODE                         (1UL << 0)  ///< Set bus mode to pure i3c, mixed i3c + i2c fast etc.

/* I3C Control Codes: Bus mode arguments */
#define I3C_BUS_MODE_PURE                               (0x00UL)    ///< Pure i3c device
#define I3C_BUS_MODE_MIXED_FAST_I2C_FMP_SPEED_1_MBPS    (0x01UL)    ///< Mixed i3c + i2c device, Speed: Fast Mode Plus   1 MBPS
#define I3C_BUS_MODE_MIXED_FAST_I2C_FM_SPEED_400_KBPS   (0x02UL)    ///< Mixed i3c + i2c device, Speed: Fast Mode      400 KBPS
#define I3C_BUS_MODE_MIXED_SLOW_I2C_SS_SPEED_100_KBPS   (0x03UL)    ///< Mixed i3c + i2c device, Speed: Standard Mode  100 KBPS
#define I3C_BUS_MODE_MIXED_LIMITED                      (0x04UL)

#define I3C_BUS_MAX_DEVS                                11

/* Clock rates and periods */
#define I3C_BUS_MAX_I3C_SCL_RATE                        12900000
#define I3C_BUS_TYP_I3C_SCL_RATE                        12500000
#define I3C_BUS_I2C_FM_PLUS_SCL_RATE                    1000000
#define I3C_BUS_I2C_FM_SCL_RATE                         400000
#define I3C_BUS_I2C_SS_SCL_RATE                         100000
#define I3C_BUS_TLOW_OD_MIN_NS                          200

/****** I3C Event *****/
#define ARM_I3C_EVENT_TRANSFER_DONE                     (1UL << 0)  ///< Master Transmit/Receive finished
#define ARM_I3C_EVENT_TRANSFER_ERROR                    (1UL << 1)  ///< Master Transmit/Receive Error

/* I3C CCC (Common Command Codes) related definitions */
#define I3C_CCC_DIRECT                                  BIT(7)

#define I3C_CCC_ID(id, broadcast)                       ((id) | ((broadcast) ? 0 : I3C_CCC_DIRECT))

/* Commands valid in both broadcast and unicast modes */
#define I3C_CCC_ENEC(broadcast)                         I3C_CCC_ID(0x0, broadcast)
#define I3C_CCC_DISEC(broadcast)                        I3C_CCC_ID(0x1, broadcast)
#define I3C_CCC_ENTAS(as, broadcast)                    I3C_CCC_ID(0x2 + (as), broadcast)
#define I3C_CCC_RSTDAA(broadcast)                       I3C_CCC_ID(0x6, broadcast)
#define I3C_CCC_SETMWL(broadcast)                       I3C_CCC_ID(0x9, broadcast)
#define I3C_CCC_SETMRL(broadcast)                       I3C_CCC_ID(0xa, broadcast)
#define I3C_CCC_SETXTIME(broadcast)                     ((broadcast) ? 0x28 : 0x98)
#define I3C_CCC_VENDOR(id, broadcast)                   ((id) + ((broadcast) ? 0x61 : 0xe0))

/* Broadcast-only commands */
#define I3C_CCC_ENTDAA                                  I3C_CCC_ID(0x7, true)
#define I3C_CCC_DEFSLVS                                 I3C_CCC_ID(0x8, true)
#define I3C_CCC_ENTTM                                   I3C_CCC_ID(0xb, true)
#define I3C_CCC_ENTHDR(x)                               I3C_CCC_ID(0x20 + (x), true)

/* Unicast-only commands */
#define I3C_CCC_SETDASA                                 I3C_CCC_ID(0x7, false)
#define I3C_CCC_SETNEWDA                                I3C_CCC_ID(0x8, false)
#define I3C_CCC_GETMWL                                  I3C_CCC_ID(0xb, false)
#define I3C_CCC_GETMRL                                  I3C_CCC_ID(0xc, false)
#define I3C_CCC_GETPID                                  I3C_CCC_ID(0xd, false)
#define I3C_CCC_GETBCR                                  I3C_CCC_ID(0xe, false)
#define I3C_CCC_GETDCR                                  I3C_CCC_ID(0xf, false)
#define I3C_CCC_GETSTATUS                               I3C_CCC_ID(0x10, false)
#define I3C_CCC_GETACCMST                               I3C_CCC_ID(0x11, false)
#define I3C_CCC_SETBRGTGT                               I3C_CCC_ID(0x13, false)
#define I3C_CCC_GETMXDS                                 I3C_CCC_ID(0x14, false)
#define I3C_CCC_GETHDRCAP                               I3C_CCC_ID(0x15, false)
#define I3C_CCC_GETXTIME                                I3C_CCC_ID(0x19, false)

#define I3C_CCC_EVENT_SIR                               BIT(0)
#define I3C_CCC_EVENT_MR                                BIT(1)
#define I3C_CCC_EVENT_HJ                                BIT(3)

/**
\brief I3C Command
*/
typedef struct _I3C_CMD {
  uint8_t   rw;
  uint8_t   cmd_id;
  uint16_t  len;
  uint8_t  *data;
  uint8_t   addr;
} I3C_CMD;

/**
\brief I3C Status
*/
typedef struct _ARM_I3C_STATUS {
  uint32_t busy     :1;
  uint32_t reserved :31;
} ARM_I3C_STATUS;

// Function documentation
/**
  \fn          ARM_DRIVER_VERSION GetVersion (void)
  \brief       Get i3c driver version.
  \return      \ref ARM_DRIVER_VERSION

  \fn          ARM_I3C_CAPABILITIES GetCapabilities (void)
  \brief       Get i3c driver capabilities.
  \return      \ref ARM_I3C_CAPABILITIES

  \fn          int32_t Initialize (ARM_I3C_SignalEvent_t cb_event)
  \brief       Initialize I3C Interface.
  \param[in]   cb_event  Pointer to \ref ARM_I3C_SignalEvent
  \return      \ref execution_status

  \fn          int32_t Uninitialize (void)
  \brief       De-initialize I3C Interface.
  \return      \ref execution_status

  \fn          int32_t PowerControl (ARM_POWER_STATE state)
  \brief       Control I3C Interface Power.
  \param[in]   state     Power state
  \return      \ref execution_status

  \fn          int32_t MasterTransmit (uint8_t addr, const uint8_t *data, uint16_t len)
  \brief       Start transmitting data as I3C Master.
  \param[in]   addr      Assigned Slave Address;
                          Dynamic Address for i3c, Static Address for i2c slave device
  \param[in]   data      Pointer to buffer with data to transmit to I3C Slave
  \param[in]   len       Number of data bytes to transmit
  \return      \ref execution_status

  \fn          int32_t MasterReceive (uint8_t addr, uint8_t *data, uint16_t len)
  \brief       Start receiving data as I3C Master.
  \param[in]   addr      Assigned Slave Address;
                          Dynamic Address for i3c, Static Address for i2c slave device
  \param[out]  data      Pointer to buffer for data to receive from I3C Slave
  \param[in]   len       Number of data bytes to receive
  \return      \ref execution_status

  \fn          int32_t Control (uint32_t control, uint32_t arg)
  \brief       Control I3C Interface.
  \param[in]   control   Operation
  \param[in]   arg       Argument of operation (optional)
  \return      \ref execution_status

  \fn          int32_t MasterSendCommand (I3C_CMD cmd)
  \brief       Send a I3C Command
  \param[in]   cmd       The I3C_CMD to be sent
  \return      \ref execution status

  \fn          int32_t MasterAssignDA (uint8_t *dyn_addr, uint8_t sta_addr)
  \brief       Assign dynamic address to the i3c slave using SETDASA
               Note: Only required for i3c slave devices;
                     i2c slave device uses static address for communication \ref I3Cx_AttachI2Cdev.
  \param[in]   dyn_addr  Pointer to char where dynamic address will be copied.
  \param[in]   sta_addr  Static address of i3c device
  \return      \ref execution status

  \fn          int32_t AttachI2Cdev (uint8_t sta_addr)
  \brief       Attach legacy i2c device to the i3c bus.
  \param[in]   sta_addr  static address of i2c device
  \return      \ref execution_status

  \fn          int32_t Detachdev (uint8_t addr)
  \brief       Detach already attached i2c/i3c device from the i3c bus.
  \param[in]   addr      static  address of already attached i2c device
                                 OR
                         dynamic address of already attached i3c device
  \return      \ref execution_status
*/

typedef void (*ARM_I3C_SignalEvent_t) (uint32_t event);  ///< Pointer to \ref I3C_SignalEvent : Signal I3C Event.


/**
\brief I3C Driver Capabilities.
*/
typedef struct _ARM_I3C_CAPABILITIES {
  uint32_t legacy_i2c_dev   : 1;           ///< Supports legacy i2c device
  uint32_t reserved         : 31;          ///< Reserved (must be zero)
} ARM_I3C_CAPABILITIES;


/**
\brief Access structure of the I3C Driver.
*/
typedef struct _ARM_DRIVER_I3C {
  ARM_DRIVER_VERSION   (*GetVersion)        (void);                                             ///< Pointer to \ref GetVersion        : Get I3C driver version.
  ARM_I3C_CAPABILITIES (*GetCapabilities)   (void);                                             ///< Pointer to \ref GetCapabilities   : Get I3C driver capabilities.
  int32_t              (*Initialize)        (ARM_I3C_SignalEvent_t cb_event);                   ///< Pointer to \ref Initialize        : Initialize    I3C Interface.
  int32_t              (*Uninitialize)      (void);                                             ///< Pointer to \ref Uninitialize      : De-initialize I3C Interface.
  int32_t              (*PowerControl)      (ARM_POWER_STATE state);                            ///< Pointer to \ref PowerControl      : Control I3C Interface Power.
  int32_t              (*MasterTransmit)    (uint8_t addr, const uint8_t *data, uint16_t len);  ///< Pointer to \ref MasterTransmit    : Start transmitting data as I3C Master.
  int32_t              (*MasterReceive)     (uint8_t addr, uint8_t *data, uint16_t len);        ///< Pointer to \ref MasterReceive     : Start receiving    data as I3C Master.
  int32_t              (*Control)           (uint32_t control, uint32_t arg);                   ///< Pointer to \ref Control           : Control I3C Interface.
  int32_t              (*MasterSendCommand) (I3C_CMD *cmd);                                     ///< Pointer to \ref MasterSendCommand : Send I3C CCC
  int32_t              (*MasterAssignDA)    (uint8_t *dyn_addr, uint8_t sta_addr);              ///< Pointer to \ref MasterAssignDA    : Assign I3C Dynamic Address
  int32_t              (*AttachI2Cdev)      (uint8_t sta_addr);                                 ///< Pointer to \ref AttachI2Cdev      : Attach legacy i2c device to the i3c bus.
  int32_t              (*Detachdev)         (uint8_t addr);                                     ///< Pointer to \ref DetachI2Cdev      : Detach already attached i2c/i3c device from the i3c bus.
} const ARM_DRIVER_I3C;


#ifdef  __cplusplus
}
#endif

#endif /* DRIVER_I3C_H_ */
