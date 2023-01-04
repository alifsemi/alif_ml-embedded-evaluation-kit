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
 * @file     DMA_dev.h
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     04-Nov-2020
 * @brief    Device Info header.
 * @bug      None.
 * @Note     None
 ******************************************************************************/
#ifndef DMA_DEV_H

#define DMA_DEV_H
/* Includes ------------------------------------------------------------------*/
#include "Driver_DMA.h"
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include <DMA_Opcode.h>

#include <stdbool.h>

#ifdef  __cplusplus
extern "C"
{
#endif

#define DMA_MAX_CHANNELS     8                /*!< Max 8 channels supported */
#define DMA_MAX_PERI_REQ     32               /*!< Max 32 peripheral requests supported */
#define DMA_MAX_EVENTS       32               /*!< Max 32 events supported */
#define DMA_MAX_IRQ          32               /*!< Max 32 irqs supported */
#define DMA_MAX_BURST_LEN    16               /*!< Max 16 xfers are supported */
#define DMA_MAX_BURST_SIZE   8                /*!< Max 8bytes(DATA_WIDTH/8) */
#define DMA_MAX_BUFF_DEPTH   128              /*!< 32lines * 4bytes for each FIFO */
#define DMA_MAX_LP_CNT       256              /*!< Max Loop count */
#define DMA_NUM_LP_REGS      2                /*!< Max Loop registers */

#define DMA_IRQ_ABORT_OFFSET 32               /*!< Abort irq offset */

typedef enum {
    DMA_DRV_UNINITIALIZED,                         /*!< DMA driver UnInitialized */
    DMA_DRV_INIT_DONE,                             /*!< DMA Driver Initialized */
} DMA_DRV_FLAG_Type;

typedef enum {
    DMA_CHANNEL_USE_USER_MCODE = (1 << 0),         /*!< Use user provided mcode for channel */
    DMA_CHANNEL_I2S_MONO_MODE  = (1 << 1),         /*!< DMA channel in I2S mono mode */
} DMA_CHANNEL_FLAG_Type;

typedef enum {
    MANAGER = 0,                                   /*!< DMA Manager thread */
    CHANNEL,                                       /*!< DMA Channel thread */
} DMA_THREAD_TYPE;

typedef enum {
    STOPPED = 0,                                   /*!< Thread is Stopped */
    EXECUTING,                                     /*!< Thread is executing */
    CACHE_MISS,                                    /*!< Cache miss occurred */
    UPDATING_PC,                                   /*!< Thread is updating the program counter */
    WAITING_FOR_EVENT,                             /*!< Thread is waiting for an event */
    AT_BARRIER,                                    /*!< Thread is waiting at an barrier */
    WAITING_FOR_PERIPHERAL = 7,                    /*!< Thread is waiting for a peripheral */
    KILLING,                                       /*!< Thread is at killing state */
    COMPLETING,                                    /*!< Thread is at completing state */
    FAULTING_COMPLETING = 14,                      /*!< Thread is in faulting completing state */
    FAULTING = 15,                                 /*!< Thread is in Faulting State */
} DMA_THREAD_STATUS;

typedef union _DMA_DRV_STATUS {
    uint32_t status;                               /*!< DMA Driver status */
    ARM_DMA_STATUS status_b;                       /*!< DMA Driver status bits */
} DMA_DRV_STATUS;

typedef struct _DMA_DESC_INFO {
    ARM_DMA_SignalEvent_t cb_event;                /*!< DMA Application Event Callback associate with this transfer */
    ARM_DMA_DATA_DIR  direction;                   /*!< Direction of data transfer associated with this transaction */
    DMA_SECURE_STATUS sec_state;
    uint32_t          total_len;                   /*!< Number of bytes associated with this transaction */
    uint32_t          src_addr;                    /*!< Source address associated with this transaction */
    uint32_t          dst_addr;                    /*!< Destination address associated with this transaction */
    uint8_t           src_bsize;                   /*!< Src Burst Size associated with this transaction */
    uint8_t           src_blen;                    /*!< SrcBurst length associated with this transaction */
    uint8_t           dst_bsize;                   /*!< Dest Burst Size associated with this transaction */
    uint8_t           dst_blen;                    /*!< Dest Burst length associated with this transaction */
    int8_t            peri_num;                    /*!< peripheral number associated with this transaction */
} DMA_DESC_INFO;

typedef struct _DMA_CHANNEL_INFO {
    uint8_t           flags;                       /*!< Channel flags */
    bool              last_req;
    int8_t            chnl_num;                    /*!< Channel number associated with this transaction */
    IRQn_Type         irq;                         /*!< IRQ numbers associated with this transaction */
    int8_t            evnt_index;                  /*!< Event/IRQ index associated with this transaction */
    DMA_DESC_INFO     desc_info;                   /*!< DMA descriptor information associated with this transaction */
} DMA_CHANNEL_INFO;

typedef struct _DMA_THREAD_INFO {
    DMA_THREAD_STATUS     curr_state;               /*!< Current state of the thread */
    DMA_CHANNEL_INFO      channel_info;             /*!< Channel information associated with this transaction */
    uint8_t               dma_mcode[DMA_MCODE_SIZE];/*!< DMA microcode built for this transaction */
    void                  *user_mcode;              /*!< User provided mcode */
    bool                  in_use;                   /*!< Status of DMA thread being used  */
} DMA_THREAD_INFO;

typedef struct _DMA_CONFIG_INFO {
    DMA_THREAD_INFO    channel_thread[DMA_MAX_CHANNELS];/*!< DMA Channel thread info */
    int8_t             event[DMA_MAX_EVENTS];           /*!< DMA events to channel utilization mapping */
} DMA_CONFIG_INFO;

/* Fault Type DMA  Manager Register */
typedef union {
    uint32_t value;
    struct {
        uint32_t undef_instr     : 1;     /*!< Undefined Instruction */
        uint32_t operand_invalid : 1;     /*!< Invalid arguments passed */
        uint32_t RESERVED2       : 2;     /*!< Reserved */
        uint32_t dmago_err       : 1;     /*!< Security aspect while executing DMAGO */
        uint32_t mgr_evnt_err    : 1;     /*!< security aspect while executing DMAWFE or DMASEV */
        uint32_t RESERVED3       : 10;    /*!< Reserved */
        uint32_t instr_fetch_err : 1;     /*!< Instr fetch error on AXI */
        uint32_t RESERVED4       : 13;    /*!< Reserved */
        uint32_t dbg_instr       : 1;     /*!< Instr read from system or debug interface */
    } FTRD_b;
} DMA_FTRD_Type;

/* Fault Type DMA  Channel Register */
typedef union {
    uint32_t value;
    struct {
        uint32_t undef_instr     : 1;     /*!< Undefined Instruction */
        uint32_t operand_invalid : 1;     /*!< Invalid arguments passed */
        uint32_t RESERVED6       : 3;     /*!< Reserved */
        uint32_t ch_evnt_err     : 1;     /*!< security aspect while executing DMAWFE or DMASEV */
        uint32_t ch_periph_err   : 1;     /*!< security aspect while executing DMAWFP/DMALDP/DMASTP/DMAFLUSHP */
        uint32_t ch_rdwr_err     : 1;     /*!< security aspect while programming CCRn */
        uint32_t RESERVED7       : 4;     /*!< Reserved */
        uint32_t mfifo_err       : 1;     /*!< mfifo prevented channel from executing DMALD/DMAST */
        uint32_t no_st_data      : 1;     /*!< mfifo didn't contain data to perform DMAST */
        uint32_t RESERVED8       : 2;     /*!< Reserved */
        uint32_t instr_fetch_err : 1;     /*!< AXI response on inst fetch */
        uint32_t data_write_err  : 1;     /*!< AXI response on data write */
        uint32_t data_read_err   : 1;     /*!< AXI response on data read */
        uint32_t RESERVED9       : 11;    /*!< Reserved */
        uint32_t dbg_instr       : 1;     /*!< Instr read from system or debug interface */
        uint32_t lockup_err      : 1;     /*!< channel lockedup due to resource starvation */
    } FTR_b;
} DMA_FTR_Type;

typedef union {
    uint32_t value;
    struct {
        uint32_t dbg_thrd             : 1;     /*!< Debug Manager/Channel Thread */
        uint32_t reserved0            : 7;     /*!< Reserved */
        uint32_t chn_num              : 3;     /*!< Channel number */
        uint32_t reserved1            : 5;     /*!< Reserved */
        uint32_t ins_byte0            : 8;     /*!< Instruction byte 0 */
        uint32_t ins_byte1            : 8;     /*!< Instruction byte 1 */
    } value_b;
} DMA_DBGINST0_Type;

typedef union {
    uint32_t value;
    struct {
        uint32_t ins_byte2            : 8;     /*!< Instruction byte 2 */
        uint32_t ins_byte3            : 8;     /*!< Instruction byte 3 */
        uint32_t ins_byte4            : 8;     /*!< Instruction byte 4 */
        uint32_t ins_byte5            : 8;     /*!< Instruction byte 5 */
    } value_b;
} DMA_DBGINST1_Type;

/* Register fields and masks */

/* DMA DSR: Status of Manager thread */
#define DMA_DSR_DMA_STATUS_Pos               0U
#define DMA_DSR_DMA_STATUS_Msk               (0xFUL << DMA_DSR_DMA_STATUS_Pos)
#define DMA_DSR_WAKEUP_EVENT_Pos             4U
#define DMA_DSR_WAKEUP_EVENT_Msk             (0x1FUL << DMA_DSR_WAKEUP_EVENT_Pos)
#define DMA_DSR_DNS_Pos                      9U
#define DMA_DSR_DNS_Msk                      (0x1UL << DMA_DSR_DNS_Pos)

/* DMA FSRD: Fault Status of Manager */
#define DMA_FSRD_FAULT_STATUS_MANAGER_Pos    0U
#define DMA_FSRD_FAULT_STATUS_MANAGER_Msk    (0x1UL << DMA_FSRD_FAULT_STATUS_MANAGER_Pos)

/* DMA FSRC: Fault Status of Channels */
#define DMA_FSRC_FAULT_STATUS_Pos            0U
#define DMA_FSRC_FAULT_STATUS_Msk            (0xFFUL << DMA_FSRC_FAULT_STATUS_Pos)

/* DMA CSR: Channel Status */
#define DMA_CSR_CHANNEL_STATUS_Pos           0U
#define DMA_CSR_CHANNEL_STATUS_Msk           (0xFUL << DMA_CSR_CHANNEL_STATUS_Pos)
#define DMA_CSR_WAKEUP_NUM_Pos               4U
#define DMA_CSR_WAKEUP_NUM_Msk               (0x1FUL << DMA_CSR_WAKEUP_NUM_Pos)
#define DMA_CSR_WFP_B_OR_S_OPERAND_Pos       14U
#define DMA_CSR_WFP_B_OR_S_OPERAND_Msk       (0x1UL << DMA_CSR_WFP_B_OR_S_OPERAND_Pos)
#define DMA_CSR_WFP_PERIPH_OPERAND_Pos       15U
#define DMA_CSR_WFP_PERIPH_OPERAND_Msk       (0x1UL << DMA_CSR_WFP_PERIPH_OPERAND_Pos)
#define DMA_CSR_CHANNEL_NONSEC_Pos           21U
#define DMA_CSR_CHANNEL_NONSEC_Msk           (0x1UL << DMA_CSR_CHANNEL_NONSEC_Pos)

/* DMA DBGSTATUS: Debug Status */
#define DMA_DBGSTATUS_DBGSTATUS_Pos          0U
#define DMA_DBGSTATUS_DBGSTATUS_Msk          (0x1UL << DMA_DBGSTATUS_DBGSTATUS_Pos)

/* DMA DBGCMD: Debug Command */
#define DMA_DBGCMD_DBGCMD_Pos                0U
#define DMA_DBGCMD_DBGCMD_Msk                (0x3UL << DMA_DBGCMD_DBGCMD_Pos)

/* DMA WD.IRQ.ONLY: Watchdog IRQ Only */
#define DMA_WD_IRQ_ONLY_Pos                  0U
#define DMA_WD_IRQ_ONLY_Msk                  (0x1UL << DMA_WD_IRQ_ONLY_Pos)

typedef struct
{
    __IM uint32_t DSR;                         /*!< 0x0 Manager Status Register */
    __IM uint32_t DPC;                         /*!< 0x4 Manager Program Counter Register */
    uint32_t RESERVED1[6];                     /*!< 0x8-0x1C */
    __IOM uint32_t INTEN;                      /*!< 0x20 Interrupt Enable */
    __IM uint32_t INT_EVENT_RIS;               /*!< 0x24 Event-Interrupt Raw-Status */
    __IM uint32_t INTMIS;                      /*!< 0x28 Status of Active Interrupts */
    __OM uint32_t INTCLR;                      /*!< 0x2C Interrupt Clear */
    __IM uint32_t FSRD;                        /*!< 0x30 Fault Status of Manager */
    __IM uint32_t FSRC;                        /*!< 0x34 Fault Status of Channels */
    __IM uint32_t FTRD;                        /*!< 0x38 Fault Type of Manager */
    uint32_t RESERVED5;                        /*!< 0x3C */
    __IM uint32_t FTR[DMA_MAX_CHANNELS];       /*!< 0x40 - 0x5C Fault Type of Channeln */
    uint32_t RESERVED10[40];                   /*!< 0x60 - 0xFC */
    struct {
        __IM uint32_t CSR;                     /*!< 0x0 Status register of Channeln */
        __IM uint32_t CPC;                     /*!< 0x4 Program Counter of Channeln */
    } CHANNEL_RT_INFO[DMA_MAX_CHANNELS];       /*!< 0x100 - 0x13C */
    uint32_t RESERVED13[176];                  /*!< 0x140 - 0x3FC */
    struct {
        __IM uint32_t SAR;                     /*!< 0x0 Source Address of Channeln */
        __IM uint32_t DAR;                     /*!< 0x4 Destination Address of Channeln */
        __IM DMA_CCR_Type CCR;                 /*!< 0x8 Channel Control of Channeln */
        __IM uint32_t LC0;                     /*!< 0xC Loop Control 0 of Channeln */
        __IM uint32_t LC1;                     /*!< 0x10 Loop Control 1 of Channeln */
        uint32_t RESERVED14[3];                /*!< 12bytes hole */
    } RT_CHANNEL_CFG[DMA_MAX_CHANNELS];        /*!< 0x400 - 0x4F0 */
    uint32_t RESERVED15[512];                  /*!< 0x500 - 0xCFC */
    __IM uint32_t DBGSTATUS;                   /*!< 0xD00 Debug Status */
    __OM uint32_t DBGCMD;                      /*!< 0xD04 Debug Command */
    __OM uint32_t DBGINST0;                    /*!< 0xD08 Debug Instruction0 */
    __OM uint32_t DBGINST1;                    /*!< 0xD0C Debug Instruction1 */
    uint32_t RESERVED18[92];                   /*!< 0xD10 - 0xE7C */
    __IOM uint32_t WD;                         /*!< 0xE80 Watchdog */
} DMA_TypeDef;

typedef struct _DMA_DRV_INFO {
    DMA_CONFIG_INFO       *cfg;                    /*!< DMA Controller configuration */
    DMA_SECURE_STATUS     apb_intf;                /*!< DMA interface to be used */
    DMA_TypeDef           *intf_paddr;             /*!< DMA interface physical address */
    DMA_DRV_STATUS        drv_status;              /*!< DMA Driver Status */
    uint8_t               flags;                   /*!< DMA Driver Flags */
    const IRQn_Type       irq_start;               /*!< DMA irq0 start index */
    const uint32_t        abort_irq_priority;      /*!< DMA Abort IRQ priority */
} DMA_DRV_INFO;

#ifdef  __cplusplus
}
#endif

#endif /* DMA_DEV_H */
