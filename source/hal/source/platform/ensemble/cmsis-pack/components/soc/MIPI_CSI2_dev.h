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
 * @file     MIPI_CSI2_dev.h
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     24-March-2022
 * @brief    Device Specific Header file for MIPI CSI2 Driver.
 ******************************************************************************/



#ifndef MIPI_CSI2_DEV_H_
#define MIPI_CSI2_DEV_H_

#include "RTE_Components.h"
#include CMSIS_device_header
#include "Driver_MIPI_CSI2.h"

/**
\brief Camera Serial Interface (MIPI_CSI2)
*/
typedef struct {                                /*!< (@ 0x49033000) MIPI_CSI2 Structure                                        */
  __IOM uint32_t  VERSION;                      /*!< (@ 0x00000000) Version Register                                           */
  __IOM uint32_t  N_LANES;                      /*!< (@ 0x00000004) number of active lanes mipi_csi2_host Register             */
  __IOM uint32_t  CSI2_RESETN;                  /*!< (@ 0x00000008) mipi_csi2_host logic reset state Register                  */
  __IOM uint32_t  INT_ST_MAIN;                  /*!< (@ 0x0000000C) status of individual interrupt sources Register            */
  __IOM uint32_t  DATA_IDS_1;                   /*!< (@ 0x00000010) Data type fields for Data ID monitors Register             */
  __IOM uint32_t  DATA_IDS_2;                   /*!< (@ 0x00000014) Data type fields for Data ID monitors Register             */
  __IM  uint32_t  RESERVED[5];
  __IOM uint32_t  INT_ST_AP_MAIN;               /*!< (@ 0x0000002C) status of individual interrupt sources Register            */
  __IOM uint32_t  DATA_IDS_VC_1;                /*!< (@ 0x00000030) virtual channel fields for Data ID Monitors Register       */
  __IOM uint32_t  DATA_IDS_VC_2;                /*!< (@ 0x00000034) virtual channel fields for Data ID Monitors Register       */
  __IM  uint32_t  RESERVED1[2];
  __IOM uint32_t  PHY_SHUTDOWNZ;                /*!< (@ 0x00000040) control the PHY shutdown mode Register                     */
  __IOM uint32_t  DPHY_RSTZ;                    /*!< (@ 0x00000044) control the PHY reset mode Register                        */
  __IOM uint32_t  PHY_RX;                       /*!< (@ 0x00000048) RX-related signals from PHY Register                       */
  __IOM uint32_t  PHY_STOPSTATE;                /*!< (@ 0x0000004C) contains the stopstate signals from PHY Register           */
  __IOM uint32_t  PHY_TEST_CTRL0;               /*!< (@ 0x00000050) control for vendor specific interface in the
                                                                    PHY Register                                               */
  __IOM uint32_t  PHY_TEST_CTRL1;               /*!< (@ 0x00000054) control for vendor specific interface in the
                                                                    PHY Register                                               */
  __IOM uint32_t  PHY2_TEST_CTRL0;              /*!< (@ 0x00000058) control for vendor specific interface in the
                                                                    PHY Register                                               */
  __IOM uint32_t  PHY2_TEST_CTRL1;              /*!< (@ 0x0000005C) control for vendor specific interface in the
                                                                    PHY Register                                               */
  __IM  uint32_t  RESERVED2[8];
  __IOM uint32_t  IPI_MODE;                     /*!< (@ 0x00000080) IPI interface generates the video frame Register           */
  __IOM uint32_t  IPI_VCID;                     /*!< (@ 0x00000084) virtual channel processed by IPI                           */
  __IOM uint32_t  IPI_DATA_TYPE;                /*!< (@ 0x00000088) data type processed by IPI                                 */
  __IOM uint32_t  IPI_MEM_FLUSH;                /*!< (@ 0x0000008C) control the flush of IPI memory                            */
  __IOM uint32_t  IPI_HSA_TIME;                 /*!< (@ 0x00000090) video horizontal synchronism active                        */
  __IOM uint32_t  IPI_HBP_TIME;                 /*!< (@ 0x00000094) video horizontal back porch                                */
  __IOM uint32_t  IPI_HSD_TIME;                 /*!< (@ 0x00000098) video horizontal sync delay                                */
  __IOM uint32_t  IPI_HLINE_TIME;               /*!< (@ 0x0000009C) configures the overall time for each video line            */
  __IOM uint32_t  IPI_SOFTRSTN;                 /*!< (@ 0x000000A0) controls the IPI logic reset state                         */
  __IM  uint32_t  RESERVED3[2];
  __IOM uint32_t  IPI_ADV_FEATURES;             /*!< (@ 0x000000AC) configures advanced features for IPI mode                  */
  __IOM uint32_t  IPI_VSA_LINES;                /*!< (@ 0x000000B0) configures vertical synchronism active period              */
  __IOM uint32_t  IPI_VBP_LINES;                /*!< (@ 0x000000B4) configures vertical back porch period                      */
  __IOM uint32_t  IPI_VFP_LINES;                /*!< (@ 0x000000B8) configures vertical front porch period                     */
  __IOM uint32_t  IPI_VACTIVE_LINES;            /*!< (@ 0x000000BC) configures vertical resolution of the video                */
  __IM  uint32_t  RESERVED4[2];
  __IOM uint32_t  VC_EXTENSION;                 /*!< (@ 0x000000C8) virtual channel extension                                  */
  __IOM uint32_t  PHY_CAL;                      /*!< (@ 0x000000CC) contains the CALIBRATION signal status                     */
  __IM  uint32_t  RESERVED5[4];
  __IOM uint32_t  INT_ST_PHY_FATAL;             /*!< (@ 0x000000E0) fatal interruption caused by PHY packet discarded          */
  __IOM uint32_t  INT_MSK_PHY_FATAL;            /*!< (@ 0x000000E4) Interrupt mask for INT_ST_PHY_FATAL                        */
  __IOM uint32_t  INT_FORCE_PHY_FATAL;          /*!< (@ 0x000000E8) Interrupt force register used for test purposes            */
  __IM  uint32_t  RESERVED6;
  __IOM uint32_t  INT_ST_PKT_FATAL;             /*!< (@ 0x000000F0) fatal interruption related with packet construction        */
  __IOM uint32_t  INT_MSK_PKT_FATAL;            /*!< (@ 0x000000F4) Interrupt mask for INT_ST_PKT_FATAL                        */
  __IOM uint32_t  INT_FORCE_PKT_FATAL;          /*!< (@ 0x000000F8) Interrupt force used for test purpose                      */
  __IM  uint32_t  RESERVED7[5];
  __IOM uint32_t  INT_ST_PHY;                   /*!< (@ 0x00000110) Interrupt caused by PHY                                    */
  __IOM uint32_t  INT_MSK_PHY;                  /*!< (@ 0x00000114) Interrupt mask for INT_MSK_PHY                             */
  __IOM uint32_t  INT_FORCE_PHY;                /*!< (@ 0x00000118) Interrupt force register used for test purpose              */
  __IM  uint32_t  RESERVED8[5];
  __IOM uint32_t  INT_ST_LINE;                  /*!< (@ 0x00000130) Interruption related with line construction                */
  __IOM uint32_t  INT_MSK_LINE;                 /*!< (@ 0x00000134) Interrupt mask for INT_ST_LINE                             */
  __IOM uint32_t  INT_FORCE_LINE;               /*!< (@ 0x00000138) Interrupt force register used for test purpose             */
  __IM  uint32_t  RESERVED9;
  __IOM uint32_t  INT_ST_IPI_FATAL;             /*!< (@ 0x00000140) Fatal interruption caused by IPI interface                 */
  __IOM uint32_t  INT_MSK_IPI_FATAL;            /*!< (@ 0x00000144) Interrupt mask for INT_ST_IPI_FATAL                        */
  __IOM uint32_t  INT_FORCE_IPI_FATAL;          /*!< (@ 0x00000148) Interrupt force register used for test purposes            */
  __IM  uint32_t  RESERVED10;
  __IOM uint32_t  INT_ST_IPI2_FATAL;            /*!< (@ 0x00000150) Fatal interruption caused by IPI interface 2               */
  __IOM uint32_t  INT_MSK_IPI2_FATAL;           /*!< (@ 0x00000154) Interrupt mask for INT_ST_IPI2_FATAL                       */
  __IOM uint32_t  INT_FORCE_IPI2_FATAL;         /*!< (@ 0x00000158) Interrupt force register used for test purposes            */
  __IM  uint32_t  RESERVED11;
  __IOM uint32_t  INT_ST_IPI3_FATAL;            /*!< (@ 0x00000160) Fatal interruption caused by IPI interface 3               */
  __IOM uint32_t  INT_MSK_IPI3_FATAL;           /*!< (@ 0x00000164) Interrupt mask for INT_ST_IPI3_FATAL                       */
  __IOM uint32_t  INT_FORCE_IPI3_FATAL;         /*!< (@ 0x00000168) Interrupt force register used for test purposes            */
  __IM  uint32_t  RESERVED12;
  __IOM uint32_t  INT_ST_IPI4_FATAL;            /*!< (@ 0x00000170) Fatal interruption caused by IPI interface 4               */
  __IOM uint32_t  INT_MSK_IPI4_FATAL;           /*!< (@ 0x00000174) Interrupt mask for INT_ST_IPI4_FATAL                       */
  __IOM uint32_t  INT_FORCE_IPI4_FATAL;         /*!< (@ 0x00000178) Interrupt force register used for test purposes            */
  __IM  uint32_t  RESERVED13;
  __IOM uint32_t  INT_ST_AP_GENERIC;            /*!< (@ 0x00000180) notifies which interruption bits caused the interruption   */
  __IOM uint32_t  INT_MSK_AP_GENERIC;           /*!< (@ 0x00000184) interrupt mask for INT_ST_AP_GENERIC                       */
  __IOM uint32_t  INT_FORCE_AP_GENERIC;         /*!< (@ 0x00000188) interrupt mask for INT_ST_AP_GENERIC                       */
  __IM  uint32_t  RESERVED14;
  __IOM uint32_t  INT_ST_AP_IPI_FATAL;          /*!< (@ 0x00000190) notifies which interruption bits caused the interruption   */
  __IOM uint32_t  INT_MSK_AP_IPI_FATAL;         /*!< (@ 0x00000194) notifies which interruption bits caused the interruption   */
  __IOM uint32_t  INT_FORCE_AP_IPI_FATAL;       /*!< (@ 0x00000198) Interrupt force register is used for test purposes         */
  __IM  uint32_t  RESERVED15;
  __IOM uint32_t  INT_ST_AP_IPI2_FATAL;         /*!< (@ 0x000001A0) notifies which interruption bits caused the interruption   */
  __IOM uint32_t  INT_MSK_AP_IPI2_FATAL;        /*!< (@ 0x000001A4) notifies which interruption bits caused the interruption   */
  __IOM uint32_t  INT_FORCE_AP_IPI2_FATAL;      /*!< (@ 0x000001A8) Interrupt force register is used for test purposes         */
  __IM  uint32_t  RESERVED16;
  __IOM uint32_t  INT_ST_AP_IPI3_FATAL;         /*!< (@ 0x000001B0) notifies which interruption bits caused the interruption   */
  __IOM uint32_t  INT_MSK_AP_IPI3_FATAL;        /*!< (@ 0x000001B4) notifies which interruption bits caused the interruption   */
  __IOM uint32_t  INT_FORCE_AP_IPI3_FATAL;      /*!< (@ 0x000001B8) Interrupt force register is used for test purposes         */
  __IM  uint32_t  RESERVED17;
  __IOM uint32_t  INT_ST_AP_IPI4_FATAL;         /*!< (@ 0x000001C0) notifies which interruption bits caused the interruption   */
  __IOM uint32_t  INT_MSK_AP_IPI4_FATAL;        /*!< (@ 0x000001C4) notifies which interruption bits caused the interruption   */
  __IOM uint32_t  INT_FORCE_AP_IPI4_FATAL;      /*!< (@ 0x000001C8) Interrupt force register is used for test purposes         */
  __IM  uint32_t  RESERVED18[13];
  __IOM uint32_t  IPI2_MODE;                    /*!< (@ 0x00000200) This register selects how the IPI interface 2
                                                                    generates the video frame                                  */
  __IOM uint32_t  IPI2_VCID;                    /*!< (@ 0x00000204) virtual channel processed by IPI2                          */
  __IOM uint32_t  IPI2_DATA_TYPE;               /*!< (@ 0x00000208) data type processed by IPI2                                */
  __IOM uint32_t  IPI2_MEM_FLUSH;               /*!< (@ 0x0000020C) control the flush of IPI2 memory                           */
  __IOM uint32_t  IPI2_HSA_TIME;                /*!< (@ 0x00000210) video horizontal synchronism active                        */
  __IOM uint32_t  IPI2_HBP_TIME;                /*!< (@ 0x00000214) video horizontal back porch                                */
  __IOM uint32_t  IPI2_HSD_TIME;                /*!< (@ 0x00000218) video horizontal sync delay                                */
  __IOM uint32_t  IPI2_ADV_FEATURES;            /*!< (@ 0x0000021C) configures advanced features for IPI2 mode                 */
  __IOM uint32_t  IPI3_MODE;                    /*!< (@ 0x00000220) This register selects how the IPI interface 3
                                                                    generates the video frame                                  */
  __IOM uint32_t  IPI3_VCID;                    /*!< (@ 0x00000224) virtual channel processed by IPI3                          */
  __IOM uint32_t  IPI3_DATA_TYPE;               /*!< (@ 0x00000228) data type processed by IPI3                                */
  __IOM uint32_t  IPI3_MEM_FLUSH;               /*!< (@ 0x0000022C) control the flush of IPI3 memory                           */
  __IOM uint32_t  IPI3_HSA_TIME;                /*!< (@ 0x00000230) video horizontal synchronism active                        */
  __IOM uint32_t  IPI3_HBP_TIME;                /*!< (@ 0x00000234) video horizontal back porch                                */
  __IOM uint32_t  IPI3_HSD_TIME;                /*!< (@ 0x00000238) video horizontal sync delay                                */
  __IOM uint32_t  IPI3_ADV_FEATURES;            /*!< (@ 0x0000023C) configures advanced features for IPI3 mode                 */
  __IOM uint32_t  IPI4_MODE;                    /*!< (@ 0x00000240) This register selects how the IPI interface 4
                                                                    generates the video frame                                  */
  __IOM uint32_t  IPI4_VCID;                    /*!< (@ 0x00000244) virtual channel processed by IPI4                          */
  __IOM uint32_t  IPI4_DATA_TYPE;               /*!< (@ 0x00000248) data type processed by IPI4                                */
  __IOM uint32_t  IPI4_MEM_FLUSH;               /*!< (@ 0x0000024C) control the flush of IPI4 memory                           */
  __IOM uint32_t  IPI4_HSA_TIME;                /*!< (@ 0x00000250) video horizontal synchronism active                        */
  __IOM uint32_t  IPI4_HBP_TIME;                /*!< (@ 0x00000254) video horizontal back porch                                */
  __IOM uint32_t  IPI4_HSD_TIME;                /*!< (@ 0x00000258) video horizontal sync delay                                */
  __IOM uint32_t  IPI4_ADV_FEATURES;            /*!< (@ 0x0000025C) configures advanced features for IPI4 mode                 */
  __IM  uint32_t  RESERVED19[8];
  __IOM uint32_t  INT_ST_BNDRY_FRAME_FATAL;     /*!< (@ 0x00000280) fatal interruption related with matching Frame
                                                                    start with frame end for specific virtual
                                                                    channel                                                    */
  __IOM uint32_t  INT_MSK_BNDRY_FRAME_FATAL;    /*!< (@ 0x00000284) Interrupt mask for INT_ST_BNDRY_FRAME_FATAL                */
  __IOM uint32_t  INT_FORCE_BNDRY_FRAME_FATAL;  /*!< (@ 0x00000288) Interrupt force register used for test purposes            */
  __IM  uint32_t  RESERVED20;
  __IOM uint32_t  INT_ST_SEQ_FRAME_FATAL;       /*!< (@ 0x00000290) Interrupt force register used for test purposes            */
  __IOM uint32_t  INT_MSK_SEQ_FRAME_FATAL;      /*!< (@ 0x00000294) Interrupt mask for INT_ST_SEQ_FRAME_FATAL                  */
  __IOM uint32_t  INT_FORCE_SEQ_FRAME_FATAL;    /*!< (@ 0x00000298) force register used for test purposes                      */
  __IM  uint32_t  RESERVED21;
  __IOM uint32_t  INT_ST_CRC_FRAME_FATAL;       /*!< (@ 0x000002A0) Interruption related with frames with at least
                                                                    one CRC error for specific virtual channel                 */
  __IOM uint32_t  INT_MSK_CRC_FRAME_FATAL;      /*!< (@ 0x000002A4) Interrupt mask for INT_ST_CRC_FRAME_FATAL                  */
  __IOM uint32_t  INT_FORCE_CRC_FRAME_FATAL;    /*!< (@ 0x000002A8) Interruption force register used for test purposes         */
  __IM  uint32_t  RESERVED22;
  __IOM uint32_t  INT_ST_PLD_CRC_FATAL;         /*!< (@ 0x000002B0) Interruption related with payload Checksum error           */
  __IOM uint32_t  INT_MSK_PLD_CRC_FATAL;        /*!< (@ 0x000002B4) Interrupt mask for INT_ST_PLD_CRC_FATAL                    */
  __IOM uint32_t  INT_FORCE_PLD_CRC_FATASL;      /*!< (@ 0x000002B8) Interruption force register used for test purposes         */
  __IM  uint32_t  RESERVED23;
  __IOM uint32_t  INT_ST_DATA_ID;               /*!< (@ 0x000002C0) Interruption related with the unrecognised or
                                                                    unimplemented data type detected in a specific
                                                                    virtual channel                                            */
  __IOM uint32_t  INT_MSK_DATA_ID;              /*!< (@ 0x000002C4) Interrupt mask for INT_ST_DATA_ID                          */
  __IOM uint32_t  INT_FORCE_DATA_ID;            /*!< (@ 0x000002C8) Interruption force register used for test purposes         */
  __IM  uint32_t  RESERVED24;
  __IOM uint32_t  INT_ST_ECC_CORRECT;           /*!< (@ 0x000002D0) Interruption related with header error detected
                                                                    and corrected for a specific virtual channel               */
  __IOM uint32_t  INT_MSK_ECC_CORRECT;          /*!< (@ 0x000002D4) Interrupt mask for INT_ST_ECC_CORRECT                      */
  __IOM uint32_t  INT_FORCE_ECC_CORRECT;        /*!< (@ 0x000002D8) Interruption force register used for test purposes         */
  __IM  uint32_t  RESERVED25;
  __IOM uint32_t  IPI_RAM_ERR_LOG_AP;           /*!< (@ 0x000002E0) This register stores the faulty RAM addresses
                                                                    in FMEDA mode                                              */
  __IOM uint32_t  IPI_RAM_ERR_ADDR_AP;          /*!< (@ 0x000002E4) This register stores the first faulty RAM addresses
                                                                    in FMEDA mode                                              */
  __IOM uint32_t  IPI2_RAM_ERR_LOG_AP;          /*!< (@ 0x000002E8) This register stores the faulty RAM addresses
                                                                    in FMEDA mode                                              */
  __IOM uint32_t  IPI2_RAM_ERR_ADDR_AP;         /*!< (@ 0x000002EC) This register stores the first faulty RAM addresses
                                                                    in FMEDA mode                                              */
  __IOM uint32_t  IPI3_RAM_ERR_LOG_AP;          /*!< (@ 0x000002F0) This register stores the faulty RAM addresses
                                                                    in FMEDA mode                                              */
  __IOM uint32_t  IPI3_RAM_ERR_ADDR_AP;         /*!< (@ 0x000002F4) This register stores the first faulty RAM addresses
                                                                    in FMEDA mode                                              */
  __IOM uint32_t  IPI4_RAM_ERR_LOG_AP;          /*!< (@ 0x000002F8) This register stores the faulty RAM addresses
                                                                    in FMEDA mode                                              */
  __IOM uint32_t  IPI4_RAM_ERR_ADDR_AP;         /*!< (@ 0x000002FC) This register stores the first faulty RAM addresses
                                                                    in FMEDA mode                                              */
  __IOM uint32_t  SCRAMBLING;                   /*!< (@ 0x00000300) This register configures De-scrambler block                */
  __IOM uint32_t  SCRAMBLING_SEED1;             /*!< (@ 0x00000304) This register configures De-scrambler block for
                                                                    lane 1                                                     */
  __IOM uint32_t  SCRAMBLING_SEED2;             /*!< (@ 0x00000308) This register configures De-scrambler block for
                                                                    lane 1                                                     */
  __IOM uint32_t  SCRAMBLING_SEED3;             /*!< (@ 0x0000030C) This register configures De-scrambler block for
                                                                    lane 3                                                     */
  __IOM uint32_t  SCRAMBLING_SEED4;             /*!< (@ 0x00000310) This register configures De-scrambler block for
                                                                    lane 4                                                     */
  __IOM uint32_t  SCRAMBLING_SEED5;             /*!< (@ 0x00000314) This register configures De-scrambler block for
                                                                    lane 5                                                     */
  __IOM uint32_t  SCRAMBLING_SEED6;             /*!< (@ 0x00000318) This register configures De-scrambler block for
                                                                    lane 6                                                     */
  __IOM uint32_t  SCRAMBLING_SEED7;             /*!< (@ 0x0000031C) This register configures De-scrambler block for
                                                                    lane 7                                                     */
  __IOM uint32_t  SCRAMBLING_SEED8;             /*!< (@ 0x00000320) This register configures De-scrambler block for
                                                                    lane 8                                                     */
  __IM  uint32_t  RESERVED26[7];
  __IOM uint32_t  N_SYNC;                       /*!< (@ 0x00000340) This register configures advanced features for
                                                                    FMEDA mode                                                 */
  __IOM uint32_t  ERR_INJ_CTRL_AP;              /*!< (@ 0x00000344) This register configures IPI memory error injection
                                                                    for FMEDA mode                                             */
  __IOM uint32_t  ERR_INJ_CHK_MSK_AP;           /*!< (@ 0x00000348) This register configures the error injection
                                                                    mask for ECC check bits in FMEDA mode                      */
  __IOM uint32_t  ERR_INJ_DH32_MSK_AP;          /*!< (@ 0x0000034C) This register configures the error injection
                                                                    mask for data high 32bits in FMEDA mode                    */
  __IOM uint32_t  ERR_INJ_DL32_MSK_AP;          /*!< (@ 0x00000350) This register configures the error injection
                                                                    mask for data high 32bits in FMEDA mode                    */
  __IOM uint32_t  ERR_INJ_ST_AP;                /*!< (@ 0x00000354) This register configures the error injection
                                                                    mask for data high 32bits in FMEDA mode                    */
  __IM  uint32_t  RESERVED27[2];
  __IOM uint32_t  INT_ST_FAP_PHY_FATAL;         /*!< (@ 0x00000360) Interruptions caused by PHY packet discarded               */
  __IOM uint32_t  INT_MSK_FAP_PHY_FATAL;        /*!< (@ 0x00000364) Interrupt mask for INT_ST_FAP_PHY_FATAL                    */
  __IOM uint32_t  INT_FORCE_FAP_PHY_FATAL;      /*!< (@ 0x00000368) Interrupt force register used for test purposes            */
  __IM  uint32_t  RESERVED28;
  __IOM uint32_t  INT_ST_FAP_PKT_FATAL;         /*!< (@ 0x00000370) Interruption related with packet construction
                                                                    and packet discarded                                       */
  __IOM uint32_t  INT_MSK_FAP_PKT_FATAL;        /*!< (@ 0x00000374) Interrupt mask for INT_ST_PKT_FATAL                        */
  __IOM uint32_t  INT_FORCE_FAP_PKT_FATAL;      /*!< (@ 0x00000378) Interrupt mask for INT_ST_PKT_FATAL                        */
  __IM  uint32_t  RESERVED29[5];
  __IOM uint32_t  INT_ST_FAP_PHY;               /*!< (@ 0x00000390) Interruption caused by PHY                                 */
  __IOM uint32_t  INT_MSK_FAP_PHY;              /*!< (@ 0x00000394) Interrupt mask for INT_ST_PHY                              */
  __IOM uint32_t  INT_FORCE_FAP_PHY;            /*!< (@ 0x00000398) Interrupt force register used for test purposes            */
  __IM  uint32_t  RESERVED30[5];
  __IOM uint32_t  INT_ST_FAP_LINE;              /*!< (@ 0x000003B0) Interrupt related with Line construction                   */
  __IOM uint32_t  INT_MSK_FAP_LINE;             /*!< (@ 0x000003B4) Interrupt mask for INT_ST_LINE                             */
  __IOM uint32_t  INT_FORCE_FAP_LINE;           /*!< (@ 0x000003B8) Interrupt force register is used for test purposes         */
  __IM  uint32_t  RESERVED31;
  __IOM uint32_t  INT_ST_FAP_IPI_LINE;          /*!< (@ 0x000003C0) fatal Interruption caused by IPI Interface                 */
  __IOM uint32_t  INT_MSK_FAP_IPI_LINE;         /*!< (@ 0x000003C4) Interrupt mask for INT_ST_IPI_FATAL                        */
  __IOM uint32_t  INT_FORCE_FAP_IPI_LINE;       /*!< (@ 0x000003C8) Interrupt force register is used for test purposes         */
  __IM  uint32_t  RESERVED32;
  __IOM uint32_t  INT_ST_FAP_IPI2_LINE;         /*!< (@ 0x000003D0) fatal Interruption caused by IPI Interface 2               */
  __IOM uint32_t  INT_MSK_FAP_IPI2_LINE;        /*!< (@ 0x000003D4) Interrupt mask for INT_ST_IPI2_FATAL                       */
  __IOM uint32_t  INT_FORCE_FAP_IPI2_LINE;      /*!< (@ 0x000003D8) Interrupt force register is used for test purposes         */
  __IM  uint32_t  RESERVED33;
  __IOM uint32_t  INT_ST_FAP_IPI3_LINE;         /*!< (@ 0x000003E0) fatal Interruption caused by IPI Interface 3               */
  __IOM uint32_t  INT_MSK_FAP_IPI3_LINE;        /*!< (@ 0x000003E4) Interrupt mask for INT_ST_IPI3_FATAL                       */
  __IOM uint32_t  INT_FORCE_FAP_IPI3_LINE;      /*!< (@ 0x000003E8) Interrupt force register is used for test purposes         */
  __IM  uint32_t  RESERVED34;
  __IOM uint32_t  INT_ST_FAP_IPI4_LINE;         /*!< (@ 0x000003F0) fatal Interruption caused by IPI Interface 4               */
  __IOM uint32_t  INT_MSK_FAP_IPI4_LINE;        /*!< (@ 0x000003F4) Interrupt mask for INT_ST_IPI4_FATAL                       */
  __IOM uint32_t  INT_FORCE_FAP_IPI4_LINE;      /*!< (@ 0x000003F8) Interrupt force register is used for test purposes         */
  __IM  uint32_t  RESERVED35[9];
  __IOM uint32_t  INT_ST_FAP_BNDRY_FRAME_FATAL; /*!< (@ 0x00000420) fatal interruption related with matching Frame
                                                                    start with frame end for specific virtual
                                                                    channel                                                    */
  __IOM uint32_t  INT_MSK_FAP_BNDRY_FRAME_FATAL;/*!< (@ 0x00000424) Interrupt mask for INT_ST_BNDRY_FRAME_FATAL                */
  __IOM uint32_t  INT_FORCE_FAP_BNDRY_FRAME_FATAL;/*!< (@ 0x00000428) Interrupt force register used for test purposes          */
  __IM  uint32_t  RESERVED36;
  __IOM uint32_t  INT_ST_FAP_SEQ_FRAME_FATAL;   /*!< (@ 0x00000430) Interrupt force register used for test purposes            */
  __IOM uint32_t  INT_MSK_FAP_SEQ_FRAME_FATAL;  /*!< (@ 0x00000434) Interrupt mask for INT_ST_SEQ_FRAME_FATAL                  */
  __IOM uint32_t  INT_FORCE_FAP_SEQ_FRAME_FATAL;/*!< (@ 0x00000438) force register used for test purposes                      */
  __IM  uint32_t  RESERVED37;
  __IOM uint32_t  INT_ST_FAP_CRC_FRAME_FATAL;   /*!< (@ 0x00000440) Interruption related with frames with at least
                                                                    one CRC error for specific virtual channel                 */
  __IOM uint32_t  INT_MSK_FAP_CRC_FRAME_FATAL;  /*!< (@ 0x00000444) Interrupt mask for INT_ST_CRC_FRAME_FATAL                  */
  __IOM uint32_t  INT_FORCE_FAP_CRC_FRAME_FATAL;/*!< (@ 0x00000448) Interruption force register used for test purposes         */
  __IM  uint32_t  RESERVED38;
  __IOM uint32_t  INT_ST_FAP_PLD_CRC_FATAL;     /*!< (@ 0x00000450) Interruption related with payload Checksum error           */
  __IOM uint32_t  IPI_MSK_FAP_PLD_CRC_FATAL;    /*!< (@ 0x00000454) Interrupt mask for INT_ST_PLD_CRC_FATAL                    */
  __IOM uint32_t  INT_FORCE_FAP_PLD_CRC_FATAL;  /*!< (@ 0x00000458) Interruption force register used for test purposes         */
  __IM  uint32_t  RESERVED39;
  __IOM uint32_t  INT_ST_FAP_DATA_ID;           /*!< (@ 0x00000460) Interruption related with the unrecognised or
                                                                    unimplemented data type detected in a specific
                                                                    virtual channel                                            */
  __IOM uint32_t  INT_MSK_FAP_DATA_ID;          /*!< (@ 0x00000464) Interrupt mask for INT_ST_DATA_ID                          */
  __IOM uint32_t  INT_FORCE_FAP_DATA_ID;        /*!< (@ 0x00000468) Interruption force register used for test purposes         */
  __IM  uint32_t  RESERVED40;
  __IOM uint32_t  INT_ST_FAP_ECC_CORRECT;       /*!< (@ 0x00000470) Interruption related with header error detected
                                                                    and corrected for a specific virtual channel               */
  __IOM uint32_t  INT_MSK_FAP_ECC_CORRECT;      /*!< (@ 0x00000474) Interrupt mask for INT_ST_ECC_CORRECT                      */
  __IOM uint32_t  INT_FORCE_FAP_ECC_CORRECT;    /*!< (@ 0x00000478) Interruption force register used for test purposes         */
} MIPI_CSI2_Type;                               /*!< Size = 1148 (0x47c)                                                       */

