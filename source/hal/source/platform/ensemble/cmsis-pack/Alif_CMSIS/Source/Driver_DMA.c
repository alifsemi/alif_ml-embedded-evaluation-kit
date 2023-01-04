/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/**************************************************************************//**
 * @file     Driver_DMA.c
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     04-Nov-2020
 * @brief    CMSIS-Driver for DMA.
 * @bug      None.
 * @Note     None
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "DMA_dev.h"
#include "string.h"

#define ARM_DMA_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(2, 1) /*!< DMA Driver Version */

static const ARM_DRIVER_VERSION DriverVersion = {
        ARM_DMA_API_VERSION,
        ARM_DMA_DRV_VERSION
};


#if !((RTE_DMA0) || (RTE_DMA1) || (RTE_DMA2))
#error "DMA is not enabled in the RTE_Device.h"
#endif

#if !defined(RTE_Drivers_DMA)
#error "DMA not defined in RTE_Components.h!"
#endif

/* Driver Capabilities */
static const ARM_DMA_CAPABILITIES DriverCapabilities = {
    1,   /* supports memory to memory operation */
    1,   /* supports memory to peripheral operation */
    1,   /* supports peripheral to memory operation */
    0,   /* supports Scatter Gather */
    1,   /* supports Secure/Non-Secure mode operation */
    0    /* reserved (must be zero) */
};

#if (RTE_DMA0)
static DMA_CONFIG_INFO DMA0_CONFIG = {

};

static DMA_DRV_INFO DMA0 = {
    .cfg        = &DMA0_CONFIG,
    .apb_intf   = RTE_DMA0_APB_INTERFACE,
    .intf_paddr = NULL,
    .flags      = NULL,
    .irq_start  = DMA0_IRQ0,
    .abort_irq_priority = RTE_DMA0_ABORT_IRQ_PRI,
};
#endif

#if (RTE_DMA1)
static DMA_CONFIG_INFO DMA1_CONFIG = {

};

static DMA_DRV_INFO DMA1 = {
    .cfg        = &DMA1_CONFIG,
    .apb_intf   = RTE_DMA1_APB_INTERFACE,
    .intf_paddr = NULL,
    .flags      = NULL,
    .irq_start  = DMA1_IRQ0,
    .abort_irq_priority = RTE_DMA1_ABORT_IRQ_PRI,
};
#endif

#if (RTE_DMA2)
static DMA_CONFIG_INFO DMA2_CONFIG = {

};

static DMA_DRV_INFO DMA2 = {
    .cfg        = &DMA2_CONFIG,
    .apb_intf   = RTE_DMA2_APB_INTERFACE,
    .intf_paddr = NULL,
    .flags      = NULL,
    .irq_start  = DMA2_IRQ0,
    .abort_irq_priority = RTE_DMA2_ABORT_IRQ_PRI,
};
#endif

/**
  \fn          ARM_DRIVER_VERSION DMA_GetVersion (void)
  \brief       Get DMA driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION DMA_GetVersion (void)
{
  return DriverVersion;
}

/**
  \fn          ARM_DMA_CAPABILITIES DMA_GetCapabilities (void)
  \brief       Get DMA driver capabilities
  \return      \ref ARM_DMA_CAPABILITIES
*/
static ARM_DMA_CAPABILITIES DMA_GetCapabilities (void)
{
  return DriverCapabilities;
}

/**
  \fn          void DMA_EnableInterrupt (DMA_THREAD_INFO *thread,
                                         DMA_DRV_INFO *dma)
  \brief       Enable the particular interrupt
  \param[in]   thread Pointer to DMA thread resources
  \param[in]   dma    Pointer to DMA resources
*/
__STATIC_INLINE void DMA_EnableInterrupt (DMA_THREAD_INFO *thread,
                                          DMA_DRV_INFO *dma)
{
    DMA_CHANNEL_INFO *channel_info = &thread->channel_info;

    dma->intf_paddr->INTEN |= (1 << channel_info->evnt_index);
}

/**
  \fn          void DMA_DisableInterrupt (DMA_THREAD_INFO *thread,
                                          DMA_DRV_INFO *dma)
  \brief       Disable the particular interrupt
  \param[in]   thread Pointer to DMA thread resources
  \param[in]   dma    Pointer to DMA resources
*/
__STATIC_INLINE void DMA_DisableInterrupt (DMA_THREAD_INFO *thread,
                                           DMA_DRV_INFO *dma)
{
    DMA_CHANNEL_INFO *channel_info = &thread->channel_info;

    dma->intf_paddr->INTEN &= ~(1 << channel_info->evnt_index);
}

/**
  \fn          uint32_t DMA_GetEventInterruptStatus (DMA_DRV_INFO *dma)
  \brief       Get status of event & interrupts
  \param[in]   dma    Pointer to DMA resources
  \return      uint32_t raw interrupt event-status
*/
uint32_t DMA_GetEventInterruptStatus (DMA_DRV_INFO *dma)
{
    return dma->intf_paddr->INT_EVENT_RIS;
}

/**
  \fn          void DMA_ClearInterrupt (DMA_THREAD_INFO *thread, DMA_DRV_INFO *dma)
  \brief       Clear the particular interrupt
  \param[in]   thread Pointer to DMA thread resources
  \param[in]   dma    Pointer to DMA resources
*/
__STATIC_INLINE void DMA_ClearInterrupt (DMA_THREAD_INFO *thread, DMA_DRV_INFO *dma)
{
    DMA_CHANNEL_INFO *channel_info = &thread->channel_info;

    dma->intf_paddr->INTCLR = 1 << channel_info->evnt_index;
}

/**
  \fn          DMA_SECURE_STATUS DMA_GetManagerSecurityStatus (DMA_DRV_INFO *dma)
  \brief       Get Security Current status of the Manager
  \param[in]   dma    Pointer to DMA resources
  \return      DMA_SECURE_STATUS security status
*/
DMA_SECURE_STATUS DMA_GetManagerSecurityStatus (DMA_DRV_INFO *dma)
{
    return _FLD2VAL(DMA_DSR_DNS, dma->intf_paddr->DSR);
}

/**
  \fn          bool DMA_GetManagerFaultStatus (DMA_DRV_INFO *dma)
  \brief       Get Fault status of Manager thread
  \param[in]   dma    Pointer to DMA resources
  \return      bool manager fault status
*/
bool DMA_GetManagerFaultStatus (DMA_DRV_INFO *dma)
{
    return _FLD2VAL(DMA_FSRD_FAULT_STATUS_MANAGER, dma->intf_paddr->FSRD);
}

/**
  \fn          uint8_t DMA_GetChannelFaultStatus (DMA_DRV_INFO *dma)
  \brief       Get Fault status of Channels
  \param[in]   dma Pointer to DMA resources
  \return      uint8_t Channel fault status
*/
__STATIC_INLINE uint8_t DMA_GetChannelFaultStatus (DMA_DRV_INFO *dma)
{
    return _FLD2VAL(DMA_FSRC_FAULT_STATUS, dma->intf_paddr->FSRC);
}

/**
  \fn          uint8_t DMA_GetChannelStatus (uint8_t channel, DMA_DRV_INFO *dma)
  \brief       Get Current status of the Channel
  \param[in]   channel Channel Number
  \param[in]   dma    Pointer to DMA resources
  \return      uint8_t channel status
*/
__STATIC_INLINE uint8_t DMA_GetChannelStatus (uint8_t channel, DMA_DRV_INFO *dma)
{
    return _FLD2VAL(DMA_CSR_CHANNEL_STATUS,
                    dma->intf_paddr->CHANNEL_RT_INFO[channel].CSR);
}

/**
  \fn          DMA_SECURE_STATUS DMA_GetChannelSecurityStatus (uint8_t channel,
                                                               DMA_DRV_INFO *dma)
  \brief       Get Security Current status of the Channel
  \param[in]   channel Channel Number
  \param[in]   dma    Pointer to DMA resources
  \return      DMA_SECURE_STATUS security status
*/
DMA_SECURE_STATUS DMA_GetChannelSecurityStatus (uint8_t channel,
                                                DMA_DRV_INFO *dma)
{
    return _FLD2VAL(DMA_CSR_CHANNEL_NONSEC,
                    dma->intf_paddr->CHANNEL_RT_INFO[channel].CSR);
}

/**
  \fn          uint32_t DMA_GetChannelSrcAddress (uint8_t channel,
                                                  DMA_DRV_INFO *dma)
  \brief       Get Source Address of the Channel
  \param[in]   channel Channel Number
  \param[in]   dma    Pointer to DMA resources
  \return      uint32_t Current Source Address
*/
__STATIC_INLINE uint32_t DMA_GetChannelSrcAddress (uint8_t channel,
                                                   DMA_DRV_INFO *dma)
{
    return dma->intf_paddr->RT_CHANNEL_CFG[channel].SAR;
}

/**
  \fn          uint32_t DMA_GetChannelDestAddress (uint8_t channel,
                                                   DMA_DRV_INFO *dma)
  \brief       Get Destination Address of the Channel
  \param[in]   channel Channel Number
  \param[in]   dma    Pointer to DMA resources
  \return      uint32_t Current Destination Address
*/
__STATIC_INLINE uint32_t DMA_GetChannelDestAddress (uint8_t channel,
                                                    DMA_DRV_INFO *dma)
{
    return dma->intf_paddr->RT_CHANNEL_CFG[channel].DAR;
}

