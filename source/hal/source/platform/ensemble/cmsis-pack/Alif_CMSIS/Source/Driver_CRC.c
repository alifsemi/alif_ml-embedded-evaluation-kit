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
 * @file     Driver_CRC.c
 * @author   Nisarga A M
 * @email    nisarga.am@alifsemi.com
 * @version  V1.0.0
 * @date     18-april-2022
 * @brief    CMSIS driver for CRC (Cyclic Redundancy Check ).
 ******************************************************************************/

/* Project Includes */
#include "Driver_CRC.h"
#include "CRC_dev.h"

#if !(RTE_CRC0 || RTE_CRC1)
#error "CRC is not enabled in the RTE_device.h"
#endif

#if (defined(RTE_Drivers_CRC0) && !RTE_CRC0)
#error "CRC0 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_CRC1) && !RTE_CRC1)
#error "CRC1 not configured in RTE_Device.h!"
#endif

#define ARM_CRC_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)  /*  Driver version */

/*Driver version*/
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_CRC_API_VERSION,
    ARM_CRC_DRV_VERSION
};

/*Driver Capabilities   */
static const ARM_CRC_CAPABILITIES DriverCapabilities = {
    1,  /* Supports CRC_8_CCITT */
    1,  /* Supports CRC_16 */
    1,  /* Supports CRC_16_CCITT */
    1,  /* Supports CRC_32 */
    1,  /* Supports CRC_32C */
    0   /* Reserved ( must be ZERO) */
};

/**
 @fn           ARM_DRIVER_VERSION CRC_GetVersion(void)
 @brief        get CRC version
 @param        none
 @return       driver version
 */
static ARM_DRIVER_VERSION CRC_GetVersion(void)
{
    return DriverVersion;
}

/**
 @fn           ARM_CRC_CAPABILITIES CRC_GetCapabilities(void)
 @brief        get CRC Capabilites
 @param        none
 @return       driver Capabilites
 */
static ARM_CRC_CAPABILITIES CRC_GetCapabilities(void)
{
    return DriverCapabilities;
}

/**
 @fn           CRC_Byte_Swap(int arg,CRC_resources_t *crc )
 @brief        To select the CRC byte swap
 @param[in]    ctrl : to enable or disable the byte swap of CRC
 @param[in]    CRC  : Pointer to CRC device resources
 @return       none
 */
__STATIC_INLINE void CRC_Byte_Swap (int arg, CRC_resources_t *crc)
{
    if(arg)  /* To enable the byte swap CRC Bit */
        crc->reg_base->CONTROL |= BYTE_SWAP;

    else  /* To disable the byte swap CRC Bit */
        crc->reg_base->CONTROL &= ~(BYTE_SWAP);
}

/**
 @fn           CRC_Bit_swap(int arg,CRC_resources_t *crc )
 @brief        To select the CRC bit swap
 @param[in]    ctrl : to enable or disable the bit swap of CRC
 @param[in]    CRC  : Pointer to CRC device resources
 @return       none
 */
__STATIC_INLINE void CRC_Bit_swap (int arg, CRC_resources_t *crc)
{
    if(arg)  /* To enable the bit swap CRC Bit */
        crc->reg_base->CONTROL |= BIT_SWAP;

    else  /* To disable the bit swap CRC Bit */
        crc->reg_base->CONTROL &= ~(BIT_SWAP);
}

/**
 @fn           CRC_Custom_Poly(int arg,CRC_resources_t *crc )
 @brief        To give the custom polynomial
 @param[in]    ctrl : to enable or disable the Custom polynomial bit of CRC
 @param[in]    CRC  : Pointer to CRC device resources
 @return       none
 */
__STATIC_INLINE void CRC_Custom_Poly (int arg, CRC_resources_t *crc)
{

    if(arg) /* To enable the Custom polynomial CRC Bit */
        crc->reg_base->CONTROL |= CUSTOM_POLY;

    else /* To disable the Custom polynomial CRC Bit */
        crc->reg_base->CONTROL &= ~(CUSTOM_POLY);
}

/**
 @fn           CRC_Invert(int arg,CRC_resources_t *crc )
 @brief        To Invert CRC bit
 @param[in]    ctrl: to enable or disable the Invert bit of CRC
 @param[in]    CRC : Pointer to CRC device resources
 @return       none
 */
