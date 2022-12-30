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
 * @file     DMA_Opcode.c
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     01-Aug-2022
 * @brief    Opcode APIs
 * @bug      None.
 * @Note     None
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "RTE_Components.h"
#include CMSIS_device_header
#include "DMA_Opcode.h"

/* 8 bit-opcode with variable data payload of 0, 8, 16 or 32bits */
#define OP_1BYTE_LEN         1
#define OP_2BYTE_LEN         2
#define OP_3BYTE_LEN         3
#define OP_6BYTE_LEN         6

#define OP_DMAADDH(ar)       (0x54 | (ar << 1))             /*!< Adds an immediate 16bit value to SARn or DARn */
#define OP_DMAADNH(ar)       (0x5C | (ar << 1))             /*!< Adds an immediate 16bit negative value to SARn or DARn */
#define OP_DMAEND            0x00                           /*!< End signal as the DMAC sequence is complete */
#define OP_DMAFLUSHP         0x35                           /*!< Flush the peripheral contents and sends message to resend its level status */
#define OP_DMAGO(ns)         (0xA0 | (ns << 1))             /*!< Execute thread in secure/non-secure mode */
#define OP_DMAKILL           0x01                           /*!< Terminate Execution of a thread */
#define OP_DMALD             0x04                           /*!< Performs DMA Load operation */
#define OP_DMALDS            0x05                           /*!< Performs DMA Single Load operation */
#define OP_DMALDB            0x07                           /*!< Performs DMA Burst Load operation */
#define OP_DMALDP(bs)        (0x25 | (bs << 1))             /*!< Performs DMA Load & Notify Peripheral Single/Burst operation */
#define OP_DMALP(lc)         (0x20 | (lc << 1))             /*!< Loop instruct DMAC to load 8bit val to LC0/LC1 reg */
#define OP_DMALPEND(nf, lc)  (0x28 | (nf << 4) | (lc << 2)) /*!< Loop End, nf=0, lc=1 if DMALPFE started loop*/
#define OP_DMALPENDS(lc)     (0x39 | (lc << 2))             /*!< Loop End Single */
#define OP_DMALPENDB(lc)     (0x3B | (lc << 2))             /*!< Loop End Burst */
#define OP_DMAMOV            0xBC                           /*!< Move 32bit immediate into SAR/DAR/CCR */
#define OP_DMANOP            0x18                           /*!< For code alignment */
#define OP_DMARMB            0x12                           /*!< Read Memory barrier, write-after-read sequence */
#define OP_DMASEV            0x34                           /*!< Send event */
#define OP_DMAST             0x08                           /*!< Performs DMA Store operation */
#define OP_DMASTS            0x09                           /*!< Performs DMA Single Store operation */
#define OP_DMASTB            0x0B                           /*!< Performs DMA Burst Store operation */
#define OP_DMASTP(bs)        (0x29 | (bs << 1))             /*!< Performs DMA Store & Notify Peripheral Single/Burst operation */
#define OP_DMASTZ            0x0C                           /*!< Store Zeros */
#define OP_DMAWFE            0x36                           /*!< Wait for event */
#define OP_DMAWFP_P(p)       (0x30 | (p << 0))              /*!< Wait for peripheral with peripheral bit set */
#define OP_DMAWFP(bs)        (0x30 | (bs << 1))             /*!< Wait for peripheral in single/burst mode */
#define OP_DMAWMB            0x13                           /*!< Write memory barrier */

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
                       uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_3BYTE_LEN) > DMA_MCODE_SIZE)
        return false;

    if (reg == SAR)
      addr[(*idx)++] = OP_DMAADDH(0);
    else if (reg == DAR)
      addr[(*idx)++] = OP_DMAADDH(1);
    else
      return false;

    addr[(*idx)++] = off;
    addr[(*idx)++] = off >> 8;

    return true;
}

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
                          uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_3BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    if (reg == SAR)
      addr[(*idx)++] = OP_DMAADNH(0);
    else if (reg == DAR)
      addr[(*idx)++] = OP_DMAADNH(1);
    else
      return false;

    off = off - 1;
    off = ~off;
    addr[(*idx)++] = off;
    addr[(*idx)++] = off >> 8;

    return true;
}

/**
  \fn          bool DMA_ConstructEnd (uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMAEND
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructEnd (uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_1BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    addr[(*idx)++] = OP_DMAEND;

    return true;
}

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
bool DMA_ConstructFlushPeri (int8_t peri, uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_2BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    addr[(*idx)++] = OP_DMAFLUSHP;
    peri = peri & 0x1F;
    addr[(*idx)++] = (peri << 3);

    return true;
}

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
                      uint32_t imm, uint8_t *addr)
{
    uint8_t idx = 0;

    addr[idx++] = OP_DMAGO(ns);
    addr[idx++] = cns & 0x7;
    addr[idx++] = imm;
    addr[idx++] = imm >> 8;
    addr[idx++] = imm >> 16;
    addr[idx++] = imm >> 24;
}

/**
  \fn          void DMA_ConstructKill (uint8_t *addr)
  \brief       Build the opcode for DMAKILL
  \param[in]   addr start address of microcode in memory
  \return      void
*/
void DMA_ConstructKill (uint8_t *addr)
{
    *addr = OP_DMAKILL;
}

