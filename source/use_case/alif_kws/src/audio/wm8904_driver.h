

/* project includes*/
#include <stdint.h>
#include "global_map.h"

// slave id for WM8904
#define WM8904_SLAVE        			   		   0x1A //7-bit address
#define WM8904_DEV_ID					   		   0x8904


/*
 * WM8904_Quick_startup_sequence_register
 */
 #define WM8904_SW_RESET_AND_ID                   0x00
 #define WM8904_WRITE_SEQUENCER_0                 0x6C
 #define WM8904_WRITE_SEQUENCER_3                 0x6F

/*
 * WM8904_Start-up sequence Register 1
 */
 #define WM8904_BIAS_CONTROL_0                    0x04
 #define WM8904_VMID_CONTROL_0                    0x05
 #define WM8904_VMID_CONTROL_0                    0x05
 #define WM8904_BIAS_CONTROL_0                    0x04
 #define WM8904_POWER_MANAGEMENT_2                0x0E
 #define WM8904_POWER_MANAGEMENT_3                0x0F
 #define WM8904_CLOCK_RATES_2                     0x16
 #define WM8904_POWER_MANAGEMENT_6                0x12

 /*
 * WM8904_Start-up sequence Register 2
 */
 #define WM8904_BIAS_CONTROL_0                    0x04
 #define WM8904_CHARGE_PUMP_0                     0x62

 /*
 * WM8904_Start-up sequence Register 3
 */
 #define WM8904_ANALOGUE_HP_0                     0x5A
 #define WM8904_ANALOGUE_LINEOUT_0                0x5E
 #define WM8904_ANALOGUE_HP_0                     0x5A
 #define WM8904_ANALOGUE_LINEOUT_0                0x5E
 #define WM8904_DC_SERVO_0                        0x43
 #define WM8904_DC_SERVO_1                        0x44

 /*
 * WM8904_Start-up sequence Register 4
 */
 #define WM8904_ANALOGUE_HP_0                     0x5A
 #define WM8904_ANALOGUE_LINEOUT_0                0x5E
 #define WM8904_ANALOGUE_HP_0                     0x5A
 #define WM8904_ANALOGUE_LINEOUT_0                0x5E

 /*
 * WM8904_control_register
 */
 #define WM8904_ANALOGUE_OUT12_ZC                 0x3D
 #define WM8904_ANALOGUE_LEFT_INPUT_0             0x2C
 #define WM8904_ANALOGUE_RIGHT_INPUT_0            0x2D
 #define WM8904_DAC_DIGITAL_1                     0x21
 #define WM8904_CLASS_W_0                         0x68
 #define WM8904_DIGITAL_PULLS                     0x7E
 #define WM8904_DAC_DIGITAL_VOLUME_LEFT           0x1E
 #define WM8904_DAC_DIGITAL_VOLUME_RIGHT          0x1F
 #define WM8904_DRC_0                             0x28
 #define WM8904_AUDIO_INTERFACE_0                 0x18
 #define WM8904_AUDIO_INTERFACE_1                 0x19
 
 // REGISTERS MASKING:
 
/*
 * R4 (0x04) - Bias Control 0
 */
#define BIAS_CNTL_ISEL_LP_BIAS                    0x0000
#define BIAS_CNTL_ISEL_HP_BIAS                    0x0008
#define BIAS_CNTL_BIAS_ENA                        0x0001

/*
 * R5 (0x05) - VMID Control 0
 */
#define VMID_CNTL0_VMID_BUF_ENA                   0x0040
#define VMID_CNTL0_VMID_RES_OFF                   0x0000
#define VMID_CNTL0_VMID_RES_NORMAL                0x0002
#define VMID_CNTL0_VMID_RES_LP                    0x0004
#define VMID_CNTL0_VMID_RES_FAST                  0x0006
#define VMID_CNTL0_VMID_ENA                       0x0001

/*
 * R14 (0x0E) - Power Management 2
 */
#define PWR_MGMT2_HPL_PGA_ENA                     0x0002
#define PWR_MGMT2_HPR_PGA_ENA                     0x0001

/*
 * R15 (0x0F) - Power Management 3
 */
#define PWR_MGMT3_LINEOUTL_PGA_ENA                0x0002
#define PWR_MGMT3_LINEOUTR_PGA_ENA                0x0001

/*
 * R22 (0x16) - Clock Rates 2
 */
#define CLK_RTE2_OPCLK_ENA                        0x0008
#define CLK_RTE2_CLK_SYS_ENA                      0x0004
#define CLK_RTE2_CLK_DSP_ENA                      0x0002
#define CLK_RTE2_TOCLK_ENA                        0x0001


/*
 * R18 (0x12) - Power Management 6
 */
#define PWR_MGMT6_DACL_ENA                        0x0008
#define PWR_MGMT6_DACR_ENA                        0x0004
#define PWR_MGMT6_ADCL_ENA                        0x0002
#define PWR_MGMT6_ADCR_ENA                        0x0001

/*
 * R98 (0x62) - Charge Pump 0
 */
#define CHRG_PMP_CP_ENA                           0x0001

/*
 * R90 (0x5A) - Analogue HP 0
 */