#define MIPI_CSI2                                ((MIPI_CSI2_Type*)MIPI_CSI2_BASE)

/*N_LANES register bits parameters*/
#define CSI2_N_LANES_Pos                         0U
#define CSI2_N_LANES_Msk                         MASK(2,0)

/*CSI2_RESETN bits parameters*/
#define CSI2_RESETN_Pos                          0U
#define CSI2_RESETN_Msk                          (1U << CSI2_RESETN_Pos)

/*INT_ST_MAIN register bits parameters*/
#define CSI2_STATUS_INT_PHY_FATAL_Pos            0U
#define CSI2_STATUS_INT_PHY_FATAL_Msk            (1U << CSI2_STATUS_INT_PHY_FATAL_Pos)
#define CSI2_STATUS_INT_PKT_FATAL_Pos            1U
#define CSI2_STATUS_INT_PKT_FATAL_Msk            (1U << CSI2_STATUS_INT_PKT_FATAL_Pos)
#define CSI2_STATUS_INT_BNDRY_FRAME_FATAL_Pos    2U
#define CSI2_STATUS_INT_BNDRY_FRAME_FATAL_Msk    (1U << CSI2_STATUS_INT_BNDRY_FRAME_FATAL_Pos)
#define CSI2_STATUS_INT_SEQ_FRAME_FATAL_Pos      3U
#define CSI2_STATUS_INT_SEQ_FRAME_FATAL_Msk      (1U << CSI2_STATUS_INT_SEQ_FRAME_FATAL_Pos)
#define CSI2_STATUS_INT_CRC_FRAME_FATAL_Pos      4U
#define CSI2_STATUS_INT_CRC_FRAME_FATAL_Msk      (1U << CSI2_STATUS_INT_CRC_FRAME_FATAL_Pos)
#define CSI2_STATUS_INT_PLD_CRC_FATAL_Pos        5U
#define CSI2_STATUS_INT_PLD_CRC_FATAL_Msk        (1U << CSI2_STATUS_INT_PLD_CRC_FATAL_Pos)
#define CSI2_STATUS_INT_DATA_ID_Pos              6U
#define CSI2_STATUS_INT_DATA_ID_Msk              (1U << CSI2_STATUS_INT_DATA_ID_Pos)
#define CSI2_STATUS_INT_ECC_CORRECTED_Pos        7U
#define CSI2_STATUS_INT_ECC_CORRECTED_Msk        (1U << CSI2_STATUS_INT_ECC_CORRECTED_Pos)
#define CSI2_STATUS_INT_PHY_Pos                  16U
#define CSI2_STATUS_INT_PHY_Msk                  (1U << CSI2_STATUS_INT_PHY_Pos)
#define CSI2_STATUS_INT_LINE_Pos                 17U
#define CSI2_STATUS_INT_LINE_Msk                 (1U << CSI2_STATUS_INT_LINE_Pos)
#define CSI2_STATUS_INT_IPI_FATAL_Pos            18U
#define CSI2_STATUS_INT_IPI_FATAL_Msk            (1U << CSI2_STATUS_INT_IPI_FATAL_Pos)

