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
 * @file     DMA_Opcode.h
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     01-Aug-2022
 * @brief    API exposed for Opcode generation
 * @bug      None.
 * @Note     None
 ******************************************************************************/
#ifndef DMA_OPCODE_H
#define DMA_OPCODE_H
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#ifdef  __cplusplus
extern "C"
{
#endif

#define DMA_CACHE_SUPPORT    0x2              /*!< Cache set to Non-Cacheable & Modifiable */
#define DMA_NO_SWAP          0                /*!< Swapping is not supported */
#define DMA_MCODE_SIZE       256              /*!< Max Code bytes, which can be configured */

/* Loop count registers */
typedef enum {
    LC0,
    LC1,
} DMA_LC_Type;

typedef enum {
    DMA_BURST_FIXED = 0,                           /*!< Fixed Address burst */
    DMA_BURST_INCREMENTING,                        /*!< Incrementing Address burst */
} DMA_BURST_TYPE;

/* DMA transfer type */
typedef enum {
    SINGLE = 0,
    BURST = 1,
    PERIPHERAL = 2,
    FORCE = 2,
} DMA_XFER_Type;

typedef struct _DMA_LOOP_ARGS {
    DMA_XFER_Type xfer_type;
    DMA_LC_Type lc;
    uint8_t jump;
    bool nf;
} DMA_LOOP_ARGS;

typedef union {
    uint32_t value;
    struct {
        uint32_t src_inc              : 1;     /*!< Fixed/Increment type burst */
        uint32_t src_burst_size       : 3;     /*!< No of bytes DMAC reads from source in a beat */
        uint32_t src_burst_len        : 4;     /*!< No of data transfers in a burst read from source */
        uint32_t src_prot_ctrl        : 3;     /*!< Protection control when reading from source */
        uint32_t src_cache_ctrl       : 3;     /*!< Cache control when reading from source */
        uint32_t dst_inc              : 1;     /*!< Fixed/Increment type burst */
        uint32_t dst_burst_size       : 3;     /*!< No of bytes DMAC writes to destination in a beat */
        uint32_t dst_burst_len        : 4;     /*!< No of data transfers in a burst DMAC writes to destination */
        uint32_t dst_prot_ctrl        : 3;     /*!< Protection control when writing to destination */
        uint32_t dst_cache_ctrl       : 3;     /*!< Cache control when writing to destination */
        uint32_t endian_swap_size     : 3;     /*!< Cache control when writing to destination */
    } value_b;
} DMA_CCR_Type;

typedef enum {
    SECURE = 0,                                    /*!< Secure State */
    NON_SECURE,                                    /*!< Non-Secure State */
} DMA_SECURE_STATUS;

/* DMA registers */
typedef enum {
    SAR,
    CCR,
    DAR
} DMA_REG_Type;

/**
  \fn          bool DMA_ConstructAdd (DMA_REG_Type reg, uint16_t off,
                                      uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMAADDH
  \param[in]   reg Source or Destination Address Register
  \param[in]   off 16bit-immediate offset which needs to be added
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructAdd (DMA_REG_Type reg, uint16_t off,
                       uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructAddNeg (DMA_REG_Type reg, int16_t off,
                                         uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMAADNH
  \param[in]   reg Source or Destination Address Register
  \param[in]   off 16bit-immediate value which needs to be subtracted
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructAddNeg (DMA_REG_Type reg, int16_t off,
                          uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructEnd (uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMAEND
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructEnd (uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructFlushPeri (int8_t peri,
                                            uint8_t *addr,
                                            uint16_t *idx)
  \brief       Build the opcode for DMAFLUSHP
  \param[in]   peri peripheral number
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructFlushPeri (int8_t peri, uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructGo (bool ns, uint8_t cns,
                                     uint32_t imm, uint8_t *addr)
  \brief       Build the opcode for DMAGO
  \param[in]   ns Defines the secure/Non-Secure State
  \param[in]   cns Defines the channel number
  \param[in]   imm 32bit address where the microcode resides
  \param[in]   addr instruction base address in memory
  \return      void
*/
void DMA_ConstructGo (DMA_SECURE_STATUS ns, uint8_t cns,
                      uint32_t imm, uint8_t *addr);

/**
  \fn          void DMA_ConstructKill (uint8_t *addr)
  \brief       Build the opcode for DMAKILL
  \param[in]   addr start address of microcode in memory
  \return      void
*/
void DMA_ConstructKill (uint8_t *addr);

/**
  \fn          bool DMA_ConstructLoad (DMA_XFER_Type xfer_type, uint8_t *addr,
                                       uint16_t *idx)
  \brief       Build the opcode for DMALD
  \param[in]   xfer_type Burst/Single/Force Load operation
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructLoad (DMA_XFER_Type xfer_type, uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructLoadPeri (DMA_XFER_Type xfer_type, int8_t peri,
                                           uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMALDP
  \param[in]   xfer_type Burst or Single Load operation
  \param[in]   peri Peripheral number
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructLoadPeri (DMA_XFER_Type xfer_type, int8_t peri,
                            uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructLoop (DMA_LC_Type lc, uint8_t iter,
                                       uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMALP
  \param[in]   lc Loop Counter register number
  \param[in]   iter 8bit loop value
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructLoop (DMA_LC_Type lc, uint8_t iter,
                        uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructLoopEnd (DMA_LOOP_ARGS lp_args, uint8_t *addr,
                                          uint16_t *idx)
  \brief       Build the opcode for DMALPEND
  \param[in]   lp_args loop arguments single/burst/force, lc, nf, jump
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructLoopEnd (DMA_LOOP_ARGS *lp_args, uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructMove (uint32_t imm, DMA_REG_Type reg,
                                       uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMAMOV
  \param[in]   imm 32bit immediate address
  \param[in]   reg SAR/CCR/DAR register
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructMove (uint32_t imm, DMA_REG_Type reg, uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructNop (uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMANOP
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructNop (uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructReadBarrier (uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMARMB
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructReadBarrier (uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructSendEvent (uint8_t event_num,
                                            uint8_t *addr,
                                            uint16_t *idx)
  \brief       Build the opcode for DMASEV
  \param[in]   event_num Event number
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructSendEvent (uint8_t event_num, uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructStore (DMA_XFER_Type xfer_type, uint8_t *addr,
                                        uint16_t *idx)
  \brief       Build the opcode for DMAST
  \param[in]   xfer_type Burst/Single/Force Load operation
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructStore (DMA_XFER_Type xfer_type, uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructStorePeri (DMA_XFER_Type xfer_type, int8_t peri,
                                            uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMASTP
  \param[in]   xfer_type Burst or Single Store operation
  \param[in]   peri Peripheral number
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructStorePeri (DMA_XFER_Type xfer_type, int8_t peri,
                             uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructStoreZeros (uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMASTZ
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructStoreZeros (uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructWaitforEvent (bool invalidate,
                                               uint8_t event_num,
                                               uint8_t *addr,
                                               uint16_t *idx)
  \brief       Build the opcode for DMAWFE
  \param[in]   invalidate Set for invalidating the Cache
  \param[in]   event_num Event number
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructWaitforEvent (bool invalidate, uint8_t event_num,
                                uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructWaitforPeri (DMA_XFER_Type xfer_type,
                                              int8_t peri_num,
                                              uint8_t *addr,
                                              uint16_t *idx)
  \brief       Build the opcode for DMAWFE
  \param[in]   xfer_type Single/Burst/Peripheral
  \param[in]   peri_num Peripheral number
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructWaitforPeri (DMA_XFER_Type xfer_type, int8_t peri_num,
                               uint8_t *addr, uint16_t *idx);

/**
  \fn          bool DMA_ConstructWriteBarrier (uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMAWMB
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructWriteBarrier (uint8_t *addr, uint16_t *idx);

/**
  \fn          void DMA_ConstructKill (uint8_t *addr)
  \brief       Build the opcode for DMAKILL
  \param[in]   addr start address of microcode in memory
  \return      void
*/
void DMA_ConstructKill (uint8_t *addr);

/**
  \fn          bool DMA_ConstructGo (bool ns, uint8_t cns,
                                     uint32_t imm, uint8_t *addr)
  \brief       Build the opcode for DMAGO
  \param[in]   ns Defines the secure/Non-Secure State
  \param[in]   cns Defines the channel number
  \param[in]   imm 32bit address where the microcode resides
  \param[in]   addr instruction base address in memory
  \return      void
*/
void DMA_ConstructGo (DMA_SECURE_STATUS ns, uint8_t cns,
                      uint32_t imm, uint8_t *addr);

#ifdef  __cplusplus
}
#endif

#endif /* DMA_OPCODE_H */
