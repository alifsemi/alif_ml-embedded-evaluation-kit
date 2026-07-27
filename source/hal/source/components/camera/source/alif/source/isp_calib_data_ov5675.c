/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*******************************************************************************
 * @file     isp_calib_data_ov5675.c
 * @brief    ISP calibration data for the OV5675 sensor.
 *           This file is #included from isp_param.c — do not compile directly.
 ******************************************************************************/

ISP_CALIB_DATA_S calibration_data = {
    .modules = {
        .autoRoute = {
            .autoRoute = {100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200,
                    102400, 204800, 409600, 819200, 1638400, 3276800},
        },
#if (RTE_ISP_BLS_MODULE)
        .bls = {
            .enable = 1,
            .opType = OP_TYPE_MANUAL,
            .manualAttr = {
                .blackLevel = {64, 64, 64, 64},
            },
            .autoAttr = {
                .blackLevel  = {
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                    {64, 64, 64, 64},
                },
            },
        },
#endif /* RTE_ISP_BLS_MODULE */
#if (RTE_ISP_EXPM_MODULE)
        .aem = {
            .enable = 1,
            .expAltMode = 0,
            .blockWin = {
                .hOffs = 0,
                .vOffs = 0,
                .hSize = RTE_OV5675_CAMERA_SENSOR_FRAME_WIDTH,
                .vSize = RTE_OV5675_CAMERA_SENSOR_FRAME_HEIGHT,
            },
        },
#endif /* RTE_ISP_EXPM_MODULE */

#if (RTE_ISP_AE_MODULE)
        .ae = {
            .opType = OP_TYPE_AUTO,
            .manualAttr = {
                .intTime = 33000,
                .again = 8 * 1024,
                .dgain = 1024,
            },
            .autoAttr = {
                .expTimeRange = {
                    .min =  100,
                    .max =  16000,
                },
                .againRange = {
                    .min = 1 * 1024,
                    .max = 15 * 1024,
                },
                .dgainRange = {
                    .min = 1 * 1024,
                    .max = 1 * 1024,
                },
                .aeRunInterval = 6,
                .aeTarget = 100,
                .dampOver = 0x10,
                .dampUnder = 0x10,
                .tolerance = 30,
                .antiflicker = {
                    .enable = 0,
                    .flickerFreq = 100,
                },
                .aeMode = AE_MODE_FIX_FRAME_RATE,
                .gainThreshold = 15360,
                .aeRoute = {
                    .totalNum = 0,
                },
                .aeDelayAttr = {
                    .blackDelayFrame = 0,
                    .whiteDelayFrame = 0,
                },
                .weight = {
                    {1, 1, 1, 1, 1},
                    {1, 1, 1, 1, 1},
                    {1, 1, 1, 1, 1},
                    {1, 1, 1, 1, 1},
                    {1, 1, 1, 1, 1}
                },
            },
        },
#endif /* RTE_ISP_AE_MODULE */

#if (RTE_ISP_WBM_MODULE)
        .wbm = {
            .enable   = 0,
            .measMode = ISP_AWB_MEAS_MODE_RGB,
            .measRect = {
                .hOffs = 0,
                .vOffs = 0,
                .hSize = RTE_OV5675_CAMERA_SENSOR_FRAME_WIDTH,
                .vSize = RTE_OV5675_CAMERA_SENSOR_FRAME_HEIGHT,
            },
            .wpRange = {
                .maxY       = 0xEB,
                .refCr_MaxR = 0x80,
                .minY_MaxG  = 0xC0,
                .refCb_MaxB = 0x80,
                .maxCSum    = 0x14,
                .minC       = 0x14,
            },
        },
#endif /* RTE_ISP_WBM_MODULE */

#if (RTE_ISP_WB_MODULE)
        .wb = {
            .enable = 0,
            .opType = OP_TYPE_AUTO,
            .manualAttr = {
                .wbGain = {0x100, 0x100, 0x100, 0x100},
            },
            .autoAttr = {
                .runInterval = 12,
                .speed = 64,
                .tolerance = 1,
                .initColorTemp = 5000,
                .calibParam = {
                    .centLine = {-766376, -642393, -2921800},
                    .rgMin = 1086700,
                    .rgMax = 3050800,
                    .wpRange0 = {
                        .wpLCurve = {
                            .rg = { 790659, 953496, 1087350, 1277770,
                                1427190, 1521530, 1594220, 1791900,
                                1965800, 2113520, 2264770, 2450610,
                                2600660, 2750720, 2818650,
                                3050800 },
                            .dist = { 560682, 506246, 480332, 408497,
                                256789, 269600, 317996, 416285,
                                245149, 80289, -42413, -71649,
                                -4396, 111498, 229011, 453350 },
                        },
                        .wpRCurve = {
                            .rg = { 796080, 971478, 1096380, 1295500,
                                1423700, 1550280, 1700330, 1850390,
                                2000440, 2150500, 2300550, 2450610,
                                2630520, 2750720, 2900770,
                                3050800 },
                            .dist = { -48178, 37442, 108593, 173398,
                                161051, 170806, 219416, 259086,
                                288821, 304528, 301610, 271649,
                                266912,  88502,  -67514,
                                -253350 },
                        },
                    },
                    .wpRange1 = {
                        .wpLCurve = {
                            .rg = { 791651, 1289310, 1508700, 1631550,
                                1762950, 1861430, 1976370, 2091310,
                                2206250, 2321190, 2490840, 2629990,
                                2687900, 2780950, 2895890,
                                3010800 },
                            .dist = { -178231, 46924, 124591, 108020,
                                168323, 171752, 194948, 210209,
                                216012, 209423, 225067, 235329,
                                111501, -30197, -152124, -292180 },
                        },
                        .wpRCurve = {
                            .rg =  { 797383, 1275640, 1482030, 1601860,
                                1792600, 1869890, 1959780, 2076980,
                                2281190, 2332430, 2431000, 2574250,
                                2654400, 2765620, 2839520,
                                3010800 },
                            .dist = { 449646, 289261, 93411, 142743,
                                266621, 286629, 206521, 53599,
                                -134047, -144737, -124515, -148586,
                                -49479,  73539,  227056, 312180 },
                        },
                    },
                    .illuminant[ILLUMINANT_A] = {
                        .illuType = ILLUMINANT_A,
                        .colorTemp = 2856,
                        .wbGain = { 0x15e, 0x100, 0x100, 0x301 },
                    },
                    .illuminant[ILLUMINANT_TL84] = {
                        .illuType = ILLUMINANT_TL84,
                        .colorTemp = 4000,
                        .wbGain = { 0x1b5, 0x100, 0x100, 0x23D },
                    },
                    .illuminant[ILLUMINANT_CWF] = {
                        .illuType = ILLUMINANT_CWF,
                        .colorTemp = 4100,
                        .wbGain = { 0x216, 0x100, 0x100, 0x25c },
                    },
                    .illuminant[ILLUMINANT_D50] = {
                        .illuType = ILLUMINANT_D50,
                        .colorTemp = 5000,
                        .wbGain = { 0x245, 0x100, 0x100, 0x1AE },
                    },
                    .illuminant[ILLUMINANT_D65] = {
                        .illuType = ILLUMINANT_D65,
                        .colorTemp = 6500,
                        .wbGain = { 0x286, 0x100, 0x100, 0x15E },
                    },
                },
            },
        },
#endif /* RTE_ISP_WB_MODULE */

#if (RTE_ISP_DMSC_MODULE)
        .dmsc = {
            .enable    = 1,
            .threshold = 10,
            .cacAttr   = {
                .enable      = 0,
                .hClipMode   = 0,
                .vClipMode   = 0,
                .hStart      = 0,
                .vStart      = 0,
                .aBlue       = 0,
                .aRed        = 0,
                .bBlue       = 0,
                .bRed        = 0,
                .cBlue       = 0,
                .cRed        = 0,
                .xNormShift  = 7,
                .xNormFactor = 29,
                .yNormShift  = 7,
                .yNormFactor = 29,
            },
        },
#endif /* RTE_ISP_DMSC_MODULE */

#if (RTE_ISP_FLT_MODULE)
        .flt = {
            .enable = 1,
            .opType = OP_TYPE_MANUAL,
            .manualAttr = {
                .denoiseLevel = 0,
                .sharpenLevel = 0,
            },
            .autoAttr = {
                .denoiseLevel = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                .sharpenLevel = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            },
        },
#endif /* RTE_ISP_FLT_MODULE */

#if (RTE_ISP_CCM_MODULE)
        .ccm = {
            .opType = OP_TYPE_MANUAL,
            .manualAttr = {
                .colorMatrix = {
                    289, -70, -16,
                    -21, 183, -67,
                    -81, -8, 304
                },
                .rOffset = 0,
                .gOffset = 0,
                .bOffset = 0,
            },
            .autoAttr = {
                .illuminantCCM[ILLUMINANT_A] = {
                    .colorTemp   = 2856,
                    .colorMatrix = {192, -6, -57, -70, 230, -24, 11, -149, 276},
                    .rOffset = 0, .gOffset = 0, .bOffset = 0,
                },
                .illuminantCCM[ILLUMINANT_TL84] = {
                    .colorTemp   = 4000,
                    .colorMatrix = {222, -60, -32, -64, 236, -35, 1, -78, 222},
                    .rOffset = 0, .gOffset = 0, .bOffset = 0,
                },
                .illuminantCCM[ILLUMINANT_CWF] = {
                    .colorTemp   = 4100,
                    .colorMatrix = {257, -90, -37, -65, 219, -18, 2, -82, 223},
                    .rOffset = 0, .gOffset = 0, .bOffset = 0,
                },
                .illuminantCCM[ILLUMINANT_D50] = {
                    .colorTemp   = 5000,
                    .colorMatrix = {207, -55, -23, -48, 224, -34, 6, -89, 222},
                    .rOffset = 0, .gOffset = 0, .bOffset = 0,
                },
                .illuminantCCM[ILLUMINANT_D65] = {
                    .colorTemp   = 6500,
                    .colorMatrix = {229, -82, -17, -39, 215, -47, 0, -68, 207},
                    .rOffset = 0, .gOffset = 0, .bOffset = 0,
                },
            },
        },
#endif /* RTE_ISP_CCM_MODULE */

#if (RTE_ISP_GAMMAOUT_MODULE)
        .gammaOut = {
            .enable = 1,
            .gammaY = {
                0x000, 0x049, 0x089, 0x0B7, 0x0DF, 0x11F, 0x154, 0x183, 0x1AD,
                0x1F6, 0x235, 0x26F, 0x2D3, 0x32A, 0x378, 0x3BF, 0x3FF
            },
        },
#endif /* RTE_ISP_GAMMAOUT_MODULE */

#if (RTE_ISP_CSM_MODULE)
        .csm = {
            .type = ISP_CSM_TYPE_601,
            .quantization = ISP_CSM_LIM_RANGE,
        },
#endif /* RTE_ISP_CSM_MODULE */


    },
};