/*PHY_SHUTDOWNZ register bits parameters*/
#define CSI2_PHY_SHUTDOWNZ_Pos                   0U
#define CSI2_PHY_SHUTDOWNZ_Msk                   (1U << CSI2_PHY_SHUTDOWNZ_Pos)

/*DPHY_RSTZ register bits parameters*/
#define CSI2_PHY_RSTZ_Pos                        0U
#define CSI2_PHY_RSTZ_Msk                        (1U << CSI2_PHY_RSTZ_Pos)

/*PHY_RX register bits parameters*/
#define CSI2_PHY_RXULPSESC_LANE_0_Pos            0U
#define CSI2_PHY_RXULPSESC_LANE_0_Msk            (1U << CSI2_PHY_RXULPSESC_LANE_0_Pos)
#define CSI2_PHY_RXULPSESC_LANE_1_Pos            1U
#define CSI2_PHY_RXULPSESC_LANE_1_Msk            (1U << CSI2_PHY_RXULPSESC_LANE_1_Pos)
#define CSI2_PHY_RXULPSCLKNOT_Pos                16U
#define CSI2_PHY_RXULPSCLKNOT_Msk                (1U << CSI2_PHY_RXULPSCLKNOT_Pos)
#define CSI2_PHY_RXCLKACTIVEHS_Pos               17U
#define CSI2_PHY_RXCLKACTIVEHS_Msk               (1U << CSI2_PHY_RXCLKACTIVEHS_Pos)