/**
  \fn          bool DMA_GetDbgStatus (DMA_DRV_INFO *dma)
  \brief       Get Debug Status
  \param[in]   dma  Pointer to DMA resources
  \return      bool Debug Idle/Busy Status
*/
__STATIC_INLINE bool DMA_GetDbgStatus (DMA_DRV_INFO *dma)
{
    return _FLD2VAL(DMA_DBGSTATUS_DBGSTATUS, dma->intf_paddr->DBGCMD);
}

/**
  \fn          void DMA_ExecuteCmd (DMA_DRV_INFO *dma)
  \brief       Execute command in DBGINST register
  \param[in]   dma  Pointer to DMA resources
  \return      void
*/
__STATIC_INLINE void DMA_ExecuteCmd (DMA_DRV_INFO *dma)
{
    dma->intf_paddr->DBGCMD = _VAL2FLD(DMA_DBGCMD_DBGCMD, 0U);
}

/**
  \fn          void DMA_WriteDbgInst0 (uint32_t dbginst0, DMA_DRV_INFO *dma)
  \brief       Write DBGINST0 register
  \param[in]   dbginst0  Debug Instruction0 value
  \param[in]   dma  Pointer to DMA resources
  \return      void
*/
__STATIC_INLINE void DMA_WriteDbgInst0 (uint32_t dbginst0, DMA_DRV_INFO *dma)
{
    dma->intf_paddr->DBGINST0 = dbginst0;
}

/**
  \fn          void DMA_WriteDbgInst1 (uint32_t dbginst1, DMA_DRV_INFO *dma)
  \brief       Write DBGINST0 register
  \param[in]   dbginst1  Debug Instruction1 value
  \param[in]   dma  Pointer to DMA resources
  \return      void
*/
__STATIC_INLINE void DMA_WriteDbgInst1 (uint32_t dbginst1, DMA_DRV_INFO *dma)
{
    dma->intf_paddr->DBGINST1 = dbginst1;
}

/**
  \fn          int8_t DMA_GetFreeChannel (DMA_CONFIG_INFO *dma_cfg)
  \brief       Return free channel if available
  \param[in]   dma_cfg  Pointer to DMA Configuration resources
  \return      int8_t channel number or -1 if not available
*/
__STATIC_INLINE int8_t DMA_GetFreeChannel (DMA_CONFIG_INFO *dma_cfg)
{
    int8_t cnt;

    for (cnt = 0; cnt < DMA_MAX_CHANNELS; cnt++)
    {
        if (dma_cfg->channel_thread[cnt].in_use == false)
        {
            dma_cfg->channel_thread[cnt].in_use = true;
            return cnt;
        }
    }

    return -1;
}


/**
  \fn          int8_t DMA_ReleaseChannel (int8_t chnl, DMA_CONFIG_INFO *dma_cfg)
  \brief       Release the channel
  \param[in]   chnl  Channel Number
  \param[in]   dma_cfg  Pointer to DMA Configuration resources
  \return      void
*/
__STATIC_INLINE void DMA_ReleaseChannel (int8_t chnl, DMA_CONFIG_INFO *dma_cfg)
{
    dma_cfg->channel_thread[chnl].in_use = false;
}

/**
  \fn          int8_t DMA_ReleaseEvent (int8_t chnl, int8_t evnt_idx,
                                        DMA_CONFIG_INFO *dma_cfg)
  \brief       Release the event
  \param[in]   chnl  Channel Number
  \param[in]   evnt_idx  Event Number
  \param[in]   dma_cfg  Pointer to DMA Configuration resources
  \return      void
*/
__STATIC_INLINE void DMA_ReleaseEvent (int8_t chnl,
                                       int8_t evnt_idx,
                                       DMA_CONFIG_INFO *dma_cfg)
{
    dma_cfg->channel_thread[chnl].channel_info.evnt_index = -1;
    dma_cfg->event[evnt_idx] = -1;
}

/**
  \fn          int8_t DMA_GetFreeEvent (DMA_CONFIG_INFO *dma_cfg)
  \brief       Return free channel if available
  \param[in]   dma_cfg  Pointer to DMA Configuration resources
  \return      int8_t channel number or -1 if not available
*/
__STATIC_INLINE int8_t DMA_GetFreeEvent (DMA_CONFIG_INFO *dma_cfg)
{
    int8_t cnt;

    for (cnt = 0; cnt < DMA_MAX_EVENTS; cnt++)
    {
        if (dma_cfg->event[cnt] == -1)
        {
            return cnt;
        }
    }

    return -1;
}

/**
  \fn          int32_t DMA_CopyDesc (ARM_DMA_PARAMS *params, DMA_DESC_INFO *dma_desc)
  \brief       Copy the descriptor information
  \param[in]   params  DMA parameters set from application
  \param[in]   dma_desc  Channel descriptor information
  \return      \ref execution_status
*/
__STATIC_INLINE int32_t DMA_CopyDesc (ARM_DMA_PARAMS *params,
                                      DMA_DESC_INFO *dma_desc)
{
    if (((1 << params->burst_size) > DMA_MAX_BURST_SIZE) ||
        (params->burst_len > DMA_MAX_BURST_LEN) ||
        (!params->burst_len) ||
        (params->peri_reqno >= DMA_MAX_PERI_REQ) ||
        (params->dir >= ARM_DMA_DIR_NOT_SET) ||
        (!params->cb_event) ||
        (!params->num_bytes))
        return ARM_DRIVER_ERROR_PARAMETER;

    dma_desc->direction = params->dir;
    dma_desc->dst_addr  = LocalToGlobal(params->dst_addr);
    dma_desc->src_addr  = LocalToGlobal(params->src_addr);
    dma_desc->dst_blen  = params->burst_len;
    dma_desc->src_blen  = params->burst_len;
    dma_desc->peri_num  = params->peri_reqno;
    dma_desc->total_len = params->num_bytes;
    dma_desc->cb_event  = params->cb_event;

    dma_desc->dst_bsize = params->burst_size;

    if(params->dir == ARM_DMA_MEM_TO_MEM)
    {
        while ((dma_desc->dst_addr |
                dma_desc->src_addr |
                dma_desc->total_len) &
               ((1 << dma_desc->dst_bsize) - 1))
        {
            dma_desc->dst_bsize = dma_desc->dst_bsize - 1;
        }
    }
    else
    {
        if((dma_desc->dst_addr |
            dma_desc->src_addr |
            dma_desc->total_len) &
           ((1 << dma_desc->dst_bsize) - 1))
        {
            return ARM_DMA_ERROR_UNALIGNED;
        }
    }

    dma_desc->src_bsize = dma_desc->dst_bsize;

    return ARM_DRIVER_OK;
}

/**
  \fn          void DMA_Desc2CCR(DMA_CCR_Type *ccr, DMA_CHANNEL_INFO *chnl_info)
  \brief       Prepare the CCR from Descriptor information
  \param[in]   ccr  Pointer to CCR type
  \param[in]   chnl_info  channel info
  \return      \ref execution_status
*/
__STATIC_INLINE void DMA_Desc2CCR(DMA_CCR_Type *ccr, DMA_CHANNEL_INFO *chnl_info)
{
    ccr->value = 0;

    ccr->value_b.dst_burst_len = chnl_info->desc_info.dst_blen - 1;
    ccr->value_b.src_burst_len = chnl_info->desc_info.src_blen - 1;

    ccr->value_b.dst_burst_size = chnl_info->desc_info.dst_bsize;
    ccr->value_b.src_burst_size = chnl_info->desc_info.src_bsize;

    ccr->value_b.dst_cache_ctrl = DMA_CACHE_SUPPORT;
    ccr->value_b.src_cache_ctrl = DMA_CACHE_SUPPORT;

    if (chnl_info->desc_info.direction == ARM_DMA_MEM_TO_DEV)
    {
        ccr->value_b.dst_inc = DMA_BURST_FIXED;
        ccr->value_b.src_inc = DMA_BURST_INCREMENTING;
    }
    else if (chnl_info->desc_info.direction == ARM_DMA_DEV_TO_MEM)
    {
        ccr->value_b.dst_inc = DMA_BURST_INCREMENTING;
        ccr->value_b.src_inc = DMA_BURST_FIXED;
    }
    else
    {
        ccr->value_b.dst_inc = DMA_BURST_INCREMENTING;
        ccr->value_b.src_inc = DMA_BURST_INCREMENTING;
    }

    if (chnl_info->desc_info.sec_state)
        ccr->value_b.dst_prot_ctrl = 0x2; /* Non-Privileged, Non-Instruction access, Non-Secure  */
    else
        ccr->value_b.dst_prot_ctrl = 0x0; /* Non-Privileged, Non-Instruction access, Secure  */

    ccr->value_b.src_prot_ctrl = ccr->value_b.dst_prot_ctrl; /* Set same as dst protection */

    ccr->value_b.endian_swap_size = DMA_NO_SWAP;
}

/**
  \fn          int32_t DMA_InvalidateDCache (DMA_DESC_INFO *desc_info)
  \brief       Invalidate the Dcache based on direction
  \param[in]   desc_info  DMA descriptor info
  \return      None
*/
__STATIC_INLINE void DMA_InvalidateDCache(DMA_DESC_INFO *desc_info)
{
    if ((desc_info->direction == ARM_DMA_MEM_TO_MEM) ||
        (desc_info->direction == ARM_DMA_DEV_TO_MEM))
    {
        SCB_InvalidateDCache_by_Addr(GlobalToLocal(desc_info->dst_addr), desc_info->total_len);
    }
}