#define ANLG_HP0_HPL_RMV_SHORT                    0x0080
#define ANLG_HP0_HPL_ENA_OUTP                     0x0040
#define ANLG_HP0_HPL_ENA_DLY                      0x0020
#define ANLG_HP0_HPL_ENA                          0x0010
#define ANLG_HP0_HPR_RMV_SHORT                    0x0008
#define ANLG_HP0_HPR_ENA_OUTP                     0x0004
#define ANLG_HP0_HPR_ENA_DLY                      0x0002
#define ANLG_HP0_HPR_ENA                          0x0001

/*
 * R94 (0x5E) - Analogue Lineout 0
 */
#define ANLG_LINOUT0_LINEOUTL_RMV_SHORT           0x0080
#define ANLG_LINOUT0_LINEOUTL_ENA_OUTP            0x0040
#define ANLG_LINOUT0_LINEOUTL_ENA_DLY             0x0020
#define ANLG_LINOUT0_LINEOUTL_ENA                 0x0010
#define ANLG_LINOUT0_LINEOUTR_RMV_SHORT           0x0008
#define ANLG_LINOUT0_LINEOUTR_ENA_OUTP            0x0004
#define ANLG_LINOUT0_LINEOUTR_ENA_DLY             0x0002
#define ANLG_LINOUT0_LINEOUTR_ENA                 0x0001

/*
 * R67 (0x43) - DC Servo 0
 */
#define DC_SRV0_DCS_ENA_CHAN_3                    0x0008
#define DC_SRV0_DCS_ENA_CHAN_2                    0x0004
#define DC_SRV0_DCS_ENA_CHAN_1                    0x0002
#define DC_SRV0_DCS_ENA_CHAN_0                    0x0001

/*
 * R68 (0x44) - DC Servo 1
 */
#define DC_SRV1_DCS_TRIG_STARTUP_3                0x0080
#define DC_SRV1_DCS_TRIG_STARTUP_2                0x0040
#define DC_SRV1_DCS_TRIG_STARTUP_1                0x0020
#define DC_SRV1_DCS_TRIG_STARTUP_0                0x0010

/*
 * R108 (0x6C) - Write Sequencer 0
 */
#define WTE_SEQ0_WSEQ_ENA                         0x0100

/*
 * R111 (0x6F) - Write Sequencer 3
 */
#define WTE_SEQ3_WSEQ_START                       0x0100

/*
 * R61 (0x3D) - Analogue OUT12 ZC
 */
#define ANLG_OUT12ZC_HPL_BYP_ENA                  0x0008
#define ANLG_OUT12ZC_HPR_BYP_ENA                  0x0004

/*
 * R44 (0x2C) - Analogue Left Input 0
 */
#define ANLG_LIN0_LINMUTE                         0x0080

/*
 * R45 (0x2D) - Analogue Right Input 0
 */
#define ANLG_RIN0_RINMUTE                         0x0080

/*
 * R33 (0x21) - DAC Digital 1
 */
#define DAC_DG1_DEEMPH_NODEM                      0x0000
#define DAC_DG1_DEEMPH_32KHZ                      0x0002
#define DAC_DG1_DEEMPH_44KHZ                      0x0004
#define DAC_DG1_DEEMPH_48KHZ                      0x0006

/*
 * R104 (0x68) - Class W 0
 */
#define CLS_W0_CP_DYN_PWR                         0x0001

/*
 * R126 (0x7E) - Digital Pulls
 */
#define DG_PLS_LRCLK_PU                           0x0008
#define DG_PLS_BCLK_PU                            0x0002

/*
 * R30 (0x1E) - DAC Digital Volume Left
 */
#define DAC_DG_VL_DACL_VOL                        0x008A

/*
 * R31 (0x1F) - DAC Digital Volume Right
 */
#define DAC_DG_VR_DACR_VOL                        0x008A

// Bit to apply volume setting
#define DAC_DG_VU                                 0x0100

/*
 * R40 (0x28) - DRC 0
 */
#define DRC0_DRC_ENA                          	  0x8000
#define DRC0_DRC_DAC_PATH                     	  0x4000
#define DRC0_GS_HYSTLEVEL_MEDIUM                  0x0800
#define DRC0_DRC_STARTUP_GAIN_0                	  0x0180

/*
 * R25 (0x19) - Audio Interface 1
 */
#define AUD_INT1_AIF_WL_16BIT                     0x0000
#define AUD_INT1_AIF_WL_20BIT                     0x0004
#define AUD_INT1_AIF_WL_24BIT                     0x0008
#define AUD_INT1_AIF_WL_32BIT                     0x000C
#define AUD_INT1_AIF_FMT_RIGHT                    0x0000
#define AUD_INT1_AIF_FMT_LEFT                     0x0001
#define AUD_INT1_AIF_FMT_I2S                      0x0002
#define AUD_INT1_AIF_FMT_DSP                      0x0003

/*
 * R24 (0x18) - Audio Interface 0
 */
#define AUD_INT0_AIFADCL_SRC                      0x0080
#define AUD_INT0_AIFADCR_SRC                      0x0040
#define AUD_INT0_AIFDACL_SRC                      0x0020
#define AUD_INT0_AIFDACR_SRC                      0x0010

//Function Definitions//
int32_t WM8904_Codec_Init(void);

/* Set output volume, scale 0..100 */
void WM8904_Set_Volume(uint8_t volume);