/*PHY_STOPSTATE register bits parameters*/
#define CSI2_PHY_STOPSTATEDATA_0_Pos             0U
#define CSI2_PHY_STOPSTATEDATA_0_Msk             (1U << CSI2_PHY_STOPSTATEDATA_0_Pos)
#define CSI2_PHY_STOPSTATEDATA_1_Pos             1U
#define CSI2_PHY_STOPSTATEDATA_1_Msk             (1U << CSI2_PHY_STOPSTATEDATA_1_Pos)
#define CSI2_PHY_STOPSTATECLK_Pos                16U
#define CSI2_PHY_STOPSTATECLK_Msk                (1U << CSI2_PHY_STOPSTATECLK_Pos)

/*IPI_MODE register bits parameters*/
#define CSI2_IPI_MODE_Pos                        0U
#define CSI2_IPI_MODE_Msk                        (1U << CSI2_IPI_MODE_Pos)
#define CSI2_IPI_COLOR_COM_Pos                   8U
#define CSI2_IPI_COLOR_COM_Msk                   (1U << CSI2_IPI_COLOR_COM_Pos)
#define CSI2_IPI_CUT_THROUGH_Pos                 16U
#define CSI2_IPI_CUT_THROUGH_Msk                 (1U << CSI2_IPI_CUT_THROUGH_Pos)
#define CSI2_IPI_ENABLE_Pos                      24U
#define CSI2_IPI_ENABLE_Msk                      (1U << CSI2_IPI_ENABLE_Pos)

