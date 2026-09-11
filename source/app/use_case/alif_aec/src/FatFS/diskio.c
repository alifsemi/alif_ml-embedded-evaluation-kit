/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "string.h"
#include "stdio.h"
#include "board_config.h"

#if defined(BOARD_SD_RESET_GPIO_PORT) || \
    defined(BOARD_SD_CARD_DETECT_GPIO_PORT) || \
    defined(BOARD_SD_VSEL_GPIO_PORT)
#include "Driver_IO.h"
#endif

/* Definitions of physical drive number for each drive */
#define DEV_MMC 0 /* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB 1 /* Example: Map USB MSD to physical drive 2 */

/* SD Card Instance */
extern sd_handle_t Hsd;
const diskio_t    *p_SD_Driver = &SD_Driver;

/* Interrupt Handler callback */
volatile uint32_t dma_done_irq;
void              sd_cb(uint16_t cmd_status, uint16_t xfer_status)
{
    ARG_UNUSED(cmd_status);

    if (xfer_status) {
        dma_done_irq = 1;
    }
}

/**
 * \fn           sd_pwr(uint8_t power_on)
 * \brief        Perform SD power sequence
 * \return       none
 */
#ifdef BOARD_SD_RESET_GPIO_PORT
extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(BOARD_SD_RESET_GPIO_PORT);
void sd_pwr_cb(uint8_t power_on)
{
    int status;

    ARM_DRIVER_GPIO *sd_pwr_gpio = &ARM_Driver_GPIO_(BOARD_SD_RESET_GPIO_PORT);

    if (power_on) {
        status = sd_pwr_gpio->SetValue(BOARD_SD_RESET_GPIO_PIN, GPIO_PIN_OUTPUT_STATE_HIGH);
        if (status) {
            SD_LOG_ERR("Failed to turn on SD power pin");
        }
    } else {
        status = sd_pwr_gpio->SetValue(BOARD_SD_RESET_GPIO_PIN, GPIO_PIN_OUTPUT_STATE_LOW);
        if (status) {
            SD_LOG_ERR("Failed to turn off SD power pin");
        }
        sys_busy_loop_us(SDMMC_RESET_DELAY_US);
    }

    return;
}
#endif

/**
 * \fn           sd_card_det_cb(void)
 * \brief        Check SD card detect GPIO
 * \return       1 if card present, 0 if not
 */
#ifdef BOARD_SD_CARD_DETECT_GPIO_PORT
extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(BOARD_SD_CARD_DETECT_GPIO_PORT);
int sd_card_det_cb(void)
{
    uint32_t pin_state;
    ARM_DRIVER_GPIO *cd_gpio = &ARM_Driver_GPIO_(BOARD_SD_CARD_DETECT_GPIO_PORT);

    cd_gpio->GetValue(BOARD_SD_CARD_DETECT_GPIO_PIN, &pin_state);

    /* Card detect: pin state 0 = card present (active low) */
    return (pin_state == 0) ? 1 : 0;
}
#endif

/**
 * \fn           sd_vsel_cb(uint8_t voltage)
 * \brief        Set SD VSEL GPIO for voltage selection
 * \param[in]    voltage: 0 = 3.3V, 1 = 1.8V
 * \return       none
 */
