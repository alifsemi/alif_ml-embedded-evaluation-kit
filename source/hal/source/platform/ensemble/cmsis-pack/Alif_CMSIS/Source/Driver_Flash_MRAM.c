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
 * @file     Driver_Flash_MRAM.c
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     23-June-2022
 * @brief    ARM CMSIS-Driver for Flash(On-Chip NVM MRAM).
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

/* System Includes */
#include "string.h"

/* Project Includes */
#include "Flash_MRAM_dev.h"

#if !(RTE_FLASH_MRAM)
#error "FLASH_MRAM is not enabled in the RTE_Device.h"
#endif

#if (defined(RTE_Drivers_FLASH_MRAM) && !RTE_FLASH_MRAM)
#error "FLASH_MRAM not configured in RTE_Device.h!"
#endif

#define ARM_FLASH_MRAM_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */

/* Flash MRAM Driver Instance */
#ifndef FLASH_MRAM_INSTANCE
#define FLASH_MRAM_INSTANCE           0   /* default 0th instance for MRAM. */
#endif

/* Flash MRAM Driver Enable? */
#if (RTE_FLASH_MRAM)

/* Flash MRAM Information */
static ARM_FLASH_INFO flash_mram_info =
{
    FLASH_MRAM_SECTOR_INFO,    ///< Sector layout information (NULL=Uniform sectors)
    FLASH_MRAM_SECTOR_COUNT,   ///< Number of sectors
    FLASH_MRAM_SECTOR_SIZE,    ///< Uniform sector size in bytes (0=sector_info used)
    FLASH_MRAM_PAGE_SIZE,      ///< Optimal programming page size in bytes
    FLASH_MRAM_PROGRAM_UNIT,   ///< Smallest programmable unit in bytes
    FLASH_MRAM_ERASED_VALUE,   ///< Contents of erased memory
    { 0, 0, 0 }                ///< Reserved (must be zero)
};

/* Flash MRAM Device Resources. */
static FLASH_MRAM_DEV flash_mram =
{
    .info   = &flash_mram_info,
    .status = {0},
    .flags  = 0
};
#endif /* RTE_FLASH_MRAM */


/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,
    ARM_FLASH_MRAM_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    0, /* event_ready */
    0, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    0, /* erase_chip */
    0  /* reserved (must be zero) */
};


/**
  \fn          ARM_DRIVER_VERSION Flash_MRAM_GetVersion(void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION Flash_MRAM_GetVersion(void)
{
    return DriverVersion;
}

/**
  \fn          ARM_FLASH_CAPABILITIES Flash_MRAM_GetCapabilities(void)
  \brief       Get driver capabilities.
  \return      \ref ARM_FLASH_CAPABILITIES
*/
static ARM_FLASH_CAPABILITIES Flash_MRAM_GetCapabilities(void)
{
    return DriverCapabilities;
}