/*IPI_VCID register bits parameters*/
#define CSI2_IPI_VCID_Pos                        0U
#define CSI2_IPI_VCID_Msk                        MASK(1,0)

/*IPI_DATA_TYPE register bits parameters*/
#define CSI2_IPI_DATA_TYPE_Pos                   0U
#define CSI2_IPI_DATA_TYPE_Msk                   MASK(5,0)

/*IPI_MEM_FLUSH register bits parameters*/
#define CSI2_IPI_FLUSH_Pos                       0U
#define CSI2_IPI_FLUSH_Msk                       (1U << CSI2_IPI_FLUSH_Pos)
#define CSI2_IPI_AUTO_FLUSH_Pos                  8U
#define CSI2_IPI_AUTO_FLUSH_Msk                  (1U << CSI2_IPI_AUTO_FLUSH_Pos)

/*IPI_ADV_FEATURES register bits parameters*/
#define CSI2_IPI_DT_OVERWRITE_Pos                0U
#define CSI2_IPI_DT_OVERWRITE_Msk                (1U << CSI2_IPI_DT_OVERWRITE_Pos)
#define CSI2_IPI_DT_Pos                          8U
#define CSI2_IPI_DT_Msk                          MASK(13,8)
#define CSI2_IPI_LINE_EVENT_SELECTION_Pos        16U
#define CSI2_IPI_LINE_EVENT_SELECTION_Msk        (1U << CSI2_IPI_LINE_EVENT_SELECTION_Pos)
#define CSI2_IPI_EN_VIDEO_Pos                    17U
#define CSI2_IPI_EN_VIDEO_Msk                    (1U << CSI2_IPI_EN_VIDEO_Pos)
#define CSI2_IPI_EN_LINE_START_Pos               18U
#define CSI2_IPI_EN_LINE_START_Msk               (1U << CSI2_IPI_EN_LINE_START_Pos)
#define CSI2_IPI_EN_NULL_Pos                     19U
#define CSI2_IPI_EN_NULL_Msk                     (1U << CSI2_IPI_EN_NULL_Pos)
#define CSI2_IPI_EN_BLANKING_Pos                 20U
#define CSI2_IPI_EN_BLANKING_Msk                 (1U << CSI2_IPI_EN_BLANKING_Pos)
#define CSI2_IPI_EN_EMBEDDED_Pos                 21U
#define CSI2_IPI_EN_EMBEDDED_Msk                 (1U << CSI2_IPI_EN_EMBEDDED_Pos)
#define CSI2_IPI_SYNC_EVENT_MODE_Pos             24U
#define CSI2_IPI_SYNC_EVENT_MODE_Msk             (1U << CSI2_IPI_SYNC_EVENT_MODE_Pos)