/**
  \fn          bool DMA_ConstructLoad (DMA_XFER_Type xfer_type, uint8_t *addr,
                                       uint16_t *idx)
  \brief       Build the opcode for DMALD
  \param[in]   xfer_type Burst/Single/Force Load operation
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructLoad (DMA_XFER_Type xfer_type, uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_1BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    if (xfer_type == FORCE)
      addr[(*idx)++] = OP_DMALD;
    else if (xfer_type == BURST)
      addr[(*idx)++] = OP_DMALDB;
    else
      addr[(*idx)++] = OP_DMALDS;

    return true;
}

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
                            uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_2BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    if (xfer_type > BURST)
      return false;

    addr[(*idx)++] = OP_DMALDP(xfer_type);
    peri = peri & 0x1F;
    addr[(*idx)++] = (peri << 3);

    return true;
}

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
                        uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_2BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    addr[(*idx)++] = OP_DMALP(lc);
    addr[(*idx)++] = iter - 1;

    return true;
}

/**
  \fn          bool DMA_ConstructLoopEnd (DMA_LOOP_ARGS lp_args, uint8_t *addr,
                                          uint16_t *idx)
  \brief       Build the opcode for DMALPEND
  \param[in]   lp_args loop arguments single/burst/force, lc, nf, jump
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructLoopEnd (DMA_LOOP_ARGS *lp_args, uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_2BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    if (lp_args->nf == 0)
      addr[(*idx)++] = OP_DMALPEND(0, 1);
    else if (lp_args->xfer_type == FORCE)
      addr[(*idx)++] = OP_DMALPEND(1, lp_args->lc);
    else  if (lp_args->xfer_type == BURST)
      addr[(*idx)++] = OP_DMALPENDB(lp_args->lc);
    else
      addr[(*idx)++] = OP_DMALPENDS(lp_args->lc);

    addr[(*idx)++] = lp_args->jump;

    return true;
}

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
bool DMA_ConstructMove (uint32_t imm, DMA_REG_Type reg,
                        uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_6BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    addr[(*idx)++] = OP_DMAMOV;
    addr[(*idx)++] = reg & 0x7;
    addr[(*idx)++] = imm;
    addr[(*idx)++] = imm >> 8;
    addr[(*idx)++] = imm >> 16;
    addr[(*idx)++] = imm >> 24;

    return true;
}

/**
  \fn          bool DMA_ConstructNop (uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMANOP
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructNop (uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_1BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    addr[(*idx)++] = OP_DMANOP;

    return true;
}

/**
  \fn          bool DMA_ConstructReadBarrier (uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMARMB
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructReadBarrier (uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_1BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    addr[(*idx)++] = OP_DMARMB;

    return true;
}

/**
  \fn          bool DMA_ConstructSendEvent (uint8_t event_num, uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMASEV
  \param[in]   event_num Event number
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructSendEvent (uint8_t event_num, uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_2BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    addr[(*idx)++] = OP_DMASEV;
    event_num = event_num & 0x1F;
    addr[(*idx)++] = event_num << 3;

    return true;
}

/**
  \fn          bool DMA_ConstructStore (DMA_XFER_Type xfer_type, uint8_t *addr,
                                        uint16_t *idx)
  \brief       Build the opcode for DMAST
  \param[in]   xfer_type Burst/Single/Force Load operation
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructStore (DMA_XFER_Type xfer_type, uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_1BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    if (xfer_type == FORCE)
      addr[(*idx)++] = OP_DMAST;
    else if (xfer_type == BURST)
      addr[(*idx)++] = OP_DMASTB;
    else
      addr[(*idx)++] = OP_DMASTS;

    return true;
}

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
                             uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_2BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    if (xfer_type > BURST)
      return false;

    addr[(*idx)++] = OP_DMASTP(xfer_type);
    peri = peri & 0x1F;
    addr[(*idx)++] = (peri << 3);

    return true;
}

/**
  \fn          bool DMA_ConstructStoreZeros (uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMASTZ
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructStoreZeros (uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_1BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    addr[(*idx)++] = OP_DMASTZ;

    return true;
}

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
                                uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_2BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    addr[(*idx)++] = OP_DMAWFE;
    event_num = event_num & 0x1F;
    addr[(*idx)++] = (event_num << 3) | (invalidate << 1);

    return true;
}

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
                               uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_2BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    if (xfer_type == PERIPHERAL)
      addr[(*idx)++] = OP_DMAWFP_P(1);
    else
      addr[(*idx)++] = OP_DMAWFP(xfer_type);

    peri_num = peri_num & 0x1F;
    addr[(*idx)++] = (peri_num << 3);

    return true;
}

/**
  \fn          bool DMA_ConstructWriteBarrier (uint8_t *addr, uint16_t *idx)
  \brief       Build the opcode for DMAWMB
  \param[in]   addr start address of microcode in memory
  \param[in]   idx offset from the start of microcode in memory
  \return      bool true if the opcode fits in the allocated space
*/
bool DMA_ConstructWriteBarrier (uint8_t *addr, uint16_t *idx)
{
    if ((*idx + OP_1BYTE_LEN) > DMA_MCODE_SIZE)
      return false;

    addr[(*idx)++] = OP_DMAWMB;

    return true;
}