/**
  \fn          int32_t MRAM_Initialize(ARM_Flash_SignalEvent_t cb_event,
                                       FLASH_MRAM_DEV          *flash_mram)
  \brief       Initialize the MRAM Flash Interface.
  \param[in]   cb_event  Pointer to \ref ARM_Flash_SignalEvent
  \param[in]   flash_mram     Pointer to MRAM flash resources structure
  \return      \ref execution_status
*/
static int32_t MRAM_Initialize(ARM_Flash_SignalEvent_t cb_event,
                               FLASH_MRAM_DEV          *flash_mram)
{
    if (flash_mram->flags & FLASH_MRAM_FLAG_INIT)
        return ARM_DRIVER_OK;

    /* Validate User MRAM flash size. */
    if (FLASH_MRAM_SIZE > MRAM_SIZE)
        return ARM_DRIVER_ERROR;

    /* Do nothing. */
    flash_mram->flags = FLASH_MRAM_FLAG_INIT;

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t MRAM_Uninitialize(FLASH_MRAM_DEV *flash_mram)
  \brief       De-initialize the MRAM Flash Interface.
  \param[in]   flash_mram  Pointer to MRAM flash resources structure
  \return      \ref execution_status
*/
static int32_t MRAM_Uninitialize(FLASH_MRAM_DEV *flash_mram)
{
    /* Do nothing. */
    flash_mram->flags = 0;

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t MRAM_PowerControl(ARM_POWER_STATE state,
                                         FLASH_MRAM_DEV  *flash_mram)
  \brief       Control the MRAM Flash interface power.
  \param[in]   state  Power state
  \param[in]   flash_mram  Pointer to MRAM flash resources structure
  \return      \ref execution_status
*/
static int32_t MRAM_PowerControl(ARM_POWER_STATE state,
                                 FLASH_MRAM_DEV  *flash_mram)
{
    /* Do nothing. */

    switch (state)
    {
    case ARM_POWER_OFF:
        flash_mram->flags &= ~FLASH_MRAM_FLAG_POWER;
        break;

    case ARM_POWER_LOW:
        break;

    case ARM_POWER_FULL:
        if (!(flash_mram->flags & FLASH_MRAM_FLAG_INIT))
          return ARM_DRIVER_ERROR;

        if (flash_mram->flags & FLASH_MRAM_FLAG_POWER)
          break;

        flash_mram->flags |= FLASH_MRAM_FLAG_POWER;
        break;

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          void write_128bit(uint8_t *p_dst, const uint8_t *p_src)
  \brief       write 128-bit data from source to destination(MRAM).
  \Note        It is CRITICAL that this code running on the Application core
                contains the following:
                 - H/W Limitations with Rev A silicon:
                    The code that is performing the MRAM writes
                     CANNOT be located and executing from MRAM.
                     I.E. the code must be executing from SRAM.
                    No code can be running from MRAM when you want to
                     write to the MRAM.
                 - The code should disable interrupts around the code
                    where the MRAM writes and cache flush occur.
                 - The code should include the function / Intrinsic “__DSB()”
                    to make sure all the data writes are flushed out
                    and have occurred.
  \param[out]  p_dst   Pointer to destination address.
  \param[in]   p_src   Pointer to source address.
  \return      none
*/
static void write_128bit(uint8_t *p_dst, const uint8_t *p_src)
{
    /* destination (MRAM address) must be always 16-byte aligned,
     * source may or may not be aligned.*/

    /* use temporary buffer for storing source data,
     * in case source data is not 16-bytes aligned.*/
    uint32_t temp_buf[4] = {0}; /* 128-bit.*/

    /* check source address is aligned to 16-bytes? */
    uint8_t *aligned_src = (uint8_t*)((uint32_t)p_src & FLASH_MRAM_ADDR_ALIGN_MASK);

    /* Disable IRQ Interrupts */
    __disable_irq();

    /* is source data unaligned? */
    if(p_src != aligned_src)
    {
        /* unaligned source data,
         *  - copy source data first in temporary buffer
         *  - then copy buffer to destination/MRAM.
         */
        memcpy(temp_buf, p_src, FLASH_MRAM_SECTOR_SIZE);

        ((volatile uint64_t *)p_dst)[0] = ((volatile uint64_t *)temp_buf)[0];
        ((volatile uint64_t *)p_dst)[1] = ((volatile uint64_t *)temp_buf)[1];
    }
    else
    {
        ((volatile uint64_t *)p_dst)[0] = ((volatile uint64_t *)p_src)[0];
        ((volatile uint64_t *)p_dst)[1] = ((volatile uint64_t *)p_src)[1];
    }

#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    /* clean/flush Dcache (internally it also calls __DSB.) */
    SCB_CleanDCache_by_Addr((uint32_t *)p_dst, FLASH_MRAM_SECTOR_SIZE);
#else
    /* Data Synchronization Barrier. */
    __DSB();
#endif

    /* Enable IRQ Interrupts */
    __enable_irq();
}


/**
  \fn          int32_t MRAM_ReadData(uint32_t addr, void *data, uint32_t cnt,
                                     FLASH_MRAM_DEV *flash_mram)
  \brief       Read data from MRAM Flash.
  \param[in]   addr   MRAM Flash address-offset.
  \param[out]  data   Pointer to a buffer storing the data read from MRAM Flash.
  \param[in]   cnt    Number of data items to read.
  \param[in]   flash_mram  Pointer to MRAM flash resources structure
  \return      number of data items read or \ref execution_status
*/
static int32_t MRAM_ReadData(uint32_t addr, void *data, uint32_t cnt,
                             FLASH_MRAM_DEV *flash_mram)
{
    /* addr is MRAM address-offset, so add MRAM Base-address to it. */
    addr += FLASH_MRAM_BASE;

    flash_mram->status.error = 1;

    if (!(flash_mram->flags & FLASH_MRAM_FLAG_POWER))
        return ARM_DRIVER_ERROR;

    /* validate MRAM flash address and count. */
    if( (addr < FLASH_MRAM_BASE ) || (addr > ( FLASH_MRAM_BASE + FLASH_MRAM_SIZE)) )
        return ARM_DRIVER_ERROR_PARAMETER;

    if( (addr + cnt) > (FLASH_MRAM_BASE + FLASH_MRAM_SIZE) )
        return ARM_DRIVER_ERROR_PARAMETER;


    /* directly copy data from MRAM, no need for 16-byte memory alignment. */
    memcpy(data, (void*)addr, cnt);

    flash_mram->status.error = 0;
    return cnt;
}

/**
  \fn          int32_t MRAM_ProgramData(uint32_t addr, const void *data, uint32_t cnt,
                                        FLASH_MRAM_DEV *flash_mram)
  \brief       Program data to MRAM Flash.
  \param[in]   addr   MRAM Flash address-offset.
  \param[in]   data   Pointer to a buffer containing the data to be programmed to MRAM Flash.
  \param[in]   cnt    Number of data items to program.
  \param[in]   flash_mram  Pointer to MRAM flash resources structure
  \return      number of data items programmed or \ref execution_status
*/
static int32_t MRAM_ProgramData(uint32_t addr, const void *data, uint32_t cnt,
                                FLASH_MRAM_DEV *flash_mram)
{
    /* addr is MRAM address-offset, so add MRAM Base-address to it. */
    addr += FLASH_MRAM_BASE;

    /* check address with aligned to 16-Bytes.*/
    uint32_t aligned_addr   = addr & FLASH_MRAM_ADDR_ALIGN_MASK;
    uint8_t *p_aligned_addr = (uint8_t *) aligned_addr;
    uint8_t *p_data         = (uint8_t *) data;
    uint32_t count          = cnt;

    /* use temporary buffer to store data in case of unaligned memory.*/
    uint8_t temp_buff[FLASH_MRAM_SECTOR_SIZE] = {0}; /* 128-Bit */

    flash_mram->status.error = 1;

    if (!(flash_mram->flags & FLASH_MRAM_FLAG_POWER))
        return ARM_DRIVER_ERROR;

    /* validate MRAM flash address and count. */
    if( (addr < FLASH_MRAM_BASE ) || (addr > ( FLASH_MRAM_BASE + FLASH_MRAM_SIZE)) )
        return ARM_DRIVER_ERROR_PARAMETER;

    if( (addr + cnt) > (FLASH_MRAM_BASE + FLASH_MRAM_SIZE) )
        return ARM_DRIVER_ERROR_PARAMETER;


    /* is MRAM flash address aligned to 16-Byte? */
    if(addr != aligned_addr)
    {
        /* unaligned MRAM flash address:
         *  - make it to nearest aligned 16-Byte address,
         *     by writing only unaligned bytes.
         */

        uint8_t offset           = addr & (~FLASH_MRAM_ADDR_ALIGN_MASK);
        uint8_t unaligned_bytes  = FLASH_MRAM_SECTOR_SIZE - offset;

        /* is unaligned bytes more than remaining count? */
        if(unaligned_bytes > count)
        {
            /* then take only remaining count. */
            unaligned_bytes = count;
        }

        /* copy original one sector data from MRAM flash address to buffer. */
        memcpy(temp_buff, p_aligned_addr, FLASH_MRAM_SECTOR_SIZE);

        /* overwrite buffer with new data as per offset and unaligned bytes. */
        memcpy(temp_buff + offset, p_data, unaligned_bytes);

        /* now, copy 128bit from buffer to MRAM flash . */
        write_128bit(p_aligned_addr, temp_buff);

        p_aligned_addr += FLASH_MRAM_SECTOR_SIZE;
        p_data         += unaligned_bytes;
        count          -= unaligned_bytes;
    }

    uint32_t sector_cnt    = count / FLASH_MRAM_SECTOR_SIZE;
    uint8_t unaligned_cnt  = count % FLASH_MRAM_SECTOR_SIZE;

    /* program 128-bit to absolute sector. */
    while(sector_cnt--)
    {
        /* as MRAM flash address is 16-byte aligned,
         * directly copy 128bit from source-data to MRAM flash. */
        write_128bit(p_aligned_addr, p_data);

        p_aligned_addr += FLASH_MRAM_SECTOR_SIZE;
        p_data         += FLASH_MRAM_SECTOR_SIZE;
    }

    /* program remaining unaligned data. */
    if(unaligned_cnt)
    {
        /* copy original one sector data from MRAM flash address to buffer.*/
        memcpy(temp_buff, p_aligned_addr, FLASH_MRAM_SECTOR_SIZE);

        /* overwrite buffer with new data as per remaining unaligned count.*/
        memcpy(temp_buff, p_data, unaligned_cnt);

        /* now, copy 128bit from buffer to MRAM flash . */
        write_128bit(p_aligned_addr, temp_buff);
    }

    flash_mram->status.error = 0;
    return cnt;
}

/**
  \fn          int32_t MRAM_EraseSector(uint32_t addr, FLASH_MRAM_DEV *flash_mram)
  \brief       Erase MRAM Flash Sector.
  \param[in]   addr   MRAM Sector address-offset
  \param[in]   flash_mram  Pointer to MRAM flash resources structure
  \return      \ref execution_status
*/
static int32_t MRAM_EraseSector(uint32_t addr, FLASH_MRAM_DEV *flash_mram)
{
    /* addr is MRAM address-offset, so add MRAM Base-address to it. */
    addr += FLASH_MRAM_BASE;

    uint32_t aligned_addr = addr & FLASH_MRAM_ADDR_ALIGN_MASK;

    uint8_t temp_buff[FLASH_MRAM_SECTOR_SIZE] = {0}; /* 128-Bit */

    flash_mram->status.error = 1;

    if(!(flash_mram->flags & FLASH_MRAM_FLAG_POWER))
        return ARM_DRIVER_ERROR;

    /* validate MRAM flash address.*/
    if( (addr < FLASH_MRAM_BASE ) || (addr > ( FLASH_MRAM_BASE + FLASH_MRAM_SIZE)) )
        return ARM_DRIVER_ERROR_PARAMETER;

    /* is MRAM flash address unaligned? */
    if( addr != aligned_addr )
        /* unaligned MRAM flash address is not supported. */
        return ARM_DRIVER_ERROR_PARAMETER;


    /* program 'FLASH_MRAM_ERASED_VALUE' to selected MRAM flash sector. */
    memset(temp_buff, FLASH_MRAM_ERASED_VALUE, FLASH_MRAM_SECTOR_SIZE);

    /* write erase value to selected MRAM sector of 16-byte. */
    write_128bit((uint8_t *)addr, temp_buff);

    flash_mram->status.error = 0;
    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t MRAM_EraseChip(FLASH_MRAM_DEV *flash_mram)
  \brief       Erase complete MRAM Flash.
               Optional function for faster full chip erase.
  \param[in]   flash_mram  Pointer to MRAM flash resources structure
  \return      \ref execution_status
*/
static int32_t MRAM_EraseChip(FLASH_MRAM_DEV *flash_mram)
{
    flash_mram->status.error = 1;

    /* MRAM flash erase chip is not supported. */
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
  \fn          ARM_FLASH_STATUS MRAM_GetStatus(FLASH_MRAM_DEV *flash_mram)
  \brief       Get MRAM Flash status.
  \param[in]   flash_mram  Pointer to MRAM flash resources structure
  \return      Flash status \ref ARM_FLASH_STATUS
*/
static ARM_FLASH_STATUS MRAM_GetStatus(FLASH_MRAM_DEV *flash_mram)
{
    /* MRAM Flash status busy flag will be always 0,
     * as it uses Blocking(Polling) method.
     */
    return (flash_mram->status);
}

/**
  \fn          ARM_FLASH_INFO * MRAM_GetInfo(FLASH_MRAM_DEV *flash_mram)
  \brief       Get MRAM Flash information.
  \param[in]   flash_mram  Pointer to MRAM flash resources structure
  \return      Pointer to Flash information \ref ARM_FLASH_INFO
*/
static ARM_FLASH_INFO * MRAM_GetInfo(FLASH_MRAM_DEV *flash_mram)
{
    return (flash_mram->info);
}

// End Flash Interface


/* Flash Driver Instance */
#if (RTE_FLASH_MRAM)

/* wrapper functions for Flash MRAM */
static int32_t Flash_MRAM_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    return (MRAM_Initialize(cb_event, &flash_mram));
}

static int32_t Flash_MRAM_Uninitialize(void)
{
    return (MRAM_Uninitialize(&flash_mram));
}

static int32_t Flash_MRAM_PowerControl(ARM_POWER_STATE state)
{
    return (MRAM_PowerControl(state, &flash_mram));
}

static int32_t Flash_MRAM_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    return (MRAM_ReadData(addr, data, cnt, &flash_mram));
}

static int32_t Flash_MRAM_ProgramData(uint32_t addr, const void *data, uint32_t cnt)
{
    return (MRAM_ProgramData(addr, data, cnt, &flash_mram));
}

static int32_t Flash_MRAM_EraseSector(uint32_t addr)
{
    return (MRAM_EraseSector(addr, &flash_mram));
}

static int32_t Flash_MRAM_EraseChip(void)
{
    return (MRAM_EraseChip(&flash_mram));
}

static ARM_FLASH_STATUS Flash_MRAM_GetStatus(void)
{
    return (MRAM_GetStatus(&flash_mram));
}

static ARM_FLASH_INFO * Flash_MRAM_GetInfo(void)
{
    return (MRAM_GetInfo(&flash_mram));
}


/* Flash Driver Control Block */
extern ARM_DRIVER_FLASH ARM_Driver_Flash_(FLASH_MRAM_INSTANCE);
ARM_DRIVER_FLASH ARM_Driver_Flash_(FLASH_MRAM_INSTANCE) =
{
    Flash_MRAM_GetVersion,
    Flash_MRAM_GetCapabilities,
    Flash_MRAM_Initialize,
    Flash_MRAM_Uninitialize,
    Flash_MRAM_PowerControl,
    Flash_MRAM_ReadData,
    Flash_MRAM_ProgramData,
    Flash_MRAM_EraseSector,
    Flash_MRAM_EraseChip,
    Flash_MRAM_GetStatus,
    Flash_MRAM_GetInfo
};
#endif /* RTE_FLASH_MRAM */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