/*IPI_HSA_TIME register bits parameters*/
#define CSI2_IPI_HSA_TIME_Pos                    0U
#define CSI2_IPI_HSA_TIME_Msk                    MASK(11,0)

/*IPI_HBP_TIME register bits parameters*/
#define CSI2_IPI_HBP_TIME_Pos                    0U
#define CSI2_IPI_HBP_TIME_Msk                    MASK(11,0)

/*IPI_HSD_TIME register bits parameters*/
#define CSI2_IPI_HSD_TIME_Pos                    0U
#define CSI2_IPI_HSD_TIME_Msk                    MASK(11,0)

/*IPI_HLINE_TIME register bits parameters*/
#define CSI2_IPI_HLINE_TIME_Pos                  0U
#define CSI2_IPI_HLINE_TIME_Msk                  MASK(14,0)

/*IPI_SOFTRSTN register bits parameters*/
#define CSI2_IPI_SOFTRSTN_Pos                    0U
#define CSI2_IPI_SOFTRSTN_Msk                    (1U << CSI2_IPI_SOFTRSTN_Pos)

/*IPI_VSA_LINES register bits parameters*/
#define CSI2_IPI_VSA_LINE_Pos                    0U
#define CSI2_IPI_VSA_LINE_Msk                    MASK(9,0)