__STATIC_INLINE void CRC_Invert (int arg, CRC_resources_t *crc)
{
    if(arg)  /* To enable the Invert CRC Bit */
        crc->reg_base->CONTROL |= INVERT_CRC;

    else     /* To disable the Invert CRC Bit */
        crc->reg_base->CONTROL &= ~(INVERT_CRC);
}

/**
 @fn           CRC_Reflect(int arg,CRC_resources_t *crc )
 @brief        To reflect CRC bit
 @param[in]    ctrl: to enable or disable the reflect bit of CRC
 @param[in]    CRC      : Pointer to CRC device resources
 @return       none
 */
__STATIC_INLINE void CRC_Reflect (int arg, CRC_resources_t *crc)
{
    if(arg)  /* To enable the reflect CRC Bit */
        crc->reg_base->CONTROL |= REFLECT_CRC;

    else  /* To disable the reflect CRC Bit */
        crc->reg_base->CONTROL &= ~(REFLECT_CRC);
}

/**
 @fn           CRC_Compute_Init(CRC_resources_t *crc )
 @brief        To load CRC Init value in CRC Control register
 @param[in]    CRC      : Pointer to CRC device resources
 @return       none
 */
__STATIC_INLINE void CRC_Compute_Init (CRC_resources_t *crc)
{
    /* To enable the INIT bit of CRC in control register */
    crc->reg_base->CONTROL |= (CRC_INIT_BIT);
}

/**
 @fn           control_bit(uint32_t control,uint32_t arg,CRC_resources_t *crc)
 @brief        To enable or disable the Reflect, Invert, Bit, Byte, Custom polynomial bit of CRC
 @param[in]    control : to get the value from the control
 @param[in]    arg  : to give enable or disable the control
 @param[in]    CRC  : Pointer to CRC device resources
 @return       none
 */
__STATIC_INLINE void control_bit (uint32_t control,uint32_t arg,CRC_resources_t *crc)
{

    /* To select the CRC byte swap */
    if (control & ARM_CRC_ENABLE_BYTE_SWAP )
    {
        CRC_Byte_Swap(arg,crc);
    }

    /*To select the CRC bit swap */
    if (control & ARM_CRC_ENABLE_BIT_SWAP)
    {
        CRC_Bit_swap(arg,crc);
    }

    /*To select the CRC custom polynomial */
    if(control & ARM_CRC_ENABLE_CUSTOM_POLY)
    {
        CRC_Custom_Poly(arg,crc);
    }

    /*To select the CRC Invert */
    if(control & ARM_CRC_ENABLE_INVERT)
    {
        CRC_Invert(arg,crc);
    }

    /*To select the CRC reflect */
    if(control & ARM_CRC_ENABLE_REFLECT)
    {
        CRC_Reflect(arg,crc);
    }
}

/**
@fn          int32_tCRC_Initialize (CRC_resources_t *crc)
@brief       Initialize the CRC interface
@param[in]   crc : Pointer to CRC resources
 @return     ARM_DRIVER_ERROR_PARAMETER : if CRC device is invalid
             ARM_DRIVER_OK              : if CRC successfully initialized or already initialized
 */
static int32_t CRC_Initialize(CRC_resources_t *crc)
{
    int ret = ARM_DRIVER_OK;

    /* Setting the flag */
    crc->flags |= CRC_FLAG_DRV_INIT_DONE;

    return ret;
}

/**
@fn          int32_t CRC_Uninitialize (CRC_resources_t *crc)
@brief       UnInitialize the CRC interface
@param[in]   crc : Pointer to CRC resources
 @return     ARM_DRIVER_ERROR_PARAMETER : if CRC device is invalid
             ARM_DRIVER_OK              : if CRC successfully initialized or already initialized
 */
static int32_t CRC_Uninitialize(CRC_resources_t *crc)
{
    int ret = ARM_DRIVER_OK;

    /* UnInitailzation for CRC control register to zero */
    crc->reg_base->CONTROL = 0U;

    /* Reset the flag */
	crc->flags = 0U;

    return ret;
}

