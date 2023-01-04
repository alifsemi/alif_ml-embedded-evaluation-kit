/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/* System Includes */
#include <string.h>
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header

/* Project Includes */
#include "I3C_dev.h"

#if !(RTE_I3C0)
#error "I3C is not enabled in the RTE_Device.h"
#endif

#if (defined(RTE_Drivers_I3C0) && !RTE_I3C0)
#error "I3C0 not configured in RTE_Device.h!"
#endif

#define ARM_I3C_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0) /* driver version */


/* I3C0 Driver Instance */
#if (RTE_I3C0)

/* I3C0 Device Control */
static struct i3c_ctrl i3c0_ctrl;

/* I3C0 Device Resources */
static struct i3c_dev i3c0 =
{
  .baseaddr     = (volatile uint32_t) I3C0_BASE,
  .core_clk     = I3C0_CLK_RATE,
  .irq          = (IRQn_Type) I3C0_IRQ,
  .irq_priority = RTE_I3C0_IRQ_PRI,
  .ctrl         = &i3c0_ctrl,
};
#endif /* RTE_I3C0 */


/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion =
{
  ARM_I3C_API_VERSION,
  ARM_I3C_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_I3C_CAPABILITIES DriverCapabilities =
{
  1,   /* Supports legacy i2c device */
  0    /* Reserved (must be zero)    */
};

/**
  \fn           uint32_t i3c_read_reg(struct i3c_dev *dev, uint32_t offset)
  \brief        Read value from i3c register offset
  \param[in]    dev     : Pointer to i3c resources structure
  \param[in]    offset  : Register offset
  \return       i3c register value from given offset
*/
static uint32_t i3c_read_reg(struct i3c_dev *dev, uint32_t offset)
{
  return *((volatile uint32_t *) (dev->baseaddr + offset));
}

/**
  \fn           void i3c_write_reg(struct i3c_dev *dev, uint32_t offset, uint32_t val)
  \brief        Write value to i3c register offset
  \param[in]    dev     : Pointer to i3c resources structure
  \param[in]    offset  : Register offset
  \param[in]    val     : Value needs to be written to register offset
  \return       none
*/
static void i3c_write_reg(struct i3c_dev *dev, uint32_t offset, uint32_t val)
{
  *((volatile uint32_t *) (dev->baseaddr + offset)) = val;
}

/**
  \fn           int i3c_master_get_addr_pos(struct i3c_dev *dev, uint8_t addr)
  \brief        Get already assigned slave address position from DAT(Device Address Table).
                For i3c, dynamic address and for i2c, static address is used for communication.
  \param[in]    dev     : Pointer to i3c resources structure
  \param[in]    addr    : Assigned Slave Address;
                           Dynamic Address for i3c, Static Address for i2c slave device
  \return       Assigned slave address position from DAT(Device Address Table) index OR
                ARM_DRIVER_ERROR in case slave is not already assigned.
*/
static int i3c_master_get_addr_pos(struct i3c_dev *dev, uint8_t addr)
{
  int pos;

  for (pos = 0; pos < dev->maxdevs; pos++)
  {
    if (addr == dev->addrs[pos])
      return pos;
  }

  return ARM_DRIVER_ERROR;
}

/**
  \fn           int i3c_master_get_free_pos(struct i3c_dev *dev)
  \brief        Get free position from DAT(Device Address Table)
  \param[in]    dev     : Pointer to i3c resources structure
  \return       Free position from DAT OR
                ARM_DRIVER_ERROR in case DAT is Full.
                Maximum 8 Slave Devices are supported (\ref register DEVICE_ADDR_TABLE_POINTER)
*/
static int i3c_master_get_free_pos(struct i3c_dev *dev)
{
  int i;

  if (!(dev->freepos & GENMASK(dev->maxdevs - 1, 0)))
    return ARM_DRIVER_ERROR;

  for (i = 0; i < dev->maxdevs; i++)
  {
    if (dev->freepos & (1 << i))
      return i;
  }

  return ARM_DRIVER_ERROR;
}

/**
  \fn           void i3c_master_wr_tx_fifo(struct i3c_dev *dev, const uint8_t *bytes, int nbytes)
  \brief        Write data to i3c TX FIFO
  \param[in]    dev     : Pointer to i3c resources structure
  \param[in]    bytes   : Pointer to buffer with data which needs to be write to i3c transmitter
  \param[in]    nbytes  : Number of bytes needs to be write
  \return       none
*/
static void i3c_master_wr_tx_fifo(struct i3c_dev  *dev,
                                  const  uint8_t  *bytes,
                                         uint32_t  nbytes)
{
  uint32_t len_in_words = nbytes / 4;
  uint32_t i, tmp;

  /* FIXME Add check for MAX TX FIFO Length */
  for (i = 0; i < len_in_words; i++)
  {
    /* FIXME Fix Memory Alignment issue */
    tmp = *((uint32_t *) bytes);
    i3c_write_reg(dev, RX_TX_DATA_PORT, tmp);
    bytes += 4;
  }

  /* write the remaining bytes in the last incomplete word */
  if (nbytes & 3)
  {
    tmp = 0;

    memcpy(&tmp, bytes, nbytes & 3);
    i3c_write_reg(dev, RX_TX_DATA_PORT, tmp);
  }
}

/**
  \fn           void i3c_master_read_rx_fifo(struct i3c_dev *dev, uint8_t *bytes, int nbytes)
  \brief        Read data from i3c RX FIFO
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    bytes    : Pointer to buffer for data to receive from i3c RX FIFO
  \param[in]    nbytes   : Number of bytes needs to be receive
  \return       none
*/
static void i3c_master_read_rx_fifo(struct i3c_dev  *dev,
                                           uint8_t  *bytes,
                                           uint32_t  nbytes)
{
  uint32_t len_in_words = nbytes / 4;
  uint32_t i, tmp;

  /* FIXME Add check for MAX RX FIFO Length */
  for (i = 0; i < len_in_words; i++)
  {
    /* FIXME Fix Memory Alignment issue */
    *((uint32_t *) bytes) = i3c_read_reg(dev, RX_TX_DATA_PORT);
    bytes += 4;
  }

  /* read the last word and copy the actual remaining data */
  if (nbytes & 3)
  {
    tmp = i3c_read_reg(dev, RX_TX_DATA_PORT);
    memcpy(bytes, &tmp, nbytes & 3);
  }
}

/**
  \fn           void i3c_enqueue_xfer(struct i3c_dev *dev)
  \brief        Add commands to i3c Command Queue
  \param[in]    dev      : Pointer to i3c resources structure
  \return       none
*/
static void i3c_enqueue_xfer(struct i3c_dev *dev)
{
  uint32_t thld_ctrl;

  /* write data to tx port (if any) */
  if (dev->xfer.tx_buf)
    i3c_master_wr_tx_fifo(dev, dev->xfer.tx_buf, dev->xfer.tx_len);

  thld_ctrl = i3c_read_reg(dev, QUEUE_THLD_CTRL);
  thld_ctrl &= ~QUEUE_THLD_CTRL_RESP_BUF_MASK;
  /* set up for an interrupt after one response */
  thld_ctrl |= QUEUE_THLD_CTRL_RESP_BUF(1);
  i3c_write_reg(dev, QUEUE_THLD_CTRL, thld_ctrl);

  i3c_write_reg(dev, COMMAND_QUEUE_PORT, dev->xfer.cmd_hi);
  i3c_write_reg(dev, COMMAND_QUEUE_PORT, dev->xfer.cmd_lo);
}

/**
  \fn           int i3c_clk_cfg(struct i3c_dev *dev)
  \brief        i3c clock configuration for i3c slave device
  \param[in]    dev      : Pointer to i3c resources structure
  \return       \ref execution_status
*/
static int i3c_clk_cfg(struct i3c_dev *dev)
{
  unsigned long core_rate, core_period;
  uint32_t scl_timing;
  uint8_t  hcnt, lcnt;

  core_rate = dev->core_clk;

  if (!core_rate)
    return ARM_DRIVER_ERROR;

  core_period = DIV_ROUND_UP(REF_CLK_RATE, core_rate);
  hcnt = DIV_ROUND_UP(I3C_BUS_THIGH_MAX_NS, core_period) - 1;
  if (hcnt < SCL_I3C_TIMING_CNT_MIN)
    hcnt = SCL_I3C_TIMING_CNT_MIN;

  lcnt = DIV_ROUND_UP(core_rate, I3C_BUS_TYP_I3C_SCL_RATE) - hcnt;

  if (lcnt < SCL_I3C_TIMING_CNT_MIN)
    lcnt = SCL_I3C_TIMING_CNT_MIN;

  scl_timing = SCL_I3C_TIMING_HCNT(hcnt) | SCL_I3C_TIMING_LCNT(lcnt);
  i3c_write_reg(dev, SCL_I3C_PP_TIMING, scl_timing);

  if (!(i3c_read_reg(dev, DEVICE_CTRL) & DEV_CTRL_I2C_SLAVE_PRESENT))
    i3c_write_reg(dev, BUS_FREE_TIMING, BUS_I3C_MST_FREE(lcnt));

  lcnt = DIV_ROUND_UP(I3C_BUS_TLOW_OD_MIN_NS, core_period);
  scl_timing = SCL_I3C_TIMING_HCNT(hcnt) | SCL_I3C_TIMING_LCNT(lcnt);
  i3c_write_reg(dev, SCL_I3C_OD_TIMING, scl_timing);

  lcnt = DIV_ROUND_UP(core_rate, I3C_BUS_SDR1_SCL_RATE) - hcnt;
  scl_timing = SCL_EXT_LCNT_1(lcnt);

  lcnt = DIV_ROUND_UP(core_rate, I3C_BUS_SDR2_SCL_RATE) - hcnt;
  scl_timing |= SCL_EXT_LCNT_2(lcnt);

  lcnt = DIV_ROUND_UP(core_rate, I3C_BUS_SDR3_SCL_RATE) - hcnt;
  scl_timing |= SCL_EXT_LCNT_3(lcnt);

  lcnt = DIV_ROUND_UP(core_rate, I3C_BUS_SDR4_SCL_RATE) - hcnt;
  scl_timing |= SCL_EXT_LCNT_4(lcnt);

  i3c_write_reg(dev, SCL_EXT_LCNT_TIMING, scl_timing);

  return ARM_DRIVER_OK;
}

/**
  \fn           int i2c_clk_cfg(struct i3c_dev *dev, uint32_t i2c_speed_mode)
  \brief        i3c clock configuration for legacy i2c slave device
  \param[in]    dev             : Pointer to i3c resources structure
  \param[in]    i2c_speed_mode  : Speed mode
                 I3C_BUS_MODE_MIXED_FAST_I2C_FMP_SPEED_1_MBPS  : Fast Mode Plus 1   MBPS
                 I3C_BUS_MODE_MIXED_FAST_I2C_FM_SPEED_400_KBPS : Fast Mode      400 KBPS
                 I3C_BUS_MODE_MIXED_SLOW_I2C_SS_SPEED_100_KBPS : Standard Mode  100 KBPS
  \return        \ref execution_status
*/
static int i2c_clk_cfg(struct i3c_dev *dev, uint32_t i2c_speed_mode)
{
  unsigned long core_rate, core_period;
  uint16_t hcnt, lcnt;
  uint32_t scl_timing;

  core_rate = dev->core_clk;
  if (!core_rate)
    return ARM_DRIVER_ERROR;

  core_period = DIV_ROUND_UP(REF_CLK_RATE, core_rate);

  /* Speed Mode: Fast Mode Plus >1 MBPS (approximately 3.124 MBPS) */
  if(i2c_speed_mode == I3C_BUS_MODE_MIXED_FAST_I2C_FMP_SPEED_1_MBPS)
  {
    lcnt = DIV_ROUND_UP(I3C_BUS_I2C_FMP_TLOW_MIN_NS, core_period);
    hcnt = DIV_ROUND_UP(core_rate, I3C_BUS_I2C_FM_PLUS_SCL_RATE) - lcnt;

    scl_timing = SCL_I2C_FMP_TIMING_HCNT(hcnt) |
                 SCL_I2C_FMP_TIMING_LCNT(lcnt);

    i3c_write_reg(dev, SCL_I2C_FMP_TIMING, scl_timing);
  }

  /* Speed Mode: Fast Mode 400 KBPS */
  if(i2c_speed_mode == I3C_BUS_MODE_MIXED_FAST_I2C_FM_SPEED_400_KBPS)
  {
    lcnt = DIV_ROUND_UP(I3C_BUS_I2C_FM_TLOW_MIN_NS, core_period);
    hcnt = DIV_ROUND_UP(core_rate, I3C_BUS_I2C_FM_SCL_RATE) - lcnt;

    scl_timing = SCL_I2C_FM_TIMING_HCNT(hcnt) |
                 SCL_I2C_FM_TIMING_LCNT(lcnt);

    i3c_write_reg(dev, SCL_I2C_FM_TIMING, scl_timing);
  }

  /* Speed Mode: Standard Mode 100 KBPS */
  if(i2c_speed_mode == I3C_BUS_MODE_MIXED_SLOW_I2C_SS_SPEED_100_KBPS)
  {
    lcnt = DIV_ROUND_UP(I3C_BUS_I2C_SS_TLOW_MIN_NS, core_period);
    hcnt = DIV_ROUND_UP(core_rate, I3C_BUS_I2C_SS_SCL_RATE) - lcnt;

    scl_timing = SCL_I2C_FM_TIMING_HCNT(hcnt) |
                 SCL_I2C_FM_TIMING_LCNT(lcnt);

    i3c_write_reg(dev, SCL_I2C_FM_TIMING, scl_timing);
  }

  i3c_write_reg(dev, BUS_FREE_TIMING, BUS_I3C_MST_FREE(lcnt));

  /* Set as legacy i2c device is present. */
  i3c_write_reg(dev, DEVICE_CTRL,
                i3c_read_reg(dev, DEVICE_CTRL) | DEV_CTRL_I2C_SLAVE_PRESENT);

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t i3c_configure_control_reg(bool enable)
  \brief        i3c EXPMST0 control register configuration
                 This function will configure
                  - if enable
                    - enable Bypass Clock gating
                    - set i3c output enable polarity: inverted
                    - enable i3c module
                          or
                  - if disable
                    - disable i3c module
  \param[in]    enable  : enable/disable i3c module
  \return       \ref execution_status
*/
static int32_t i3c_configure_control_reg(bool enable)
{

/* i3c EXPMST0 control offset */
#define I3C_EXPMST0_CTRL_OFFSET      0x24

  /* i3c EXPMST0 control configuration settings */
  __IOM uint32_t *i3c_expmst0_ctrl_reg = (uint32_t *) (CFGMST0_BASE + I3C_EXPMST0_CTRL_OFFSET);

  if(enable)
  {
    /*
     * Enable Bypass Clock gating.
     *
     * EXPMST0 Control Register: Clock Configuration
     * ip_clk_force -> [4] (1: force peripherals function clocks on, bypass clock gating)
     * pclk_force   -> [0] (1: force APB bus clocks on, bypass clock gating)
     */
    __IOM uint32_t *expmst0_ctrl_reg = (uint32_t *) CFGMST0_BASE;
    *expmst0_ctrl_reg |= ( (1 << 0) | (1 << 4) );

    /*
     * Enable i3c from EXPMST0 Control Register
     * i3c output enable pol  -> [24] (1: inverted output enable)
     * i3c module enable      -> [0]  (1: module enable)
     */
    *i3c_expmst0_ctrl_reg |= ( (1 << 0) | (1 << 24) );
  }
  else /* disable */
  {
    /* Disable i3c module. */
    *i3c_expmst0_ctrl_reg &= ( ~(1 << 0) );
  }

  return ARM_DRIVER_OK;
}

/**
  \fn           int i3c_master_init(struct i3c_dev *dev)
  \brief        Initialize i3c master.
                 This function will :
                  - Free all the position from DAT(Device Address Table)
                  - Clear Command Queue and Data buffer Queue
                  - Enable interrupt for Response Queue Ready and
                                         Transfer error status
                  - Enable i3c controller
  \param[in]    dev      : Pointer to i3c resources structure
  \return       \ref execution_status
*/
static int i3c_master_init(struct i3c_dev *dev)
{
  uint32_t val;

  /* Do clock configuration here, we might need additional param for bus mode */

  val = i3c_read_reg(dev, DEVICE_ADDR_TABLE_POINTER);
  dev->datp = val;
  dev->maxdevs = val >> 16;
  dev->freepos = GENMASK(dev->maxdevs - 1, 0);

  val = i3c_read_reg(dev, QUEUE_THLD_CTRL);
  val &= ~QUEUE_THLD_CTRL_RESP_BUF_MASK;
  i3c_write_reg(dev, QUEUE_THLD_CTRL, val);

  val = i3c_read_reg(dev, DATA_BUFFER_THLD_CTRL);
  val &= ~DATA_BUFFER_THLD_CTRL_RX_BUF;
  i3c_write_reg(dev, DATA_BUFFER_THLD_CTRL, val);

  i3c_write_reg(dev, INTR_STATUS, INTR_ALL);
  i3c_write_reg(dev, INTR_STATUS_EN, INTR_MASTER_MASK);
  i3c_write_reg(dev, INTR_SIGNAL_EN, INTR_MASTER_MASK);

  /* set first non reserved address as the master's DA */
  i3c_write_reg(dev, DEVICE_ADDR,
                DEV_ADDR_DYNAMIC_ADDR_VALID | DEV_ADDR_DYNAMIC(0x08));

  /* reject all ibis */
  i3c_write_reg(dev, IBI_SIR_REQ_REJECT, IBI_REQ_REJECT_ALL);
  i3c_write_reg(dev, IBI_MR_REQ_REJECT, IBI_REQ_REJECT_ALL);

  i3c_write_reg(dev, DEVICE_CTRL,
                i3c_read_reg(dev, DEVICE_CTRL) | DEV_CTRL_HOT_JOIN_NACK);

  /* Enable i3c controller. */
  i3c_write_reg(dev, DEVICE_CTRL,
                i3c_read_reg(dev, DEVICE_CTRL) | DEV_CTRL_ENABLE);

  return ARM_DRIVER_OK;
}

/**
  \fn           void i3c_isr(struct i3c_dev *dev)
  \brief        i3c interrupt service routine
  \param[in]    dev      : Pointer to i3c resources structure
  \return       none
*/
void i3c_isr(struct i3c_dev *dev)
{
  uint32_t status, nresp, resp, tid;
  uint32_t event = 0;

  status = i3c_read_reg(dev, INTR_STATUS);

  if (!(status & i3c_read_reg(dev, INTR_STATUS_EN)))
  {
    /* there are no interrupts that we are interested in */
    i3c_write_reg(dev, INTR_STATUS, INTR_ALL);
    return;
  }

  /* we are only interested in a response int. make sure we have a response */
  nresp = i3c_read_reg(dev, QUEUE_STATUS_LEVEL);
  nresp = QUEUE_STATUS_LEVEL_RESP(nresp);

  if (!nresp)
    return;

  resp = i3c_read_reg(dev, RESPONSE_QUEUE_PORT);

  dev->xfer.rx_len = RESPONSE_PORT_DATA_LEN(resp);
  dev->xfer.error = RESPONSE_PORT_ERR_STATUS(resp);

  tid = RESPONSE_PORT_TID(resp);

  switch (tid)
  {
    case I3C_TX_TID:
    case I3C_CCC_SET_TID:
      if (dev->xfer.error)
        event |= ARM_I3C_EVENT_TRANSFER_ERROR;
      else
        event |= ARM_I3C_EVENT_TRANSFER_DONE;
      break;

    case I3C_RX_TID:
    case I3C_CCC_GET_TID:
      if (dev->xfer.rx_len && !dev->xfer.error)
      {
        i3c_master_read_rx_fifo(dev, dev->xfer.rx_buf,
                dev->xfer.rx_len);
        event |= ARM_I3C_EVENT_TRANSFER_DONE;
      }
      else if (dev->xfer.error)
      {
        event |= ARM_I3C_EVENT_TRANSFER_ERROR;
      }
      break;

    case I3C_ADDR_ASSIGN_TID:
      if (dev->xfer.error)
        event |= ARM_I3C_EVENT_TRANSFER_ERROR;
      else
        event |= ARM_I3C_EVENT_TRANSFER_DONE;
      break;
  }

  dev->ctrl->status.busy = 0;
  if (event && dev->ctrl->cb_event)
    dev->ctrl->cb_event(event);

  if (dev->xfer.error)
  {
    /* error: Resume i3c controller and clear error status. */
    i3c_write_reg(dev, DEVICE_CTRL,
                  i3c_read_reg(dev, DEVICE_CTRL) | DEV_CTRL_RESUME);

    i3c_write_reg(dev, INTR_STATUS, INTR_TRANSFER_ERR_STAT);
  }
}

/**
  \fn           int i3c_ccc_get(struct i3c_dev *dev, I3C_CMD *ccc)
  \brief        Get CCC (Common Command Codes)
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    ccc      : Pointer to i3c command structure
  \return       \ref execution_status
*/
static int i3c_ccc_get(struct i3c_dev *dev, I3C_CMD *ccc)
{
  int32_t index = i3c_master_get_addr_pos(dev, ccc->addr);

  if (index < 0)
    return ARM_DRIVER_ERROR;

  dev->xfer.tx_buf = NULL;
  dev->xfer.tx_len = 0;
  dev->xfer.rx_buf = ccc->data;
  dev->xfer.rx_len = ccc->len;

  dev->xfer.cmd_hi = COMMAND_PORT_ARG_DATA_LEN(ccc->len) |
                     COMMAND_PORT_TRANSFER_ARG;

  dev->xfer.cmd_lo = COMMAND_PORT_READ_TRANSFER        |
                     COMMAND_PORT_CP                   |
                     COMMAND_PORT_TID(I3C_CCC_GET_TID) |
                     COMMAND_PORT_DEV_INDEX(index)     |
                     COMMAND_PORT_CMD(ccc->cmd_id)     |
                     COMMAND_PORT_TOC                  |
                     COMMAND_PORT_ROC;

  i3c_enqueue_xfer(dev);
  return ARM_DRIVER_OK;
}

/**
  \fn           int i3c_ccc_set(struct i3c_dev *dev, I3C_CMD *ccc)
  \brief        Set CCC (Common Command Codes)
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    ccc      : Pointer to i3c command structure
  \return       \ref execution_status
*/
static int i3c_ccc_set(struct i3c_dev *dev, I3C_CMD *ccc)
{
  int32_t index = i3c_master_get_addr_pos(dev, ccc->addr);

  if (index < 0)
    return ARM_DRIVER_ERROR;

  dev->xfer.rx_buf = NULL;
  dev->xfer.rx_len = 0;
  dev->xfer.tx_buf = ccc->data;
  dev->xfer.tx_len = ccc->len;

  dev->xfer.cmd_hi = COMMAND_PORT_ARG_DATA_LEN(ccc->len) |
                     COMMAND_PORT_TRANSFER_ARG;

  dev->xfer.cmd_lo = COMMAND_PORT_CP                   |
                     COMMAND_PORT_TID(I3C_CCC_SET_TID) |
                     COMMAND_PORT_DEV_INDEX(index)     |
                     COMMAND_PORT_CMD(ccc->cmd_id)     |
                     COMMAND_PORT_TOC                  |
                     COMMAND_PORT_ROC;

  i3c_enqueue_xfer(dev);
  return ARM_DRIVER_OK;
}

/* ARM_DRIVER_I3C public interface functions */

/**
  \fn           ARM_DRIVER_VERSION I3C_GetVersion(void)
  \brief        Get i3c driver version
  \return       i3c driver version
*/
static ARM_DRIVER_VERSION I3C_GetVersion(void)
{
  return DriverVersion;
}

/**
  \fn           ARM_I3C_CAPABILITIES I3C_GetCapabilities(void)
  \brief        Get i3c driver capabilities
  \return       i3c driver capabilities
*/
static ARM_I3C_CAPABILITIES I3C_GetCapabilities(void)
{
  return DriverCapabilities;
}

/**
  \fn           int I3Cx_MasterSendCommand(struct i3c_dev *dev, I3C_CMD *ccc)
  \brief        Send an I3C command to the slave
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    ccc      : Pointer to i3c command structure
  \return       \ref execution_status
*/
static int I3Cx_MasterSendCommand(struct i3c_dev *dev, I3C_CMD *ccc)
{
  int ret = 0;

  if (!(dev->ctrl->flags & I3C_FLAG_POWER))
    return ARM_DRIVER_ERROR;

  if (dev->ctrl->status.busy)
    return ARM_DRIVER_ERROR_BUSY;

  if (!ccc)
    return ARM_DRIVER_ERROR_PARAMETER;

  if (ccc->cmd_id == I3C_CCC_ENTDAA)
    return ARM_DRIVER_ERROR_PARAMETER;

  dev->ctrl->status.busy = 1;

  if (ccc->rw)
    ret = i3c_ccc_get(dev, ccc);
  else
    ret = i3c_ccc_set(dev, ccc);

  if (ret < 0)
    return ARM_DRIVER_ERROR;

  return ARM_DRIVER_OK;
}

/**
  \fn           int I3Cx_MasterReceive(struct i3c_dev *dev,  uint8_t  addr,
                                              uint8_t *data, uint16_t len)
  \brief        Read data from the slave
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    addr     : Assigned Slave Address;
                            Dynamic Address for i3c, Static Address for i2c slave device
  \param[in]    data     : Pointer to buffer for data to receive from slave
  \param[in]    len      : Number of bytes needs to be receive
  \return       \ref execution_status
*/
static int I3Cx_MasterReceive(struct i3c_dev *dev,  uint8_t  addr,
                                     uint8_t *data, uint16_t len)
{
  int32_t index;

  if (!(dev->ctrl->flags & I3C_FLAG_POWER))
    return ARM_DRIVER_ERROR;

  index = i3c_master_get_addr_pos(dev, addr);

  if (index < 0)
    return ARM_DRIVER_ERROR_PARAMETER;

  if (!data || !len)
    return ARM_DRIVER_ERROR_PARAMETER;

  if (dev->ctrl->status.busy)
    return ARM_DRIVER_ERROR_BUSY;

  dev->ctrl->status.busy = 1;

  dev->xfer.cmd_hi = COMMAND_PORT_ARG_DATA_LEN(len) |
          COMMAND_PORT_TRANSFER_ARG;

  dev->xfer.rx_buf = data;
  dev->xfer.rx_len = len;
  dev->xfer.tx_buf = NULL;
  dev->xfer.tx_len = 0;
  dev->xfer.cmd_lo = COMMAND_PORT_READ_TRANSFER  |
         COMMAND_PORT_SPEED(0)                   |
         COMMAND_PORT_TID(I3C_RX_TID)            |
         COMMAND_PORT_DEV_INDEX(index)           |
         COMMAND_PORT_ROC                        |
         COMMAND_PORT_TOC;

  /* Add commands to i3c Command Queue */
  i3c_enqueue_xfer(dev);

  return ARM_DRIVER_OK;
}

/**
  \fn           int I3Cx_MasterTransmit(struct i3c_dev *dev,  uint8_t  addr,
                                        const u int8_t *data, uint16_t len)
  \brief        Write data to the slave
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    addr     : Assigned Slave Address;
                            Dynamic Address for i3c, Static Address for i2c slave device
  \param[in]    data     : Pointer to buffer with data which needs to be transmit to slave
  \param[in]    len      : Number of bytes needs to be transmit
  \return       \ref execution_status
*/
static int I3Cx_MasterTransmit(struct i3c_dev *dev,  uint8_t  addr,
                               const  uint8_t *data, uint16_t len)
{
  int32_t index;

  if (!(dev->ctrl->flags & I3C_FLAG_POWER))
    return ARM_DRIVER_ERROR;

  index = i3c_master_get_addr_pos(dev, addr);

  if (index < 0)
    return ARM_DRIVER_ERROR_PARAMETER;

  if (!data || !len)
    return ARM_DRIVER_ERROR_PARAMETER;

  if (dev->ctrl->status.busy)
    return ARM_DRIVER_ERROR_BUSY;

  dev->ctrl->status.busy = 1;

  dev->xfer.cmd_hi = COMMAND_PORT_ARG_DATA_LEN(len) |
          COMMAND_PORT_TRANSFER_ARG;

  dev->xfer.rx_buf = NULL;
  dev->xfer.rx_len = 0;
  dev->xfer.tx_buf = data;
  dev->xfer.tx_len = len;
  dev->xfer.cmd_lo = COMMAND_PORT_SPEED(0)         |
                     COMMAND_PORT_TID(I3C_TX_TID)  |
                     COMMAND_PORT_DEV_INDEX(index) |
                     COMMAND_PORT_ROC              |
                     COMMAND_PORT_TOC;

  /* Add commands to i3c Command Queue */
  i3c_enqueue_xfer(dev);

  return ARM_DRIVER_OK;
}

/**
  \fn           int I3Cx_MasterAssignDA(struct i3c_dev *dev, uint8_t *dyn_addr, uint8_t sta_addr)
  \brief        Assign dynamic address to the i3c slave using SETDASA;
                Note: Only required for i3c slave devices;
                      i2c slave device uses static address for communication \ref I3Cx_AttachI2Cdev.
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    dyn_addr : Pointer to char where dynamic address will be copied.
  \param[in]    sta_addr : Static address of i3c device
  \return       \ref execution_status
*/
static int I3Cx_MasterAssignDA(struct i3c_dev *dev, uint8_t *dyn_addr, uint8_t sta_addr)
{
  uint32_t cmd, val;
  int32_t  pos;

  if (!dyn_addr || !sta_addr)
    return ARM_DRIVER_ERROR_PARAMETER;

  if (!(dev->ctrl->flags & I3C_FLAG_POWER))
    return ARM_DRIVER_ERROR;

  if (dev->ctrl->status.busy)
    return ARM_DRIVER_ERROR_BUSY;

  dev->ctrl->status.busy = 1;

  /* Find the first unused index in freepos, note that this also
   * corresponds to the first unused location in the DAT
   */
  pos = i3c_master_get_free_pos(dev);

  /* the dat is full */
  if (pos < 0) {
    dev->ctrl->status.busy = 0;
    return ARM_DRIVER_ERROR;
  }

  /* reserve the index */
  dev->freepos &= ~(BIT(pos));
  *dyn_addr = pos + 0x09; /* we start assigning addresses from 0x09 */
  dev->addrs[pos] = *dyn_addr;

  /* ok, we have space in the dat, program the dat in index pos */
  val = DEV_ADDR_TABLE_DYNAMIC_ADDR(*dyn_addr);
  val |= DEV_ADDR_TABLE_STATIC_ADDR(sta_addr);

  i3c_write_reg(dev, dev->datp + (pos * 4), val);

  /* Issue the SETDASA command */
  cmd = COMMAND_PORT_DEV_INDEX(pos)        |
        COMMAND_PORT_CMD(I3C_CCC_SETDASA)  |
        COMMAND_PORT_TOC                   |
        COMMAND_PORT_ROC                   |
        COMMAND_PORT_DEV_COUNT(1)          |
        COMMAND_PORT_ADDR_ASSGN_CMD        |
        COMMAND_PORT_TID(I3C_ADDR_ASSIGN_TID);

  i3c_write_reg(dev, COMMAND_QUEUE_PORT, cmd);

  return ARM_DRIVER_OK;
}

/**
  \fn           int I3Cx_AttachI2Cdev(struct i3c_dev *dev, uint8_t sta_addr)
  \brief        Attach legacy i2c device to the i3c bus.
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    sta_addr : Static address of legacy i2c device
  \return       \ref execution_status
*/
static int I3Cx_AttachI2Cdev(struct i3c_dev *dev, uint8_t sta_addr)
{
  uint32_t val = 0;
  int32_t  pos;

  if (!sta_addr)
    return ARM_DRIVER_ERROR_PARAMETER;

  if (!(dev->ctrl->flags & I3C_FLAG_POWER))
    return ARM_DRIVER_ERROR;

  /* Find the first unused index in freepos, note that this also
   * corresponds to the first unused location in the DAT(Device Address Table)
   */
  pos = i3c_master_get_free_pos(dev);

  /* DAT(Device Address Table) is full? */
  if (pos < 0)
  {
    /* error: DAT is full */
    return ARM_DRIVER_ERROR;
  }

  /* reserve the index */
  dev->freepos &= ~(BIT(pos));

  /* use static address for communication. */
  dev->addrs[pos] = sta_addr;

  /* ok, we have space in the DAT, store the static address and
   * mark as i2c legacy device is present.
   */
  val = DEV_ADDR_TABLE_STATIC_ADDR(sta_addr) | DEV_ADDR_TABLE_LEGACY_I2C_DEV;

  /* Program the DAT(device address table) in index pos. */
  i3c_write_reg(dev, dev->datp + (pos * 4), val);

  return ARM_DRIVER_OK;
}

/**
  \fn           int I3Cx_Detachdev(struct i3c_dev *dev, uint8_t addr)
  \brief        Detach already attached i2c/i3c device from the i3c bus.
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    addr     : Static  address of already attached i2c device
                                        OR
                           Dynamic address of already attached i3c device
  \return       \ref execution_status
*/
static int I3Cx_Detachdev(struct i3c_dev *dev, uint8_t addr)
{
  uint32_t val;
  int32_t  pos;

  if (!addr)
    return ARM_DRIVER_ERROR_PARAMETER;

  if (!(dev->ctrl->flags & I3C_FLAG_POWER))
    return ARM_DRIVER_ERROR;

  /* Get already attached i2c device address index in DAT (device address table). */
  pos = i3c_master_get_addr_pos(dev, addr);

  /* i2c dev is not attached to DAT? */
  if (pos < 0)
  {
    /* err: i2c slave device is not attached to DAT,
     * first attach i2c device \ref I3Cx_AttachI2Cdev */
    return ARM_DRIVER_ERROR;
  }

  /* free the index */
  dev->freepos |= (BIT(pos));
  dev->addrs[pos] = 0;

  /* clear the DAT index pos */
  val = 0;
  i3c_write_reg(dev, dev->datp + (pos * 4), val);

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t I3Cx_MasterControl(struct i3c_dev *dev, uint32_t control, uint32_t arg)
  \brief        Control i3c master.
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    control  : Operation
  \param[in]    arg      : Argument of operation
  \return       \ref execution_status
*/
static int32_t I3Cx_MasterControl(struct i3c_dev *dev, uint32_t control, uint32_t arg)
{
  int ret;

  if (!(dev->ctrl->flags & I3C_FLAG_POWER))
    return ARM_DRIVER_ERROR;

  switch(control)
  {
  case I3C_MASTER_SET_BUS_MODE:
    switch(arg)
    {
    case I3C_BUS_MODE_MIXED_FAST_I2C_FMP_SPEED_1_MBPS:
    case I3C_BUS_MODE_MIXED_FAST_I2C_FM_SPEED_400_KBPS:
    case I3C_BUS_MODE_MIXED_SLOW_I2C_SS_SPEED_100_KBPS:
    case I3C_BUS_MODE_MIXED_LIMITED:

      /* i2c clock configuration for selected Speed mode. */
      ret = i2c_clk_cfg(dev,arg);
      if (ret)
        return ret;

    /* fall through */
    case I3C_BUS_MODE_PURE:

      /* i3c clock configuration */
      ret = i3c_clk_cfg(dev);
      if (ret)
        return ret;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    break;

  default:
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ret;
}

/**
  \fn           int32_t  I3Cx_Initialize(struct i3c_dev *dev, ARM_I3C_SignalEvent_t cb_event)
  \brief        Initialize the i3c device.
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    cb_event : Pointer to I3C Event \ref ARM_I3C_SignalEvent_t
  \return       \ref execution_status
*/
static int32_t  I3Cx_Initialize(struct i3c_dev *dev, ARM_I3C_SignalEvent_t cb_event)
{
  if (dev->ctrl->flags & I3C_FLAG_INIT)
    return ARM_DRIVER_OK;

  /* Initialize I3C pads here */
  dev->ctrl->cb_event = cb_event;
  dev->ctrl->flags |= I3C_FLAG_INIT;
  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t I3Cx_Uninitialize(struct i3c_dev  *dev)
  \brief        Uninitialize the i3c device
  \param[in]    dev      : Pointer to i3c resources structure
  \return       \ref execution_status
*/
static int32_t I3Cx_Uninitialize(struct i3c_dev  *dev)
{
  dev->ctrl->flags = 0;
  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t I3Cx_PowerControl(struct i3c_dev *dev, ARM_POWER_STATE state)
  \brief        i3c power control
  \param[in]    dev      : Pointer to i3c resources structure
  \param[in]    state    : Power state
  \return       \ref execution_status
*/
static int32_t I3Cx_PowerControl(struct i3c_dev *dev, ARM_POWER_STATE state)
{
  int ret;

  switch (state)
  {
    case ARM_POWER_OFF:

      /* Disable i3c IRQ */
      NVIC_DisableIRQ(dev->irq);

      /* Clear Any Pending i3c IRQ */
      NVIC_ClearPendingIRQ(dev->irq);

      /* i3c EXPMST0 control configuration: Disable i3c Module. */
      ret = i3c_configure_control_reg(false);

      dev->ctrl->flags &= ~I3C_FLAG_POWER;
      break;

    case ARM_POWER_FULL:

      if (!(dev->ctrl->flags & I3C_FLAG_INIT))
        return ARM_DRIVER_ERROR;

      if (dev->ctrl->flags & I3C_FLAG_POWER)
        break;

      /* i3c EXPMST0 control configuration: Enable i3c Module. */
      ret = i3c_configure_control_reg(true);

      /* Initialize and Enable i3c master */
      ret = i3c_master_init(dev);
      if (ret < 0)
        return ARM_DRIVER_ERROR;

      /* Enable i3c IRQ */
      NVIC_ClearPendingIRQ(dev->irq);
      NVIC_SetPriority(dev->irq, dev->irq_priority);
      NVIC_EnableIRQ(dev->irq);

      dev->ctrl->flags |= I3C_FLAG_POWER;
      break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  return ARM_DRIVER_OK;
}


/* I3C0 Driver Instance */
#if (RTE_I3C0)

/* wrapper functions for I3C0 */
static int32_t I3C0_Initialize(ARM_I3C_SignalEvent_t cb_event)
{
  return (I3Cx_Initialize(&i3c0, cb_event));
}

static int32_t I3C0_Uninitialize(void)
{
  return (I3Cx_Uninitialize(&i3c0));
}

static int32_t I3C0_PowerControl(ARM_POWER_STATE state)
{
  return (I3Cx_PowerControl(&i3c0, state));
}

static int32_t I3C0_MasterTransmit(uint8_t addr, const uint8_t *data, uint16_t len)
{
  return (I3Cx_MasterTransmit(&i3c0, addr, data, len));
}

static int32_t I3C0_MasterReceive(uint8_t addr, uint8_t *data, uint16_t len)
{
  return (I3Cx_MasterReceive(&i3c0, addr, data, len));
}

static int32_t I3C0_MasterSendCommand(I3C_CMD *ccc)
{
  return (I3Cx_MasterSendCommand(&i3c0, ccc));
}

static int32_t I3C0_MasterControl(uint32_t control, uint32_t arg)
{
  return (I3Cx_MasterControl(&i3c0, control, arg));
}

static int32_t I3C0_MasterAssignDA(uint8_t *dyn_addr, uint8_t sta_addr)
{
  return (I3Cx_MasterAssignDA(&i3c0, dyn_addr, sta_addr));
}

static int32_t I3C0_AttachI2Cdev(uint8_t sta_addr)
{
  return (I3Cx_AttachI2Cdev(&i3c0, sta_addr));
}

static int32_t I3C0_Detachdev(uint8_t addr)
{
  return (I3Cx_Detachdev(&i3c0, addr));
}

void I3C0_IRQHandler(void)
{
  i3c_isr(&i3c0);
}


/* I3C0 Driver Control Block */
extern ARM_DRIVER_I3C Driver_I3C0;
ARM_DRIVER_I3C Driver_I3C0 =
{
  I3C_GetVersion,
  I3C_GetCapabilities,
  I3C0_Initialize,
  I3C0_Uninitialize,
  I3C0_PowerControl,
  I3C0_MasterTransmit,
  I3C0_MasterReceive,
  I3C0_MasterControl,
  I3C0_MasterSendCommand,
  I3C0_MasterAssignDA,
  I3C0_AttachI2Cdev,
  I3C0_Detachdev
};
#endif /* RTE_I3C0 */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