#ifdef BOARD_SD_VSEL_GPIO_PORT
extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(BOARD_SD_VSEL_GPIO_PORT);
void sd_vsel_cb(uint8_t voltage)
{
    int status;
    ARM_DRIVER_GPIO *vsel_gpio = &ARM_Driver_GPIO_(BOARD_SD_VSEL_GPIO_PORT);

    /* VSEL: 0 = 3.3V, 1 = 1.8V */
    status = vsel_gpio->SetValue(BOARD_SD_VSEL_GPIO_PIN, voltage ? GPIO_PIN_OUTPUT_STATE_HIGH : GPIO_PIN_OUTPUT_STATE_LOW);
    if (status) {
        SD_LOG_ERR("Failed to set SD VSEL pin to %d", voltage);
    }

    SD_LOG_DBG("SD VSEL set to %s", voltage ? "1.8V" : "3.3V");
}
#endif

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv /* Physical drive number to identify the drive */
)
{
    ARG_UNUSED(pdrv);

#ifdef BOARD_SD_CARD_DETECT_GPIO_PORT
    if (!sdhc_card_present(&Hsd, sd_card_det_cb)) {
        return STA_NODISK;
    }
#endif

    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE drivenum)  // FATFS *p_sd_card, char *MEDIA_NAME, void * media_memory,
                                        // uint32_t media_size)
{
    int        status;
    sd_param_t sd_param;

    ARG_UNUSED(drivenum);

#ifdef BOARD_SD_RESET_GPIO_PORT
    ARM_DRIVER_GPIO *sd_pwr_gpio = &ARM_Driver_GPIO_(BOARD_SD_RESET_GPIO_PORT);

    status = sd_pwr_gpio->Initialize(BOARD_SD_RESET_GPIO_PIN, NULL);
    if (status) {
        SD_LOG_ERR("Failed to initialize SD PWR GPIO");
    }

    status = sd_pwr_gpio->PowerControl(BOARD_SD_RESET_GPIO_PIN, ARM_POWER_FULL);
    if (status) {
        SD_LOG_ERR("Failed to power SD PWR GPIO");
    }

    status = sd_pwr_gpio->SetDirection(BOARD_SD_RESET_GPIO_PIN, GPIO_PIN_DIRECTION_OUTPUT);
    if (status) {
        SD_LOG_ERR("Failed to configure SD PWR GPIO direction");
    }

    status = sd_pwr_gpio->SetValue(BOARD_SD_RESET_GPIO_PIN, GPIO_PIN_OUTPUT_STATE_HIGH);
    if (status) {
        SD_LOG_ERR("Failed to set SD power pin high");
    }

#endif

#ifdef BOARD_SD_CARD_DETECT_GPIO_PORT
    ARM_DRIVER_GPIO *cd_gpio = &ARM_Driver_GPIO_(BOARD_SD_CARD_DETECT_GPIO_PORT);

    status = cd_gpio->Initialize(BOARD_SD_CARD_DETECT_GPIO_PIN, NULL);
    if (status) {
        SD_LOG_ERR("Failed to initialize SD CD GPIO");
    }

    status = cd_gpio->PowerControl(BOARD_SD_CARD_DETECT_GPIO_PIN, ARM_POWER_FULL);
    if (status) {
        SD_LOG_ERR("Failed to power SD CD GPIO");
    }

    status = cd_gpio->SetDirection(BOARD_SD_CARD_DETECT_GPIO_PIN, GPIO_PIN_DIRECTION_INPUT);
    if (status) {
        SD_LOG_ERR("Failed to configure SD CD GPIO direction");
    }
#endif

#ifdef BOARD_SD_VSEL_GPIO_PORT
    ARM_DRIVER_GPIO *vsel_gpio = &ARM_Driver_GPIO_(BOARD_SD_VSEL_GPIO_PORT);

    status = vsel_gpio->Initialize(BOARD_SD_VSEL_GPIO_PIN, NULL);
    if (status) {
        SD_LOG_ERR("Failed to initialize SD VSEL GPIO");
    }

    status = vsel_gpio->PowerControl(BOARD_SD_VSEL_GPIO_PIN, ARM_POWER_FULL);
    if (status) {
        SD_LOG_ERR("Failed to power SD VSEL GPIO");
    }

    status = vsel_gpio->SetDirection(BOARD_SD_VSEL_GPIO_PIN, GPIO_PIN_DIRECTION_OUTPUT);
    if (status) {
        SD_LOG_ERR("Failed to configure SD VSEL GPIO direction");
    }

    /* Default to 3.3V */
    status = vsel_gpio->SetValue(BOARD_SD_VSEL_GPIO_PIN, GPIO_PIN_OUTPUT_STATE_LOW);
    if (status) {
        SD_LOG_ERR("Failed to set SD VSEL to 3.3V");
    }
#endif

    sd_param.dev_id       = SDMMC_DEV_ID;
    sd_param.clock_freq   = RTE_SDC_CLOCK_SELECT;
    sd_param.bus_width    = RTE_SDC_BUS_WIDTH;
    sd_param.dma_mode     = RTE_SDC_DMA_SELECT;
    sd_param.app_callback = sd_cb;

#ifdef BOARD_SD_RESET_GPIO_PORT
    sd_param.pwr_cb     = sd_pwr_cb;
#else
    sd_param.pwr_cb     = 0;
#endif
#ifdef BOARD_SD_CARD_DETECT_GPIO_PORT
    sd_param.card_det_cb = sd_card_det_cb;
#else
    sd_param.card_det_cb = 0;
#endif
#ifdef BOARD_SD_VSEL_GPIO_PORT
    sd_param.vsel_cb = sd_vsel_cb;
#else
    sd_param.vsel_cb = 0;
#endif

    status                = p_SD_Driver->disk_initialize(&sd_param);

    if (status) {
        return STA_NOINIT;
    }

    sys_busy_loop_us(2000);

    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE  pdrv,   /* Physical drive number to identify the drive */
                  BYTE *buff,   /* Data buffer to store read data */
                  LBA_t sector, /* Start sector in LBA */
                  UINT  count   /* Number of sectors to read */
)
{
	uint32_t timeout_cnt = 0xFFFF;
    ARG_UNUSED(pdrv);

    dma_done_irq = 0;

    if (p_SD_Driver->disk_read(sector, count, (volatile uint8_t *) buff)) {
        return RES_ERROR;
	}

    while (!dma_done_irq && timeout_cnt--) {
		sys_busy_loop_us(10);
    }

	if (!dma_done_irq) {
		return RES_ERROR;
	}

    RTSS_InvalidateDCache_by_Addr((volatile void *) buff, count * SDMMC_BLK_SIZE_512_Msk);
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE        pdrv,   /* Physical drive number to identify the drive */
                   const BYTE *buff,   /* Data to be written */
                   LBA_t       sector, /* Start sector in LBA */
                   UINT        count   /* Number of sectors to write */
)
{
    DRESULT res = RES_OK;

    ARG_UNUSED(pdrv);

    dma_done_irq = 0;
    RTSS_CleanDCache_by_Addr((volatile void *) buff, count * SDMMC_BLK_SIZE_512_Msk);

    if (p_SD_Driver->disk_write(sector, count, (volatile uint8_t *) buff) != SD_DRV_STATUS_OK) {
        res = RES_ERROR;
    }

    uint32_t timeout = 100000; // Max write Delay 1sec

    while (!dma_done_irq && timeout--) {
        sys_busy_loop_us(10);
    }

    if (!dma_done_irq) {
        res = RES_ERROR;
    }

    return res;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE  pdrv, /* Physical drive number (0..) */
                   BYTE  cmd,  /* Control code */
                   void *buff  /* Buffer to send/receive control data */
)
{
    DRESULT res = 0;

    ARG_UNUSED(cmd);
    ARG_UNUSED(buff);

    switch (pdrv) {
    case DEV_MMC:

        // Process of the command for the MMC/SD card

        return res;

    case DEV_USB:

        // Process of the command the USB drive

        return res;
    }

    return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Deinitialize a Drive                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_deinitialize(BYTE pdrv)  /* Physical drive number to identify the drive */
{
    ARG_UNUSED(pdrv);

    if (p_SD_Driver->disk_uninitialize(SDMMC_DEV_ID) != SD_DRV_STATUS_OK) {
        return RES_ERROR;
    }

    return RES_OK;
}