/**
  \fn          int32_t DMA_CleanDCache (DMA_DESC_INFO *desc_info)
  \brief       Clean the Dcache based on direction
  \param[in]   desc_info  DMA descriptor info
  \return      None
*/
__STATIC_INLINE void DMA_CleanDCache(DMA_DESC_INFO *desc_info)
{
    if ((desc_info->direction == ARM_DMA_MEM_TO_MEM) ||
        (desc_info->direction == ARM_DMA_MEM_TO_DEV))
    {
        SCB_CleanDCache_by_Addr(GlobalToLocal(desc_info->src_addr), desc_info->total_len);
    }
}

/**
  \fn          int32_t DMA_PrepareMcode(uint8_t *mcode, DMA_CCR_Type ccr,
                                        DMA_CHANNEL_INFO *chnl_info)
  \brief       Prepare the DMA microcode as per the descriptor provided
  \param[in]   mcode  Pointer to microcode buffer
  \param[in]   ccr    Channel control value
  \param[in]   chnl_info  Pointer to DMA channel info
  \return      \ref execution_status
*/
int32_t DMA_PrepareMcode(uint8_t *mcode, DMA_CCR_Type ccr,
                         DMA_CHANNEL_INFO *chnl_info)
{
    uint32_t total_bytes, req_burst, rem_blen;
    uint16_t burst, rem_bytes;
    uint16_t idx = 0;
    uint16_t lp_start_lc1, lp_start_lc0;
    bool ret;
    uint16_t lc0, lc1;
    DMA_DESC_INFO *desc = &chnl_info->desc_info;
    DMA_XFER_Type xfer_type;
    DMA_LOOP_ARGS lp_args;

    ret = DMA_ConstructMove (ccr.value, CCR, mcode, &idx);
    if (!ret)
        return ARM_DMA_ERROR_BUFFER;

    ret = DMA_ConstructMove (desc->src_addr, SAR, mcode, &idx);
    if (!ret)
        return ARM_DMA_ERROR_BUFFER;

    ret = DMA_ConstructMove (desc->dst_addr, DAR, mcode, &idx);
    if (!ret)
        return ARM_DMA_ERROR_BUFFER;

    burst = (1 << desc->dst_bsize) * desc->dst_blen;
    total_bytes = desc->total_len;
    req_burst = total_bytes / burst;
    rem_bytes = total_bytes - ((total_bytes / burst) * (burst));
    rem_blen = rem_bytes / (1 << desc->dst_bsize);

    while (req_burst)
    {
        if (req_burst >= (DMA_MAX_LP_CNT * DMA_MAX_LP_CNT))
        {
            lc0 = DMA_MAX_LP_CNT;
            lc1 = DMA_MAX_LP_CNT;
            req_burst = req_burst - (DMA_MAX_LP_CNT * DMA_MAX_LP_CNT);
        } else if (req_burst >= DMA_MAX_LP_CNT)
        {
            lc0 = DMA_MAX_LP_CNT;
            lc1 = req_burst / lc0;
            req_burst = req_burst - (lc0 * lc1) ;
        } else
        {
            lc0 = req_burst;
            lc1 = 0;
            req_burst = 0;
        }

        lp_start_lc1 = 0;
        if (lc1)
        {
            ret = DMA_ConstructLoop (LC1, (uint8_t)lc1, mcode, &idx);
            if (!ret)
                return ARM_DMA_ERROR_BUFFER;
            lp_start_lc1 = idx;
        }

        if (lc0 == 0)
            return ARM_DMA_ERROR_MAX_TRANSFER;

        ret = DMA_ConstructLoop (LC0, (uint8_t)lc0, mcode, &idx);
        if (!ret)
            return ARM_DMA_ERROR_BUFFER;

        lp_start_lc0 = idx;

        if (desc->dst_blen == 1)
            xfer_type = SINGLE;
        else
            xfer_type = BURST;

        if (desc->direction != ARM_DMA_MEM_TO_MEM)
        {
            ret = DMA_ConstructFlushPeri (desc->peri_num, mcode, &idx);
            if (!ret)
                return ARM_DMA_ERROR_BUFFER;

            ret = DMA_ConstructWaitforPeri (xfer_type, desc->peri_num, mcode, &idx);
            if (!ret)
                return ARM_DMA_ERROR_BUFFER;

            if (desc->direction ==  ARM_DMA_MEM_TO_DEV)
            {
                ret = DMA_ConstructLoad (xfer_type, mcode, &idx);
                if (!ret)
                    return ARM_DMA_ERROR_BUFFER;
                ret = DMA_ConstructStorePeri (xfer_type, desc->peri_num, mcode, &idx);
                if (!ret)
                    return ARM_DMA_ERROR_BUFFER;

                /* If I2S mono mode is enabled for this channel, write zeros */
                if(chnl_info->flags & DMA_CHANNEL_I2S_MONO_MODE)
                {
                    ret = DMA_ConstructStoreZeros (mcode, &idx);
                    if (!ret)
                        return ARM_DMA_ERROR_BUFFER;
                }
            } else /* ARM_DMA_DEV_TO_MEM */
            {
                ret = DMA_ConstructLoadPeri (xfer_type, desc->peri_num, mcode, &idx);
                if (!ret)
                    return ARM_DMA_ERROR_BUFFER;
                ret = DMA_ConstructStore (xfer_type, mcode, &idx);
                if (!ret)
                    return ARM_DMA_ERROR_BUFFER;

                /* If I2S mono mode is enabled, read right channel and discard it */
                if(chnl_info->flags & DMA_CHANNEL_I2S_MONO_MODE)
                {
                    ret = DMA_ConstructLoadPeri (xfer_type, desc->peri_num, mcode, &idx);
                    if (!ret)
                        return ARM_DMA_ERROR_BUFFER;
                    ret = DMA_ConstructStore (xfer_type, mcode, &idx);
                    if (!ret)
                        return ARM_DMA_ERROR_BUFFER;
                    ret = DMA_ConstructAddNeg (DAR, (1 << desc->dst_bsize), mcode, &idx);
                    if (!ret)
                        return ARM_DMA_ERROR_BUFFER;
                }
            }
        } else /* ARM_DMA_MEM_TO_MEM */
        {
            ret = DMA_ConstructLoad (FORCE, mcode, &idx);
            if (!ret)
                return ARM_DMA_ERROR_BUFFER;
            ret = DMA_ConstructStore (FORCE, mcode, &idx);
            if (!ret)
                return ARM_DMA_ERROR_BUFFER;
        }

        lp_args.jump = idx - lp_start_lc0;
        lp_args.lc = LC0;
        lp_args.nf = 1;
        lp_args.xfer_type = FORCE;
        ret = DMA_ConstructLoopEnd (&lp_args, mcode, &idx);
        if (!ret)
            return ARM_DMA_ERROR_BUFFER;

        if (lc1)
        {
            lp_args.jump = idx - lp_start_lc1;
            lp_args.lc = LC1;
            lp_args.nf = 1;
            lp_args.xfer_type = FORCE;
            ret = DMA_ConstructLoopEnd (&lp_args, mcode, &idx);
            if (!ret)
                return ARM_DMA_ERROR_BUFFER;
        }
    }

    if (rem_blen) {

        ccr.value_b.dst_burst_len = rem_blen - 1;
        ccr.value_b.src_burst_len = rem_blen - 1;

        ret = DMA_ConstructMove (ccr.value, CCR, mcode, &idx);
        if (!ret)
            return ARM_DMA_ERROR_BUFFER;

        if (desc->direction != ARM_DMA_MEM_TO_MEM)
        {
            ret = DMA_ConstructFlushPeri (desc->peri_num, mcode, &idx);
            if (!ret)
                return ARM_DMA_ERROR_BUFFER;

            ret = DMA_ConstructWaitforPeri (BURST, desc->peri_num, mcode, &idx);
            if (!ret)
                return ARM_DMA_ERROR_BUFFER;

            if (desc->direction ==  ARM_DMA_MEM_TO_DEV)
            {
                ret = DMA_ConstructLoad (BURST, mcode, &idx);
                    if (!ret)
                        return ARM_DMA_ERROR_BUFFER;
                ret = DMA_ConstructStorePeri (BURST, desc->peri_num, mcode, &idx);
                if (!ret)
                    return ARM_DMA_ERROR_BUFFER;

                /* If I2S mono mode is enabled for this channel, write zeros */
                if(chnl_info->flags & DMA_CHANNEL_I2S_MONO_MODE)
                {
                    ret = DMA_ConstructStoreZeros (mcode, &idx);
                    if (!ret)
                        return ARM_DMA_ERROR_BUFFER;
                }
            } else /* ARM_DMA_DEV_TO_MEM */
            {
                ret = DMA_ConstructLoadPeri (BURST, desc->peri_num, mcode, &idx);
                if (!ret)
                    return ARM_DMA_ERROR_BUFFER;
                ret = DMA_ConstructStore (BURST, mcode, &idx);
                if (!ret)
                    return ARM_DMA_ERROR_BUFFER;

                /* If I2S mono mode is enabled, read right channel and discard it */
                if(chnl_info->flags & DMA_CHANNEL_I2S_MONO_MODE)
                {
                    ret = DMA_ConstructLoadPeri (BURST, desc->peri_num, mcode, &idx);
                    if (!ret)
                        return ARM_DMA_ERROR_BUFFER;
                    ret = DMA_ConstructStore (BURST, mcode, &idx);
                    if (!ret)
                        return ARM_DMA_ERROR_BUFFER;
                    ret = DMA_ConstructAddNeg (DAR, (1 << desc->dst_bsize), mcode, &idx);
                    if (!ret)
                        return ARM_DMA_ERROR_BUFFER;
                }
            }
        } else /* ARM_DMA_MEM_TO_MEM */
        {
            ret = DMA_ConstructLoad (FORCE, mcode, &idx);
            if (!ret)
                    return ARM_DMA_ERROR_BUFFER;
            ret = DMA_ConstructStore (FORCE, mcode, &idx);
            if (!ret)
                return ARM_DMA_ERROR_BUFFER;
        }
    }

    ret = DMA_ConstructWriteBarrier (mcode, &idx);
    if (!ret)
            return ARM_DMA_ERROR_BUFFER;

    ret = DMA_ConstructSendEvent (chnl_info->evnt_index, mcode, &idx);
    if (!ret)
        return ARM_DMA_ERROR_BUFFER;

    ret = DMA_ConstructEnd (mcode, &idx);
    if (!ret)
        return ARM_DMA_ERROR_BUFFER;

    /* Flush the Cache now */
    SCB_CleanDCache_by_Addr(mcode, DMA_MCODE_SIZE);

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DMA_DeAllocate (DMA_Handle_Type *handle, DMA_DRV_INFO *dma)
  \brief       Status of a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   dma  Pointer to DMA resources
  \return      \ref execution_status
*/
__STATIC_INLINE int32_t DMA_DeAllocate (DMA_Handle_Type *handle, DMA_DRV_INFO *dma)
{
    DMA_CONFIG_INFO *dma_cfg = dma->cfg;

    if (dma->flags == DMA_DRV_UNINITIALIZED)
        return ARM_DRIVER_ERROR;

    if (!handle)
        return ARM_DRIVER_ERROR_PARAMETER;

    if ((*handle > DMA_MAX_CHANNELS) || (*handle < 0))
        return ARM_DMA_ERROR_HANDLE;

    __disable_irq();

    /* If the Channel is busy then return error so that app can call Stop */
    dma_cfg->channel_thread[*handle].curr_state = DMA_GetChannelStatus (*handle, dma);
    if (dma_cfg->channel_thread[*handle].curr_state != STOPPED) {
        __enable_irq();
        return ARM_DRIVER_ERROR_BUSY;
    }

    NVIC_DisableIRQ (dma->irq_start + dma_cfg->channel_thread[*handle].channel_info.evnt_index);
    DMA_ReleaseChannel (*handle, dma_cfg);
    DMA_ReleaseEvent (*handle, dma_cfg->channel_thread[*handle].channel_info.evnt_index, dma_cfg);

    dma_cfg->channel_thread[*handle].channel_info.chnl_num = -1;

    /* De-Initialize the Channel flags */
    dma->cfg->channel_thread[*handle].channel_info.flags = 0;
    dma->cfg->channel_thread[*handle].user_mcode = NULL;

    *handle = -1;
    __enable_irq();

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DMA_GetStatus (DMA_Handle_Type *handle,
                                      uint32_t *count,
                                      DMA_DRV_INFO *dma)
  \brief       Status of a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   count  Pointer to number of bytes
  \param[in]   dma  Pointer to DMA resources
  \return      \ref execution_status
*/
static int32_t DMA_GetStatus (DMA_Handle_Type *handle, uint32_t *count,
                              DMA_DRV_INFO *dma)
{
    DMA_CONFIG_INFO *dma_cfg = dma->cfg;
    DMA_CHANNEL_INFO *chnl_info;
    DMA_DESC_INFO *desc;
    uint32_t curr_addr;

    if (dma->flags == DMA_DRV_UNINITIALIZED)
        return ARM_DRIVER_ERROR;

    if (!handle || !count)
        return ARM_DRIVER_ERROR_PARAMETER;

    if ((*handle > DMA_MAX_CHANNELS) || (*handle < 0))
        return ARM_DMA_ERROR_HANDLE;

    __disable_irq();

    chnl_info = &dma_cfg->channel_thread[*handle].channel_info;
    desc = &chnl_info->desc_info;

    dma_cfg->channel_thread[*handle].curr_state = DMA_GetChannelStatus (*handle, dma);
    if (dma_cfg->channel_thread[*handle].curr_state == STOPPED) {
        curr_addr = DMA_GetChannelSrcAddress (*handle, dma);
        *count = curr_addr - (uint32_t)desc->src_addr;
        __enable_irq();
        return ARM_DRIVER_OK;
    }

    if ((desc->direction == ARM_DMA_MEM_TO_DEV) ||
        (desc->direction == ARM_DMA_MEM_TO_MEM))
    {
        curr_addr = DMA_GetChannelSrcAddress (*handle, dma);
        *count = curr_addr - (uint32_t)desc->src_addr;
    }
    else if ((chnl_info->desc_info.direction == ARM_DMA_DEV_TO_MEM) ||
             (desc->direction == ARM_DMA_MEM_TO_MEM))
    {
        curr_addr = DMA_GetChannelDestAddress (*handle, dma);
        *count = curr_addr - desc->dst_addr;
    }

    if ((dma_cfg->channel_thread[*handle].curr_state == FAULTING_COMPLETING) ||
        (dma_cfg->channel_thread[*handle].curr_state == FAULTING))
    {
        __enable_irq();
        return ARM_DMA_ERROR_FAULT;
    }

    __enable_irq();
    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DMA_Stop (DMA_Handle_Type *handle, DMA_DRV_INFO *dma)
  \brief       Stop a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   dma  Pointer to DMA resources
  \return      \ref execution_status
*/
static int32_t DMA_Stop (DMA_Handle_Type *handle, DMA_DRV_INFO *dma)
{
    DMA_CONFIG_INFO *dma_cfg = dma->cfg;
    uint8_t kill_mcode;
    DMA_DBGINST0_Type dbginst0;

    if (dma->flags == DMA_DRV_UNINITIALIZED)
        return ARM_DRIVER_ERROR;

    if (!handle)
        return ARM_DRIVER_ERROR_PARAMETER;

    if ((*handle > DMA_MAX_CHANNELS) || (*handle < 0))
        return ARM_DMA_ERROR_HANDLE;

    __disable_irq();

    dma_cfg->channel_thread[*handle].curr_state = DMA_GetChannelStatus (*handle, dma);
    if (dma_cfg->channel_thread[*handle].curr_state == STOPPED)
    {
        __enable_irq();
        return ARM_DRIVER_OK;
    }

    if (DMA_GetDbgStatus (dma))
    {
        __enable_irq();
        return ARM_DRIVER_ERROR_BUSY;
    }

    DMA_ConstructKill (&kill_mcode);

    dbginst0.value = 0;
    dbginst0.value_b.ins_byte0 = kill_mcode;
    dbginst0.value_b.chn_num = *handle;
    dbginst0.value_b.dbg_thrd = true;
    DMA_WriteDbgInst0 (dbginst0.value, dma);
    DMA_WriteDbgInst1 (0, dma);

    DMA_ExecuteCmd (dma);

    DMA_DisableInterrupt (&dma_cfg->channel_thread[*handle], dma);
    DMA_ClearInterrupt (&dma_cfg->channel_thread[*handle], dma);

    NVIC_DisableIRQ (dma->irq_start + dma_cfg->channel_thread[*handle].channel_info.evnt_index);

    __enable_irq();

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DMA_Start (DMA_Handle_Type *handle,
                                  ARM_DMA_PARAMS *params,
                                  DMA_DRV_INFO *dma)
  \brief       Start a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   params  Pointer to DMA desc parameters
  \param[in]   dma  Pointer to DMA resources
  \return      \ref execution_status
*/
static int32_t DMA_Start (DMA_Handle_Type *handle, ARM_DMA_PARAMS *params,
                          DMA_DRV_INFO *dma)
{
    int32_t ret = 0;
    DMA_CONFIG_INFO *dma_cfg = dma->cfg;
    DMA_CHANNEL_INFO *chnl_info;
    uint8_t *mcode;
    uint8_t go_mcode[6] =  {0};
    DMA_DBGINST0_Type dbginst0;
    DMA_DBGINST1_Type dbginst1;
    DMA_CCR_Type      ccr;

    if (dma->flags == DMA_DRV_UNINITIALIZED)
        return ARM_DRIVER_ERROR;

    if (!handle || !params)
        return ARM_DRIVER_ERROR_PARAMETER;

    if ((*handle > DMA_MAX_CHANNELS) || (*handle < 0))
        return ARM_DMA_ERROR_HANDLE;

    __disable_irq();

    if (DMA_GetDbgStatus (dma))
    {
        __enable_irq();
        return ARM_DRIVER_ERROR_BUSY;
    }

    dma_cfg->channel_thread[*handle].curr_state = DMA_GetChannelStatus (*handle, dma);
    if (dma_cfg->channel_thread[*handle].curr_state != STOPPED)
        return ARM_DMA_ERROR_BUSY;

    chnl_info = &dma_cfg->channel_thread[*handle].channel_info;

    /* Set the security state of the channel same as Manager */
    chnl_info->desc_info.sec_state = DMA_GetManagerSecurityStatus(dma);

    /* Check for user provided microcode */
    if(dma->cfg->channel_thread[*handle].channel_info.flags & DMA_CHANNEL_USE_USER_MCODE)
    {
        mcode = dma->cfg->channel_thread[*handle].user_mcode;

        chnl_info->desc_info.cb_event  = params->cb_event;
        chnl_info->desc_info.src_addr  = LocalToGlobal(params->src_addr);
        chnl_info->desc_info.dst_addr  = LocalToGlobal(params->dst_addr);
        chnl_info->desc_info.total_len = params->num_bytes;
        chnl_info->desc_info.peri_num  = params->peri_reqno;
    }
    else
    {
        mcode = (uint8_t*)&dma_cfg->channel_thread[*handle].dma_mcode;

        ret = DMA_CopyDesc (params, &chnl_info->desc_info);
        if (ret < 0)
        {
            __enable_irq();
            return ret;
        }

        DMA_Desc2CCR(&ccr, chnl_info);

        ret = DMA_PrepareMcode(mcode, ccr, chnl_info);
        if (ret < 0)
        {
            __enable_irq();
            return ret;
        }
    }

    DMA_CleanDCache(&chnl_info->desc_info);

    DMA_ConstructGo (chnl_info->desc_info.sec_state,
                     chnl_info->chnl_num,
                     LocalToGlobal(mcode),
                     &go_mcode[0]);

    DMA_EnableInterrupt (&dma_cfg->channel_thread[*handle], dma);

    /* Disable it first */
    NVIC_DisableIRQ (dma->irq_start + chnl_info->evnt_index);
    /* Clear Any Pending IRQ */
    NVIC_ClearPendingIRQ (dma->irq_start + chnl_info->evnt_index);
    /* Set the priority of this particular IRQ */
    NVIC_SetPriority (dma->irq_start + chnl_info->evnt_index, params->irq_priority);
    /* Enable the IRQ */
    NVIC_EnableIRQ (dma->irq_start + chnl_info->evnt_index);


    dbginst0.value = 0;
    dbginst0.value_b.ins_byte0 = go_mcode[0];
    dbginst0.value_b.ins_byte1 = go_mcode[1];
    dbginst0.value_b.chn_num = *handle;
    dbginst0.value_b.dbg_thrd = MANAGER;
    DMA_WriteDbgInst0 (dbginst0.value, dma);

    dbginst1.value_b.ins_byte2 = go_mcode[2];
    dbginst1.value_b.ins_byte3 = go_mcode[3];
    dbginst1.value_b.ins_byte4 = go_mcode[4];
    dbginst1.value_b.ins_byte5 = go_mcode[5];
    DMA_WriteDbgInst1 (dbginst1.value, dma);

    DMA_ExecuteCmd (dma);

    __enable_irq();

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DMA_Control (DMA_Handle_Type *handle, uint32_t control, uint32_t arg, DMA_DRV_INFO *dma)
  \brief       Control DMA Interface.
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   control Operation
  \param[in]   arg     Argument 1 of operation (optional)
  \param[in]   dma     Pointer to DMA resources
  \return      common \ref execution_status and driver specific \ref dma_execution_status
*/
static int32_t DMA_Control (DMA_Handle_Type *handle, uint32_t control,
		                    uint32_t arg, DMA_DRV_INFO *dma)
{
    /* Verify whether the driver is initialized */
    if (!(dma->flags & DMA_DRV_INIT_DONE))
        return ARM_DRIVER_ERROR;

    if (!handle)
        return ARM_DRIVER_ERROR_PARAMETER;

    if ((*handle > DMA_MAX_CHANNELS) || (*handle < 0))
        return ARM_DMA_ERROR_HANDLE;

    /* Handle Control Codes */
    switch (control & ARM_DMA_CONTROL_Msk)
    {
    case ARM_DMA_USER_PROVIDED_MCODE:
        if (!arg)
            return ARM_DRIVER_ERROR_PARAMETER;

        dma->cfg->channel_thread[*handle].channel_info.flags |= DMA_CHANNEL_USE_USER_MCODE;
        dma->cfg->channel_thread[*handle].user_mcode = (void*)arg;
        break;
    case ARM_DMA_I2S_MONO_MODE:
        dma->cfg->channel_thread[*handle].channel_info.flags |= DMA_CHANNEL_I2S_MONO_MODE;
        break;
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DMA_Allocate (DMA_Handle_Type *handle, DMA_DRV_INFO *dma)
  \brief       Allocate Channel for transfer operation
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   dma  Pointer to DMA resources
  \return      \ref execution_status
*/
static int32_t DMA_Allocate (DMA_Handle_Type *handle, DMA_DRV_INFO *dma)
{
    DMA_CONFIG_INFO *dma_cfg = dma->cfg;
    int8_t event = 0;

    if (dma->flags == DMA_DRV_UNINITIALIZED)
        return ARM_DRIVER_ERROR;

    if (!handle)
        return ARM_DRIVER_ERROR_PARAMETER;

    __disable_irq();

    *handle = DMA_GetFreeChannel (dma_cfg);
    if (*handle < 0)
    {
        __enable_irq();
        return ARM_DMA_ERROR_HANDLE;
    }

    event = DMA_GetFreeEvent (dma_cfg);
    if (event < 0)
    {
        DMA_ReleaseChannel (*handle, dma_cfg);
        __enable_irq();
        return ARM_DMA_ERROR_EVENT;
    }

    /* Assign event to channel */
    dma_cfg->event[event] = *handle;
    dma_cfg->channel_thread[*handle].channel_info.evnt_index = event;
    dma_cfg->channel_thread[*handle].channel_info.chnl_num = *handle;

    /* Initialize the Channel flags */
    dma->cfg->channel_thread[*handle].channel_info.flags = 0;
    dma->cfg->channel_thread[*handle].user_mcode = NULL;

    __enable_irq();

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DMA_Initialize (DMA_DRV_INFO *dma)
  \brief       Initialize DMA Interface.
  \param[in]   dma       Pointer to DMA resources
  \return      \ref execution_status
*/
static int32_t DMA_Initialize (DMA_DRV_INFO *dma)
{
    uint8_t cnt;

    if (dma->flags == DMA_DRV_INIT_DONE)
        return ARM_DRIVER_OK;

    __disable_irq();

    /* Mark all events as not being used */
    for (cnt = 0; cnt < DMA_MAX_EVENTS; cnt++)
        dma->cfg->event[cnt] = -1;

    for (cnt = 0; cnt < DMA_MAX_CHANNELS; cnt++)
        dma->cfg->channel_thread[cnt].channel_info.chnl_num = -1;

    /* Clear Any Pending IRQ */
    NVIC_ClearPendingIRQ (dma->irq_start + DMA_IRQ_ABORT_OFFSET);
    /* Set the priority of this particular IRQ */
    NVIC_SetPriority (dma->irq_start + DMA_IRQ_ABORT_OFFSET, dma->abort_irq_priority);
    /* Enable the Abort IRQ */
    NVIC_EnableIRQ (dma->irq_start + DMA_IRQ_ABORT_OFFSET);

    dma->flags = DMA_DRV_INIT_DONE;

    __enable_irq();

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DMA_Uninitialize (DMA_DRV_INFO *dma)
  \brief       De-initialize DMA Interface.
  \param[in]   dma       Pointer to DMA resources
  \return      \ref execution_status
*/
static int32_t DMA_Uninitialize (DMA_DRV_INFO *dma)
{
    /*
     * For the sake of completeness of API,
     * this function is added.
     * dma->flags = DMA_DRV_UNINITIALIZED;
     *
     */

    (void)dma;

    return ARM_DRIVER_OK;
}

/**
  \fn          void DMA_IRQHandler (void)
  \brief       common DMA IRQ handler
  \param[in]   event_idx  Event index
  \param[in]   dma  DMA resource
  \return      None
*/
static void DMA_IRQHandler (int8_t event_idx, DMA_DRV_INFO *dma)
{
    DMA_CONFIG_INFO *dma_cfg = dma->cfg;
    int8_t handle = event_idx;
    DMA_DESC_INFO *desc_info;

    /* FIXME: Assuming that channel number and event idx are same */
    DMA_ClearInterrupt (&dma_cfg->channel_thread[handle], dma);

    desc_info = &dma_cfg->channel_thread[handle].channel_info.desc_info;

    /* Invalidate the data from cache */
    DMA_InvalidateDCache(desc_info);

    if (desc_info->cb_event)
        desc_info->cb_event (ARM_DMA_EVENT_COMPLETE, desc_info->peri_num);
}

/**
  \fn          void DMA_AbortIRQHandler (void)
  \brief       Abort DMA handler
  \param[in]   dma  DMA resource
  \return      None
*/
static void DMA_AbortIRQHandler (DMA_DRV_INFO *dma)
{
    DMA_CONFIG_INFO *dma_cfg = dma->cfg;
    uint8_t fault_status = 0;
    DMA_Handle_Type handle = 0;
    DMA_DESC_INFO *desc_info;

    /* FIXME: Assuming that channel number and event idx are same */
    fault_status = DMA_GetChannelFaultStatus (dma);
    while (fault_status)
    {
        if (fault_status & 1)
        {
            DMA_Stop (&handle, dma);
            desc_info = &dma_cfg->channel_thread[handle].channel_info.desc_info;

            /* Invalidate the data from cache */
            DMA_InvalidateDCache(desc_info);

            if (desc_info->cb_event)
                desc_info->cb_event (ARM_DMA_EVENT_ABORT, desc_info->peri_num);
        }

        handle++;
        fault_status = fault_status >> 1;
    }

    /* Get Manager Fault Status */
    fault_status = DMA_GetManagerFaultStatus (dma);
    if(fault_status)
    {
        /*
         * It requires change in HW signal state to move the manager thread
         * to Executing state.
         *
         * For now, just do watchdog reset.
         */
        while(1);
    }
}

#if (RTE_DMA0)

/**
  \fn          int32_t DMA0_Initialize (void)
  \brief       Initialize DMA Interface.
  \return      \ref execution_status
*/
static int32_t DMA0_Initialize (void)
{
    DMA_DRV_INFO *dma = &DMA0;

    /* set the apb interface for accessing registers */
    if(dma->apb_intf)
        dma->intf_paddr = (DMA_TypeDef*)DMA0_NS_BASE;
    else
        dma->intf_paddr = (DMA_TypeDef*)DMA0_SE_BASE;

    return DMA_Initialize (dma);
}

/**
  \fn          int32_t DMA0_Uninitialize (void)
  \brief       Un-Initialize DMA Interface.
  \return      \ref execution_status
*/
static int32_t DMA0_Uninitialize (void)
{
    return DMA_Uninitialize (&DMA0);
}

/**
  \fn          int32_t DMA0_Allocate (DMA_Handle_Type *handle)
  \brief       Allocate Channel for transfer operation
  \param[in]   handle  Pointer to DMA channel number
  \return      \ref execution_status
*/
static int32_t DMA0_Allocate (DMA_Handle_Type *handle)
{
    return DMA_Allocate (handle, &DMA0);
}

/**
  \fn          int32_t DMA0_Control (DMA_Handle_Type *handle, uint32_t control, uint32_t arg)
  \brief       Control DMA Interface.
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   control Operation
  \param[in]   arg     Argument 1 of operation
  \return      common \ref execution_status and driver specific \ref dma_execution_status
*/
static int32_t DMA0_Control (DMA_Handle_Type *handle, uint32_t control, uint32_t arg)
{
    return DMA_Control (handle, control, arg, &DMA0);
}

/**
  \fn          int32_t DMA0_Start (int32_t *handle, ARM_DMA_PARAMS *params)
  \brief       Start a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   params  Pointer to DMA desc parameters
  \return      \ref execution_status
*/
static int32_t DMA0_Start (DMA_Handle_Type *handle, ARM_DMA_PARAMS *params)
{
    return DMA_Start (handle, params, &DMA0);
}

/**
  \fn          int32_t DMA_Stop (int32_t *handle)
  \brief       Stop a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \return      \ref execution_status
*/
static int32_t DMA0_Stop (DMA_Handle_Type *handle)
{
    return DMA_Stop (handle, &DMA0);
}

/**
  \fn          DMA0_GetStatus (int32_t *handle, uint32_t *count)
  \brief       Status of a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   count  Pointer to pass transferred count
  \return      \ref execution_status
*/
static int32_t DMA0_GetStatus (DMA_Handle_Type *handle, uint32_t *count)
{
    return DMA_GetStatus (handle, count, &DMA0);
}

/**
  \fn          DMA0_DeAllocate (int32_t *handle)
  \brief       De-Allocate a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \return      \ref execution_status
*/
static int32_t DMA0_DeAllocate (DMA_Handle_Type *handle)
{
    return DMA_DeAllocate (handle, &DMA0);
}

/**
  \fn          void  DMA0_IRQ0Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ0
*/
void DMA0_IRQ0Handler (void)
{
    DMA_IRQHandler (0, &DMA0);
}

/**
  \fn          void  DMA0_IRQ1Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ1
*/
void DMA0_IRQ1Handler (void)
{
    DMA_IRQHandler (1, &DMA0);
}

/**
  \fn          void  DMA0_IRQ2Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ2
*/
void DMA0_IRQ2Handler (void)
{
    DMA_IRQHandler (2, &DMA0);
}

/**
  \fn          void  DMA0_IRQ3Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ3
*/
void DMA0_IRQ3Handler (void)
{
    DMA_IRQHandler (3, &DMA0);
}

/**
  \fn          void  DMA0_IRQ4Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ4
*/
void DMA0_IRQ4Handler (void)
{
    DMA_IRQHandler (4, &DMA0);
}

/**
  \fn          void  DMA0_IRQ5Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ5
*/
void DMA0_IRQ5Handler (void)
{
    DMA_IRQHandler (5, &DMA0);
}

/**
  \fn          void  DMA0_IRQ6Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ6
*/
void DMA0_IRQ6Handler (void)
{
    DMA_IRQHandler (6, &DMA0);
}

/**
  \fn          void  DMA0_IRQ7Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ7
*/
void DMA0_IRQ7Handler (void)
{
    DMA_IRQHandler (7, &DMA0);
}

/**
  \fn          void  DMA0_IRQ8Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ8
*/
void DMA0_IRQ8Handler (void)
{
    DMA_IRQHandler (8, &DMA0);
}

/**
  \fn          void  DMA0_IRQ9Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ9
*/
void DMA0_IRQ9Handler (void)
{
    DMA_IRQHandler (9, &DMA0);
}

/**
  \fn          void  DMA0_IRQ10Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ10
*/
void DMA0_IRQ10Handler (void)
{
    DMA_IRQHandler (10, &DMA0);
}

/**
  \fn          void  DMA0_IRQ11Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ11
*/
void DMA0_IRQ11Handler (void)
{
    DMA_IRQHandler (11, &DMA0);
}

/**
  \fn          void  DMA0_IRQ12Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ12
*/
void DMA0_IRQ12Handler (void)
{
    DMA_IRQHandler (12, &DMA0);
}

/**
  \fn          void  DMA0_IRQ13Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ13
*/
void DMA0_IRQ13Handler (void)
{
    DMA_IRQHandler (13, &DMA0);
}

/**
  \fn          void  DMA0_IRQ14Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ14
*/
void DMA0_IRQ14Handler (void)
{
    DMA_IRQHandler (14, &DMA0);
}

/**
  \fn          void  DMA0_IRQ15Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ15
*/
void DMA0_IRQ15Handler (void)
{
    DMA_IRQHandler (15, &DMA0);
}

/**
  \fn          void  DMA0_IRQ16Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ16
*/
void DMA0_IRQ16Handler (void)
{
    DMA_IRQHandler (16, &DMA0);
}

/**
  \fn          void  DMA0_IRQ17Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ17
*/
void DMA0_IRQ17Handler (void)
{
    DMA_IRQHandler (17, &DMA0);
}

/**
  \fn          void  DMA0_IRQ18Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ18
*/
void DMA0_IRQ18Handler (void)
{
    DMA_IRQHandler (18, &DMA0);
}

/**
  \fn          void  DMA0_IRQ19Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ19
*/
void DMA0_IRQ19Handler (void)
{
    DMA_IRQHandler (19, &DMA0);
}

/**
  \fn          void  DMA0_IRQ20Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ20
*/
void DMA0_IRQ20Handler (void)
{
    DMA_IRQHandler (20, &DMA0);
}

/**
  \fn          void  DMA0_IRQ21Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ21
*/
void DMA0_IRQ21Handler (void)
{
    DMA_IRQHandler (21, &DMA0);
}

/**
  \fn          void  DMA0_IRQ22Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ22
*/
void DMA0_IRQ22Handler (void)
{
    DMA_IRQHandler (22, &DMA0);
}

/**
  \fn          void  DMA0_IRQ23Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ23
*/
void DMA0_IRQ23Handler (void)
{
    DMA_IRQHandler (23, &DMA0);
}

/**
  \fn          void  DMA0_IRQ24Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ24
*/
void DMA0_IRQ24Handler (void)
{
    DMA_IRQHandler (24, &DMA0);
}

/**
  \fn          void  DMA0_IRQ25Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ25
*/
void DMA0_IRQ25Handler (void)
{
    DMA_IRQHandler (25, &DMA0);
}

/**
  \fn          void  DMA0_IRQ26Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ26
*/
void DMA0_IRQ26Handler (void)
{
    DMA_IRQHandler (26, &DMA0);
}

/**
  \fn          void  DMA0_IRQ27Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ27
*/
void DMA0_IRQ27Handler (void)
{
    DMA_IRQHandler (27, &DMA0);
}

/**
  \fn          void  DMA0_IRQ28Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ28
*/
void DMA0_IRQ28Handler (void)
{
    DMA_IRQHandler (28, &DMA0);
}

/**
  \fn          void  DMA0_IRQ29Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ29
*/
void DMA0_IRQ29Handler (void)
{
    DMA_IRQHandler (29, &DMA0);
}

/**
  \fn          void  DMA0_IRQ30Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ30
*/
void DMA0_IRQ30Handler (void)
{
    DMA_IRQHandler (30, &DMA0);
}

/**
  \fn          void  DMA0_IRQ31Handler (void)
  \brief       Run the IRQ Handler for DMA0-IRQ31
*/
void DMA0_IRQ31Handler (void)
{
    DMA_IRQHandler (31, &DMA0);
}

/**
  \fn          void  DMA0_AbortIRQHandler (void)
  \brief       Run the IRQ Handler for DMA0 Abort
*/
void DMA0_AbortIRQHandler (void)
{
    DMA_AbortIRQHandler (&DMA0);
}
/**
\brief Access structure of the DMA0 Driver.
*/
extern \
ARM_DRIVER_DMA Driver_DMA0;
ARM_DRIVER_DMA Driver_DMA0 = {
    DMA_GetVersion,
    DMA_GetCapabilities,
    DMA0_Initialize,
    DMA0_Uninitialize,
    DMA0_Allocate,
    DMA0_Control,
    DMA0_Start,
    DMA0_Stop,
    DMA0_GetStatus,
    DMA0_DeAllocate
};
#endif

#if (RTE_DMA1)

/**
  \fn          int32_t DMA1_Initialize (void)
  \brief       Initialize DMA Interface.
  \return      \ref execution_status
*/
static int32_t DMA1_Initialize (void)
{
    DMA_DRV_INFO *dma = &DMA1;

    /* set the apb interface for accessing registers */
    if(dma->apb_intf)
        dma->intf_paddr = (DMA_TypeDef*)DMA1_NS_BASE;
    else
        dma->intf_paddr = (DMA_TypeDef*)DMA1_SE_BASE;

    return DMA_Initialize (dma);
}

/**
  \fn          int32_t DMA1_Uninitialize (void)
  \brief       Un-Initialize DMA Interface.
  \return      \ref execution_status
*/
static int32_t DMA1_Uninitialize (void)
{
    return DMA_Uninitialize (&DMA1);
}

/**
  \fn          int32_t DMA1_Allocate (DMA_Handle_Type *handle)
  \brief       Allocate Channel for transfer operation
  \param[in]   handle  Pointer to DMA channel number
  \return      \ref execution_status
*/
static int32_t DMA1_Allocate (DMA_Handle_Type *handle)
{
       return DMA_Allocate (handle, &DMA1);
}

/**
  \fn          int32_t DMA1_Control (DMA_Handle_Type *handle, uint32_t control, uint32_t arg)
  \brief       Control DMA Interface.
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   control Operation
  \param[in]   arg     Argument 1 of operation
  \return      common \ref execution_status and driver specific \ref dma_execution_status
*/
static int32_t DMA1_Control (DMA_Handle_Type *handle, uint32_t control, uint32_t arg)
{
    return DMA_Control (handle, control, arg, &DMA1);
}

/**
  \fn          int32_t DMA1_Start (int32_t *handle, ARM_DMA_PARAMS *params)
  \brief       Start a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   params  Pointer to DMA desc parameters
  \return      \ref execution_status
*/
static int32_t DMA1_Start (DMA_Handle_Type *handle, ARM_DMA_PARAMS *params)
{
    return DMA_Start (handle, params, &DMA1);
}

/**
  \fn          int32_t DMA_Stop (int32_t *handle)
  \brief       Stop a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \return      \ref execution_status
*/
static int32_t DMA1_Stop (DMA_Handle_Type *handle)
{
    return DMA_Stop (handle, &DMA1);
}

/**
  \fn          DMA1_GetStatus (int32_t *handle, uint32_t *count)
  \brief       Status of a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   count  Pointer to pass transferred count
  \return      \ref execution_status
*/
static int32_t DMA1_GetStatus (DMA_Handle_Type *handle, uint32_t *count)
{
    return DMA_GetStatus (handle, count, &DMA1);
}

/**
  \fn          DMA1_DeAllocate (int32_t *handle)
  \brief       De-Allocate a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \return      \ref execution_status
*/
static int32_t DMA1_DeAllocate (DMA_Handle_Type *handle)
{
    return DMA_DeAllocate (handle, &DMA1);
}

/**
  \fn          void  DMA1_IRQ0Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ0
*/
void DMA1_IRQ0Handler (void)
{
    DMA_IRQHandler (0, &DMA1);
}

/**
  \fn          void  DMA1_IRQ1Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ1
*/
void DMA1_IRQ1Handler (void)
{
    DMA_IRQHandler (1, &DMA1);
}

/**
  \fn          void  DMA1_IRQ2Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ2
*/
void DMA1_IRQ2Handler (void)
{
    DMA_IRQHandler (2, &DMA1);
}

/**
  \fn          void  DMA1_IRQ3Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ3
*/
void DMA1_IRQ3Handler (void)
{
    DMA_IRQHandler (3, &DMA1);
}

/**
  \fn          void  DMA1_IRQ4Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ4
*/
void DMA1_IRQ4Handler (void)
{
    DMA_IRQHandler (4, &DMA1);
}

/**
  \fn          void  DMA1_IRQ5Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ5
*/
void DMA1_IRQ5Handler (void)
{
    DMA_IRQHandler (5, &DMA1);
}

/**
  \fn          void  DMA1_IRQ6Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ6
*/
void DMA1_IRQ6Handler (void)
{
    DMA_IRQHandler (6, &DMA1);
}

/**
  \fn          void  DMA1_IRQ7Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ7
*/
void DMA1_IRQ7Handler (void)
{
    DMA_IRQHandler (7, &DMA1);
}

/**
  \fn          void  DMA1_IRQ8Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ8
*/
void DMA1_IRQ8Handler (void)
{
    DMA_IRQHandler (8, &DMA1);
}

/**
  \fn          void  DMA1_IRQ9Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ9
*/
void DMA1_IRQ9Handler (void)
{
    DMA_IRQHandler (9, &DMA1);
}

/**
  \fn          void  DMA1_IRQ10Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ10
*/
void DMA1_IRQ10Handler (void)
{
    DMA_IRQHandler (10, &DMA1);
}

/**
  \fn          void  DMA1_IRQ11Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ11
*/
void DMA1_IRQ11Handler (void)
{
    DMA_IRQHandler (11, &DMA1);
}

/**
  \fn          void  DMA1_IRQ12Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ12
*/
void DMA1_IRQ12Handler (void)
{
    DMA_IRQHandler (12, &DMA1);
}

/**
  \fn          void  DMA1_IRQ13Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ13
*/
void DMA1_IRQ13Handler (void)
{
    DMA_IRQHandler (13, &DMA1);
}

/**
  \fn          void  DMA1_IRQ14Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ14
*/
void DMA1_IRQ14Handler (void)
{
    DMA_IRQHandler (14, &DMA1);
}

/**
  \fn          void  DMA1_IRQ15Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ15
*/
void DMA1_IRQ15Handler (void)
{
    DMA_IRQHandler (15, &DMA1);
}

/**
  \fn          void  DMA1_IRQ16Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ16
*/
void DMA1_IRQ16Handler (void)
{
    DMA_IRQHandler (16, &DMA1);
}

/**
  \fn          void  DMA1_IRQ17Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ17
*/
void DMA1_IRQ17Handler (void)
{
    DMA_IRQHandler (17, &DMA1);
}

/**
  \fn          void  DMA1_IRQ18Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ18
*/
void DMA1_IRQ18Handler (void)
{
    DMA_IRQHandler (18, &DMA1);
}

/**
  \fn          void  DMA1_IRQ19Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ19
*/
void DMA1_IRQ19Handler (void)
{
    DMA_IRQHandler (19, &DMA1);
}

/**
  \fn          void  DMA1_IRQ20Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ20
*/
void DMA1_IRQ20Handler (void)
{
    DMA_IRQHandler (20, &DMA1);
}

/**
  \fn          void  DMA1_IRQ21Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ21
*/
void DMA1_IRQ21Handler (void)
{
    DMA_IRQHandler (21, &DMA1);
}

/**
  \fn          void  DMA1_IRQ22Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ22
*/
void DMA1_IRQ22Handler (void)
{
    DMA_IRQHandler (22, &DMA1);
}

/**
  \fn          void  DMA1_IRQ23Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ23
*/
void DMA1_IRQ23Handler (void)
{
    DMA_IRQHandler (23, &DMA1);
}

/**
  \fn          void  DMA1_IRQ24Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ24
*/
void DMA1_IRQ24Handler (void)
{
    DMA_IRQHandler (24, &DMA1);
}

/**
  \fn          void  DMA1_IRQ25Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ25
*/
void DMA1_IRQ25Handler (void)
{
    DMA_IRQHandler (25, &DMA1);
}

/**
  \fn          void  DMA1_IRQ26Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ26
*/
void DMA1_IRQ26Handler (void)
{
    DMA_IRQHandler (26, &DMA1);
}

/**
  \fn          void  DMA1_IRQ27Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ27
*/
void DMA1_IRQ27Handler (void)
{
    DMA_IRQHandler (27, &DMA1);
}

/**
  \fn          void  DMA1_IRQ28Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ28
*/
void DMA1_IRQ28Handler (void)
{
    DMA_IRQHandler (28, &DMA1);
}

/**
  \fn          void  DMA1_IRQ29Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ29
*/
void DMA1_IRQ29Handler (void)
{
    DMA_IRQHandler (29, &DMA1);
}

/**
  \fn          void  DMA1_IRQ30Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ30
*/
void DMA1_IRQ30Handler (void)
{
    DMA_IRQHandler (30, &DMA1);
}

/**
  \fn          void  DMA1_IRQ31Handler (void)
  \brief       Run the IRQ Handler for DMA1-IRQ31
*/
void DMA1_IRQ31Handler (void)
{
    DMA_IRQHandler (31, &DMA1);
}

/**
  \fn          void  DMA1_AbortIRQHandler (void)
  \brief       Run the IRQ Handler for DMA1 Abort
*/
void DMA1_AbortIRQHandler (void)
{
    DMA_AbortIRQHandler (&DMA1);
}
/**
\brief Access structure of the DMA1 Driver.
*/
extern \
ARM_DRIVER_DMA Driver_DMA1;
ARM_DRIVER_DMA Driver_DMA1 = {
    DMA_GetVersion,
    DMA_GetCapabilities,
    DMA1_Initialize,
    DMA1_Uninitialize,
    DMA1_Allocate,
    DMA1_Control,
    DMA1_Start,
    DMA1_Stop,
    DMA1_GetStatus,
    DMA1_DeAllocate
};
#endif

#if (RTE_DMA2)

/**
  \fn          int32_t DMA2_Initialize (void)
  \brief       Initialize DMA Interface.
  \return      \ref execution_status
*/
static int32_t DMA2_Initialize (void)
{
    DMA_DRV_INFO *dma = &DMA2;

    /* set the apb interface for accessing registers */
    if(dma->apb_intf)
        dma->intf_paddr = (DMA_TypeDef*)DMA2_NS_BASE;
    else
        dma->intf_paddr = (DMA_TypeDef*)DMA2_SE_BASE;

    return DMA_Initialize (dma);
}

/**
  \fn          int32_t DMA2_Uninitialize (void)
  \brief       Un-Initialize DMA Interface.
  \return      \ref execution_status
*/
static int32_t DMA2_Uninitialize (void)
{
    return DMA_Uninitialize (&DMA2);
}

/**
  \fn          int32_t DMA2_Allocate (DMA_Handle_Type *handle)
  \brief       Allocate Channel for transfer operation
  \param[in]   handle  Pointer to DMA channel number
  \return      \ref execution_status
*/
static int32_t DMA2_Allocate (DMA_Handle_Type *handle)
{
    return DMA_Allocate (handle, &DMA2);
}

/**
  \fn          int32_t DMA2_Control (DMA_Handle_Type *handle, uint32_t control, uint32_t arg)
  \brief       Control DMA Interface.
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   control Operation
  \param[in]   arg     Argument 1 of operation
  \return      common \ref execution_status and driver specific \ref dma_execution_status
*/
static int32_t DMA2_Control (DMA_Handle_Type *handle, uint32_t control, uint32_t arg)
{
    return DMA_Control (handle, control, arg, &DMA2);
}

/**
  \fn          int32_t DMA2_Start (int32_t *handle, ARM_DMA_PARAMS *params)
  \brief       Start a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   params  Pointer to DMA desc parameters
  \return      \ref execution_status
*/
static int32_t DMA2_Start (DMA_Handle_Type *handle, ARM_DMA_PARAMS *params)
{
    return DMA_Start (handle, params, &DMA2);
}

/**
  \fn          int32_t DMA_Stop (int32_t *handle)
  \brief       Stop a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \return      \ref execution_status
*/
static int32_t DMA2_Stop (DMA_Handle_Type *handle)
{
    return DMA_Stop (handle, &DMA2);
}

/**
  \fn          DMA2_GetStatus (int32_t *handle, uint32_t *count)
  \brief       Status of a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \param[in]   count  Pointer to pass transferred count
  \return      \ref execution_status
*/
static int32_t DMA2_GetStatus (DMA_Handle_Type *handle, uint32_t *count)
{
    return DMA_GetStatus (handle, count, &DMA2);
}

/**
  \fn          DMA2_DeAllocate (int32_t *handle)
  \brief       De-Allocate a DMA channel
  \param[in]   handle  Pointer to DMA channel number
  \return      \ref execution_status
*/
static int32_t DMA2_DeAllocate (DMA_Handle_Type *handle)
{
    return DMA_DeAllocate (handle, &DMA2);
}

/**
  \fn          void  DMA2_IRQ0Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ0
*/
void DMA2_IRQ0Handler (void)
{
    DMA_IRQHandler (0, &DMA2);
}

/**
  \fn          void  DMA2_IRQ1Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ1
*/
void DMA2_IRQ1Handler (void)
{
    DMA_IRQHandler (1, &DMA2);
}

/**
  \fn          void  DMA2_IRQ2Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ2
*/
void DMA2_IRQ2Handler (void)
{
    DMA_IRQHandler (2, &DMA2);
}

/**
  \fn          void  DMA2_IRQ3Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ3
*/
void DMA2_IRQ3Handler (void)
{
    DMA_IRQHandler (3, &DMA2);
}

/**
  \fn          void  DMA2_IRQ4Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ4
*/
void DMA2_IRQ4Handler (void)
{
    DMA_IRQHandler (4, &DMA2);
}

/**
  \fn          void  DMA2_IRQ5Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ5
*/
void DMA2_IRQ5Handler (void)
{
    DMA_IRQHandler (5, &DMA2);
}

/**
  \fn          void  DMA2_IRQ6Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ6
*/
void DMA2_IRQ6Handler (void)
{
    DMA_IRQHandler (6, &DMA2);
}

/**
  \fn          void  DMA2_IRQ7Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ7
*/
void DMA2_IRQ7Handler (void)
{
    DMA_IRQHandler (7, &DMA2);
}

/**
  \fn          void  DMA2_IRQ8Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ8
*/
void DMA2_IRQ8Handler (void)
{
    DMA_IRQHandler (8, &DMA2);
}

/**
  \fn          void  DMA2_IRQ9Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ9
*/
void DMA2_IRQ9Handler (void)
{
    DMA_IRQHandler (9, &DMA2);
}

/**
  \fn          void  DMA2_IRQ10Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ10
*/
void DMA2_IRQ10Handler (void)
{
    DMA_IRQHandler (10, &DMA2);
}

/**
  \fn          void  DMA2_IRQ11Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ11
*/
void DMA2_IRQ11Handler (void)
{
    DMA_IRQHandler (11, &DMA2);
}

/**
  \fn          void  DMA2_IRQ12Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ12
*/
void DMA2_IRQ12Handler (void)
{
    DMA_IRQHandler (12, &DMA2);
}

/**
  \fn          void  DMA2_IRQ13Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ13
*/
void DMA2_IRQ13Handler (void)
{
    DMA_IRQHandler (13, &DMA2);
}

/**
  \fn          void  DMA2_IRQ14Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ14
*/
void DMA2_IRQ14Handler (void)
{
    DMA_IRQHandler (14, &DMA2);
}

/**
  \fn          void  DMA2_IRQ15Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ15
*/
void DMA2_IRQ15Handler (void)
{
    DMA_IRQHandler (15, &DMA2);
}

/**
  \fn          void  DMA2_IRQ16Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ16
*/
void DMA2_IRQ16Handler (void)
{
    DMA_IRQHandler (16, &DMA2);
}

/**
  \fn          void  DMA2_IRQ17Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ17
*/
void DMA2_IRQ17Handler (void)
{
    DMA_IRQHandler (17, &DMA2);
}

/**
  \fn          void  DMA2_IRQ18Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ18
*/
void DMA2_IRQ18Handler (void)
{
    DMA_IRQHandler (18, &DMA2);
}

/**
  \fn          void  DMA2_IRQ19Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ19
*/
void DMA2_IRQ19Handler (void)
{
    DMA_IRQHandler (19, &DMA2);
}

/**
  \fn          void  DMA2_IRQ20Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ20
*/
void DMA2_IRQ20Handler (void)
{
    DMA_IRQHandler (20, &DMA2);
}

/**
  \fn          void  DMA2_IRQ21Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ21
*/
void DMA2_IRQ21Handler (void)
{
    DMA_IRQHandler (21, &DMA2);
}

/**
  \fn          void  DMA2_IRQ22Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ22
*/
void DMA2_IRQ22Handler (void)
{
    DMA_IRQHandler (22, &DMA2);
}

/**
  \fn          void  DMA2_IRQ23Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ23
*/
void DMA2_IRQ23Handler (void)
{
    DMA_IRQHandler (23, &DMA2);
}

/**
  \fn          void  DMA2_IRQ24Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ24
*/
void DMA2_IRQ24Handler (void)
{
    DMA_IRQHandler (24, &DMA2);
}

/**
  \fn          void  DMA2_IRQ25Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ25
*/
void DMA2_IRQ25Handler (void)
{
    DMA_IRQHandler (25, &DMA2);
}

/**
  \fn          void  DMA2_IRQ26Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ26
*/
void DMA2_IRQ26Handler (void)
{
    DMA_IRQHandler (26, &DMA2);
}

/**
  \fn          void  DMA2_IRQ27Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ27
*/
void DMA2_IRQ27Handler (void)
{
    DMA_IRQHandler (27, &DMA2);
}

/**
  \fn          void  DMA2_IRQ28Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ28
*/
void DMA2_IRQ28Handler (void)
{
    DMA_IRQHandler (28, &DMA2);
}

/**
  \fn          void  DMA2_IRQ29Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ29
*/
void DMA2_IRQ29Handler (void)
{
    DMA_IRQHandler (29, &DMA2);
}

/**
  \fn          void  DMA2_IRQ30Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ30
*/
void DMA2_IRQ30Handler (void)
{
    DMA_IRQHandler (30, &DMA2);
}

/**
  \fn          void  DMA2_IRQ31Handler (void)
  \brief       Run the IRQ Handler for DMA2-IRQ31
*/
void DMA2_IRQ31Handler (void)
{
    DMA_IRQHandler (31, &DMA2);
}

/**
  \fn          void  DMA2_AbortIRQHandler (void)
  \brief       Run the IRQ Handler for DMA2 Abort
*/
void DMA2_AbortIRQHandler (void)
{
    DMA_AbortIRQHandler (&DMA2);
}
/**
\brief Access structure of the DMA2 Driver.
*/
extern \
ARM_DRIVER_DMA Driver_DMA2;
ARM_DRIVER_DMA Driver_DMA2 = {
    DMA_GetVersion,
    DMA_GetCapabilities,
    DMA2_Initialize,
    DMA2_Uninitialize,
    DMA2_Allocate,
    DMA2_Control,
    DMA2_Start,
    DMA2_Stop,
    DMA2_GetStatus,
    DMA2_DeAllocate
};
#endif