/**
 @fn           int32_t CRC_PowerControl (ARM_POWER_STATE state,
                                          CRC_resources_t *crc)
 @brief        CMSIS-DRIVER CRC power control
 @param[in]    state : Power state
 @param[in]    crc   : Pointer to CRC resources
 @return       ARM_DRIVER_ERROR_PARAMETER  : if crc device is invalid
               ARM_DRIVER_OK               : if crc successfully uninitialized or already not initialized
 */
static int32_t CRC_PowerControl(ARM_POWER_STATE status,
                                CRC_resources_t *crc)
{
    switch(status)
    {
    case ARM_POWER_OFF:

         /* Reset the power flag */
         crc->flags &= ~(CRC_FLAG_DRV_POWER_DONE);

    break;

    case ARM_POWER_FULL:

    if(!(crc->flags & CRC_FLAG_DRV_INIT_DONE))
    {
        /* error:Driver is not initialized */
        return ARM_DRIVER_ERROR;
    }

    if((crc->flags & CRC_FLAG_DRV_POWER_DONE))
    {
        return ARM_DRIVER_OK;
    }
    /* Set the power flag enabled */
    crc->flags |= CRC_FLAG_DRV_POWER_DONE;
    break;

    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

/**
 @fn           int32_t CRC_Control (uint32_t control,
                                    uint32_t arg,
                                    CRC_resources_t *crc)
 @brief        CMSIS-Driver crc control.
               Control CRC Interface.
 @param[in]    control : Operation \ref Driver_CRC.h : CRC control codes
 @param[in]    arg     : Argument of operation (optional)
 @param[in]    crc     : Pointer to crc resources
 @return       ARM_DRIVER_ERROR_PARAMETER  : if crc device is invalid
               ARM_DRIVER_OK               : if crc successfully uninitialized or already not initialized
 */
static int32_t CRC_Control (uint32_t control,
                            uint32_t arg,
                            CRC_resources_t *crc)
{
    int32_t ret = ARM_DRIVER_OK;

    if(!(crc->flags & CRC_FLAG_DRV_INIT_DONE))
        return ARM_DRIVER_ERROR;

    if(!(crc->flags & CRC_FLAG_DRV_POWER_DONE))
        return ARM_DRIVER_ERROR;

    if(control & ARM_CRC_CONTROL_MASK)
    {
        /* To enable or disable the Reflect, Invert, Bit, Byte, Custom polynomial bits of CRC*/
        control_bit(control,arg,crc);
    }

    else
    {
        switch (control)
        {
        case ARM_CRC_ALGORITHM_SEL:

            /* clear 8,16 and 32 bit algorithm */
            crc->reg_base->CONTROL &= ~(ALGO_SEL);

            /* clear the 8, 16, 32 bit algorithm size */
            crc->reg_base->CONTROL &= ~(ALGO_SIZE);

            switch(arg)
            {
            case ARM_CRC_ALGORITHM_SEL_8_BIT_CCITT:

            /* To enable 8 bit CRC */
            crc->reg_base->CONTROL |= CRC_8_CCITT;

            /* To enable the 8 bit algorithm size */
            crc->reg_base->CONTROL |= ALGO_8_BIT_SIZE;
            break;

            case ARM_CRC_ALGORITHM_SEL_16_BIT:

            /* To enable 16 bit CRC */
            crc->reg_base->CONTROL |= CRC_16;

            /* To enable 16 bit size CRC */
            crc->reg_base->CONTROL |= ALGO_16_BIT_SIZE;
            break;

            case ARM_CRC_ALGORITHM_SEL_16_BIT_CCITT:

            /* To enable 16 bit CCITT CRC */
            crc->reg_base->CONTROL |= CRC_16_CCITT;

            /* To enable 16 bit size CRC */
            crc->reg_base->CONTROL |= ALGO_16_BIT_SIZE;
            break;

            case ARM_CRC_ALGORITHM_SEL_32_BIT:

            /* To enable 32 bit CRC */
            crc->reg_base->CONTROL |= CRC_32;

            /* To enable 32 bit CRC */
            crc->reg_base->CONTROL |= ALGO_32_BIT_SIZE;
            break;

            case ARM_CRC_ALGORITHM_SEL_32_BIT_CUSTOM_POLY:

            /* To enable 32 bit poly custom CRC */
            crc->reg_base->CONTROL |= CRC_32C;

            /* To enable 32 bit CRC */
            crc->reg_base->CONTROL |= ALGO_32_BIT_SIZE;
            break;
            default:
            ret = ARM_DRIVER_ERROR_UNSUPPORTED;
            }
       break;
       default:
       ret = ARM_DRIVER_ERROR_UNSUPPORTED;
       }
    }
    return ret;
}

/**
@fn         int32_t CRC_Seed (uint32_t value ,CRC_resources_t *crc)
@brief      CMSIS-DRIVER CRC Seed value
            Enable the Init bit [0th bit] of the control register to load the seed value
@param[in]  seed_value : Seed value depending on whether the data is 8 bit or 16 or 32 bit
@param[in]  crc        : pointer to CRC resources
@return     \ref execution_status
*/
static int32_t CRC_Seed (uint32_t seed_value,
                         CRC_resources_t *crc)
{
    int32_t ret = ARM_DRIVER_OK;

    if(!(crc->flags & CRC_FLAG_DRV_POWER_DONE))
    {
        /* error:Driver is not initialized */
        return ARM_DRIVER_ERROR;
    }

    /* Adding 8 bit or 16 bit or 32 bit seed value to the Seed register of CRC */
    crc->reg_base->SEED = seed_value;

    /* Write the Init value in control register to load the Seed value in Seed register */
    CRC_Compute_Init(crc);

    return ret;
}

/**
@fn         int32_t CRC_PolyCustom (uint32_t value,CRC_resources_t *crc)
@brief      To add the polynomial value to polycustom register
            Enable the Init bit [0th bit] of the conrol register to load the
            polynomial value
@param[in]  polynomial : Polynomial data for 8 bit or 16 or 32 bit
@param[in]  crc        : pointer to CRC resources
@return     \ref execution_status
*/
static int32_t CRC_PolyCustom (uint32_t value,
                              CRC_resources_t *crc)
{
    int32_t ret = ARM_DRIVER_OK;

    if(!(crc->flags & CRC_FLAG_DRV_POWER_DONE))
    {
        /* error:Driver is not initialized */
        return ARM_DRIVER_ERROR;
    }
    /* Adding Polynomial value to the poly_custom register of CRC */
    crc->reg_base->POLY_CUSTOM = value;

    /* Write the Init value in control register to load the polynomial value in Poly_custom register */
    CRC_Compute_Init(crc);

    return ret;
}

/**
@fn         uint32_t bit_reflect (uint32_t input)
@brief      To reflect the 32 bit polynomial
@param[in]  input : 32 bit input Polynomial
@return     Reflected polynomial Input
*/
static uint32_t bit_reflect(uint32_t input)
{
    uint32_t refin = 0;
    uint32_t bit,i;

    for(i=0; i<32; i++)
    {
        bit = (input >> i) & 1;
        bit = bit << (32 - (i+1));
        refin |= bit;
    }

    return refin;
}

/**
@fn         uint32_t swap (uint32_t input)
@brief      To swap the 32 bit input value
@param[in]  input : 32 bit input value
@return     swaped 32 bit input value
*/
static uint32_t swap(uint32_t input_value)
{
    uint32_t res;

    res = (((input_value & 0xFF)<< 24) | ((input_value & 0xFF00)<< 8) |((input_value & 0xFF0000)>> 8) |((input_value & 0xFF000000)>>24));

    return res;

}
/**
@fn         uint32_t CRC_calculate_Unaligned(uint32_t key, uint8_t *input, uint8_t length, uint32_t poly)
@brief      To calculate the CRC result for unaligned data
             1. It will take the aligned data for CRC result from the hardware.
             2. It will the Unaligned input data and its length.
             3. If the algorithm is 32 bit CRC then it will take the standard 32 bit CRC Polynomial
             4. If the algorithm is 32 bit Custom CRC polynomial , it will take the polynomial entered
                from the user.
@param[in]  key   : Output of aligned data for CRC from the hardware
@param[in]  input : unaligned input data
@param[in]  length: length of unaligned data
@param[in]  poly  : Standard polynimial or the user entered polynomial depending upon the CRC algorithm
@return     Calculated CRC output for unaligned data
*/
static uint32_t CRC_calculate_Unaligned(uint32_t key, uint8_t *input, uint8_t length, uint32_t poly)
{
    uint32_t crc,b,polynomial;
    uint8_t c;
    int32_t i,j;

    crc = key;

    /* Store the reflected polynomial */
    polynomial = bit_reflect(poly);

    for(i=0; i<length; i++)
    {
        c = input[i];
        for(j=0; j<8; j++)
        {
            b = (crc ^ c) & 1;
            crc >>= 1;

            if( b )
            {
                crc = crc ^ polynomial;
            }
            c >>= 1;
        }
    }

    return ~crc;

}

/**
@fn         int32_t CRC_Compute (void *data_in,uint32_t len,uint32_t *data_out,CRC_resources_t *crc)
@brief      1.To calculate the CRC result for 8 bit 16 bit and 32 bit CRC algorithm.
            2.For 8 bit and 16 bit CRC algorithm our hardware can able to calculate the CRC
              result for both aligned and unaligned CRC input data by loading the CRC inputs
              in DATA_IN_8 bit register.
            3. For 32 bit CRC our hardware will support for aligned data to calculate the CRC Result.
            4. For unaligned data CRC_calculate_Unaligned function will calculate the CRC result for
               unaligned CRC input
            5. In CRC_calculate_Unaligned function load the aligned CRC result from the hardware ,
               unaligned crc input,length of unaligned input data and the polynomial for the 32 bit CRC
@param[in]  data_in : it is a pointer which holds the address of user input
            len     : Length of the input data
            data_out : To get the CRC output
@param[in]  crc  : pointer to CRC resources
@return     \ref execution_status
*/
static int32_t CRC_Compute (void *data_in,uint32_t len,uint32_t *data_out,CRC_resources_t *crc)
{
    int32_t ret = ARM_DRIVER_OK;
    uint32_t custom;
    uint32_t *data32;
    uint32_t value,crc_result,reverse, reflect;
    int8_t algo_size,count = 0;
    int8_t aligned_length,unaligned_length;

    if(!(crc->flags & CRC_FLAG_DRV_POWER_DONE))
    {
         /* error:Driver is not initialized */
         return ARM_DRIVER_ERROR;
    }

    if(data_in == NULL || data_out == NULL || len == 0)
    {
        /* error: pointer is not valid */
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* To check whether the algorithm size is 8 bit or 16 or 32 bit */
    algo_size = ((crc->reg_base->CONTROL & (ALGORITHM_CHECK)) >> 1);

    switch(algo_size)
    {
    /* For 8 bit */
    case CRC_8_BIT_SIZE:

        for (count = 0; count < len; count ++)
        {
            /* User input 8 bit data is storing into the DATA_IN_8 bit register */
            crc->reg_base->DATA_IN_8[0] = ((uint8_t *)data_in)[count];
        }

        /* data_out pointer to store the CRC output */
        *data_out = (crc->reg_base->CRC_OUT);

        break;

    /* For 16 bit */
    case CRC_16_BIT_SIZE:

        for (count = 0; count < len; count ++)
        {
            /* User input 8 bit data is storing into the DATA_IN_8 bit register */
            crc->reg_base->DATA_IN_8[0] = ((uint8_t *)data_in)[count];
        }

        /* data_out pointer to store the CRC output */
        *data_out = (crc->reg_base->CRC_OUT);

        break;

    /* For 32 bit */
    case CRC_32_BIT_SIZE:

        aligned_length   = len-(len % 4);
        unaligned_length = (len % 4);
        data32           = (uint32_t *)data_in;

        for (count = 0;count<aligned_length/4;count++)
        {
            value = *(data32++);
            /* Swap the input data */
            value = swap(value);

            /* User input 32 bit data is storing into the DATA_IN_32 bit register */
            crc->reg_base->DATA_IN_32[0] =  value;
        }

        /* Store the CRC aligned output */
        reverse = (crc->reg_base->CRC_OUT);

        *data_out = ~reverse;

        /* Check for the custom polynomial bit  */
        if(crc->reg_base->CONTROL & CUSTOM_POLY )
        {
            /* assign the user polynomial */
            custom = crc->reg_base->POLY_CUSTOM;
            *data_out = ~(*data_out);

            /* reflect the output */
            *data_out = bit_reflect(*data_out);

            /* Complement the output */
            *data_out = ~(*data_out);

        }

        else
        {
            /* Assign the 32 bit CRC standard polynomial */
            custom = STANDARD_POLY;
        }

        /* Calculate the CRC for unaligned data */
        crc_result = CRC_calculate_Unaligned(*data_out, data_in + aligned_length, unaligned_length, custom);

        *data_out = crc_result;  /* Store the CRC result */

        break;
    }

    return ret;
}

/* CRC0 Driver instance */
#if (RTE_CRC0)

static CRC_resources_t CRC0 = {
    .reg_base      = (CRC_typedef *)CRC0_BASE,
    .flags         = 0
};

/* Function Name: CRC0_Initialize */
static int32_t CRC0_Initialize(void)
{
    return (CRC_Initialize(&CRC0));
}

/* Function Name: CRC0_Uninitialize */
static int32_t CRC0_Uninitialize(void)
{
    return (CRC_Uninitialize(&CRC0));
}

/* Function Name: CRC0_PowerControl */
static int32_t CRC0_PowerControl(ARM_POWER_STATE status)
{
    return (CRC_PowerControl(status,&CRC0));
}

/* Function Name: CRC0_Control */
static int32_t CRC0_Control(uint32_t control, uint32_t arg)
{
    return (CRC_Control(control,arg,&CRC0));
}

/* Function Name: CRC0_Seed */
static int32_t CRC0_Seed(uint32_t value)
{
    return (CRC_Seed(value,&CRC0));
}

/* Function Name: CRC0_PolyCustom */
static int32_t CRC0_PolyCustom(uint32_t value)
{
    return (CRC_PolyCustom(value,&CRC0));
}

/* Function Name: CRC0_Compute */
static int32_t CRC0_Compute(void *data_in,uint32_t len,uint32_t *data_out)
{
    return (CRC_Compute(data_in,len,data_out,&CRC0));
}

extern ARM_DRIVER_CRC Driver_CRC0;
ARM_DRIVER_CRC Driver_CRC0 = {
    CRC_GetVersion,
    CRC_GetCapabilities,
    CRC0_Initialize,
    CRC0_Uninitialize,
    CRC0_PowerControl,
    CRC0_Control,
    CRC0_Seed,
    CRC0_PolyCustom,
    CRC0_Compute,
};

#endif /* RTE_CRC0 */

/* CRC1 driver instance */
#if (RTE_CRC1)

static CRC_resources_t CRC1 = {
    .reg_base      = (CRC_typedef *)CRC1_BASE,
    .flags         = 0
};

/* Function Name: CRC1_Initialize */
static int32_t CRC1_Initialize(void)
{
    return (CRC_Initialize(&CRC1));
}

/* Function Name: CRC1_Uninitialize */
static int32_t CRC1_Uninitialize(void)
{
    return (CRC_Uninitialize(&CRC1));
}

/* Function Name: CRC1_PowerControl */
static int32_t CRC1_PowerControl(ARM_POWER_STATE status)
{
    return (CRC_PowerControl(status,&CRC1));
}

/* Function Name: CRC1_Control */
static int32_t CRC1_Control(uint32_t control, uint32_t arg)
{
    return (CRC_Control(control,arg,&CRC1));
}

/* Function Name: CRC1_Seed */
static int32_t CRC1_Seed(uint32_t value)
{
    return (CRC_Seed(value,&CRC1));
}

/* Function Name: CRC1_PolyCustom */
static int32_t CRC1_PolyCustom(uint32_t value)
{
    return (CRC_PolyCustom(value,&CRC1));
}

/* Function Name: CRC1_Compute */
static int32_t CRC1_Compute(void *data_in,uint32_t len,uint32_t *data_out)
{
    return (CRC_Compute(data_in,len,data_out,&CRC1));
}

extern ARM_DRIVER_CRC Driver_CRC1;
ARM_DRIVER_CRC Driver_CRC1 = {
    CRC_GetVersion,
    CRC_GetCapabilities,
    CRC1_Initialize,
    CRC1_Uninitialize,
    CRC1_PowerControl,
    CRC1_Control,
    CRC1_Seed,
    CRC1_PolyCustom,
    CRC1_Compute
};

#endif /* RTE_CRC1 */