/*IPI_VBP_LINES register bits parameters*/
#define CSI2_IPI_VBP_LINE_Pos                    0U
#define CSI2_IPI_VBP_LINE_Msk                    MASK(9,0)

/*IPI_VFP_LINES register bits parameters*/
#define CSI2_IPI_VFP_LINE_Pos                    0U
#define CSI2_IPI_VFP_LINE_Msk                    MASK(9,0)

/*IPI_VACTIVE_LINES register bits parameters*/
#define CSI2_IPI_VACTIVE_LINE_Pos                0U
#define CSI2_IPI_VACTIVE_LINE_Msk                MASK(13,0)

/*CSI_PIXCLK_CTRL register bits parameters*/
#define CSI2_CLK_EN_Pos                          0U
#define CSI2_CLK_EN_Msk                          (1U << CSI2_CLK_EN_Pos)
#define CSI2_CLK_DIV_Pos                         16U
#define CSI2_CLK_DIV_Msk                         MASK(24,16)

/*MIPI CSI2 helper macros*/
#define MIPI_CSI2_SET_BIT(REG, BIT)              SET_BIT(REG, BIT)
#define MIPI_CSI2_CLEAR_BIT(REG, BIT)            CLEAR_BIT(REG, BIT)
#define MIPI_CSI2_READ_BIT(REG, BIT)             READ_BIT(REG, BIT)
#define MIPI_CSI2_CLEAR_REG(REG)                 CLEAR_REG(REG)
#define MIPI_CSI2_WRITE_REG(REG, VAL)            WRITE_REG(REG, VAL)
#define MIPI_CSI2_READ_REG(REG)                  READ_REG(REG)

/**
\brief MIPI CSI2 Pixel data types
*/
typedef enum {
	MIPI_CSI2_DT_YUV420_8,
	MIPI_CSI2_DT_YUV420_10,
	MIPI_CSI2_DT_YUV420_8_LEGACY,
	MIPI_CSI2_DT_YUV420_8_SHIFT,
	MIPI_CSI2_DT_YUV420_10_SHIFT,
	MIPI_CSI2_DT_YUV422_8,
	MIPI_CSI2_DT_YUV422_10,
	MIPI_CSI2_DT_RGB444,
	MIPI_CSI2_DT_RGB555,
	MIPI_CSI2_DT_RGB565,
	MIPI_CSI2_DT_RGB666,
	MIPI_CSI2_DT_RGB888,
	MIPI_CSI2_DT_RAW6,
	MIPI_CSI2_DT_RAW7,
	MIPI_CSI2_DT_RAW8,
	MIPI_CSI2_DT_RAW10,
	MIPI_CSI2_DT_RAW12,
	MIPI_CSI2_DT_RAW14,
	MIPI_CSI2_DT_RAW16,
	MIPI_CSI2_DT_USER_DEFINED_1,
	MIPI_CSI2_DT_USER_DEFINED_2,
	MIPI_CSI2_DT_USER_DEFINED_3,
	MIPI_CSI2_DT_USER_DEFINED_4,
	MIPI_CSI2_DT_USER_DEFINED_5,
	MIPI_CSI2_DT_USER_DEFINED_6,
	MIPI_CSI2_DT_USER_DEFINED_7,
	MIPI_CSI2_DT_USER_DEFINED_8,
}CSI2_PIXEL_DATA_Type;

/**
\brief MIPI CSI2 Number of data lanes
*/
typedef enum {
	MIPI_CSI2_ONE_LANE,
	MIPI_CSI2_TWO_LANE,
}CSI2_N_LANES_Type;

/**
\brief MIPI CSI2 virtual channel ID
*/
typedef enum {
	MIPI_CSI2_VC_ID_0,
	MIPI_CSI2_VC_ID_1,
	MIPI_CSI2_VC_ID_2,
	MIPI_CSI2_VC_ID_3,
}CSI2_VC_ID_Type;

/**
\brief MIPI CSI2 IPI sync event mode
*/
typedef enum {
	MIPI_CSI2_IPI_SYNCEVFSN,
	MIPI_CSI2_IPI_SYNCEVFS
}CSI2_IPI_SYNC_EVENT_Type;

/**
\brief MIPI CSI2 IPI sync event selection
*/
typedef enum {
	MIPI_CSI2_IPI_EVSELAUTO,
	MIPI_CSI2_IPI_EVSELPROG
}CSI2_IPI_EVENT_SEL_Type;

/**
\brief MIPI CSI2 IPI modes
*/
typedef enum {
	MIPI_CSI2_IPI_CAMMODE,
	MIPI_CSI2_IPI_CTRLMODE,
}CSI2_IPI_MODE_Type;

/**
\brief MIPI CSI2 IPI color component
*/
typedef enum {
	MIPI_CSI2_IPI_COLOR48,
	MIPI_CSI2_IPI_COLOR16,
}CSI2_IPI_COLOR_COP_Type;

/**
\brief MIPI CSI2 IPI memory flush
*/
typedef enum {
	MIPI_CSI2_IPI_AUTO,
	MIPI_CSI2_IPI_MANUAL,
}CSI2_IPI_MEMFLUSH_Type;

/**
\brief MIPI CSI2 Driver flags
*/
typedef enum {
	CSI2_FLAG_DRV_INIT_DONE   = (1U << 0),
	CSI2_FLAG_CLKSRC_ENABLED  = (1U << 1),
	CSI2_FLAG_HOST_CONFIGURED = (1U << 2),
	CSI2_FLAG_IPI_CONFIGURED  = (1U << 3),
}CSI2_FLAG_Type;

/**
\brief MIPI CSI2 IPI frame info
*/
typedef struct {
	uint32_t hsa_time;
	uint32_t hbp_time;
	uint32_t hsd_time;
	uint32_t hactive_time;
	uint32_t vsa_line;
	uint32_t vbp_line;
	uint32_t vfp_line;
	uint32_t vactive_line;
}ARM_MIPI_CSI2_FRAME_INFO;

/**
\brief MIPI CSI2 IPI advanced features
*/
typedef struct {
	CSI2_IPI_SYNC_EVENT_Type sync_evnt_mode;
	CSI2_IPI_EVENT_SEL_Type  event_sel;
	uint8_t                  en_embedded;
	uint8_t                  en_blanking;
	uint8_t                  en_null;
	uint8_t                  en_line_start;
	uint8_t                  en_video;
	uint8_t                  ipi_dt;
	uint8_t                  ipi_dt_overwrite;
}ARM_MIPI_CSI2_IPI_ADV_INFO;

/**
\brief MIPI CSI2 IPI info
*/
typedef struct {
	CSI2_IPI_MODE_Type           ipi_mode;
	CSI2_IPI_COLOR_COP_Type      ipi_color_cop;
	CSI2_IPI_MEMFLUSH_Type       ipi_memflush;
	ARM_MIPI_CSI2_FRAME_INFO     *frame_info;
	ARM_MIPI_CSI2_IPI_ADV_INFO   *adv_features;
}ARM_MIPI_CSI2_IPI_INFO;

/**
\brief MIPI CSI2 driver info
*/
typedef struct {
	MIPI_CSI2_Type                   *reg_base;
	ARM_MIPI_CSI2_SignalEvent_t      cb_event;
	uint8_t                          csi_pixclk_div;
	CSI2_N_LANES_Type                n_lanes;
	CSI2_VC_ID_Type                  vc_id;
	CSI2_PIXEL_DATA_Type             pixel_data_type;
	ARM_MIPI_CSI2_IPI_INFO           *ipi_info;
	IRQn_Type                        irq;
	uint32_t                         irq_priority;
	uint32_t                         int_st;
	uint8_t                          flags;
}ARM_MIPI_CSI2_DRV_INFO;


#endif /* MIPI_CSI2_DEV_H_ */
