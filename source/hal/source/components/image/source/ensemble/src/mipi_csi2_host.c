

#include "base_def.h"
#include "global_map.h"
#include "mipi_csi2_host.h"
#include "delay.h"
#include <stdio.h>
#include <stdint.h>

#define mipi_csi_base_addr 0x49033000

static void CSI_DPHY_REG_WRITE (int,uint8_t);
static uint8_t CSI_DPHY_REG_READ (int);

void csi2_IPI_initialization(void)
{


#ifdef rx_format_40x10
int HSA_TIME      = 0x5;
int HBP_TIME      = 0xA;
int HSD_TIME      = 0xA;
int HLINE_TIME    = 0x41;
int VSA_LINES     = 0x4;
int VBP_LINES     = 0x4;
int VFP_LINES     = 0x4;
int VACTIVE_LINES = 0xA;
#elif defined rx_format_640x480
int HSA_TIME      = 0x5;
int HBP_TIME      = 0xA;
//int HSD_TIME      = 0xA;
int HSD_TIME 	  = 0x32;
//int HLINE_TIME    = 0x339; //HSA+HBP+HSD+HACTIVE 640*1.25
int HLINE_TIME    = 0x361;
int VSA_LINES     = 0x4;
int VBP_LINES     = 0x4;
int VFP_LINES     = 0x4;
int VACTIVE_LINES = 0x1E0;
#elif defined rx_format_640x480_raw8 //OV5647
int HSA_TIME      = 0x5;
int HBP_TIME      = 0xA;
int HSD_TIME      = 0x32;
int HLINE_TIME    = 0x2C1; //HSA+HBP+HSD+HACTIVE 640
int VSA_LINES     = 0x4;
int VBP_LINES     = 0x4;
int VFP_LINES     = 0x4;
int VACTIVE_LINES = 0x1E0; //480
enum video_t VIDEO_FORMAT  = CSI_2_RAW8;
#elif defined rx_format_1920x1080_raw10 //OV5647
int HSA_TIME      = 0x5;
int HBP_TIME      = 0xA;
//int HSD_TIME      = 0x32;
int HSD_TIME 	  = 0x3D0;
int HLINE_TIME    = HSA_TIME+HBP_TIME+HSD_TIME+0x780; //HSA+HBP+HSD+HACTIVE 1920
int VSA_LINES     = 0x4;
int VBP_LINES     = 0x4;
int VFP_LINES     = 0x4;
int VACTIVE_LINES = VSA_LINES+VBP_LINES+VBP_LINES+0x438;
enum video_t VIDEO_FORMAT  = CSI_2_RAW10;
#elif defined rx_format_1920x1081_raw8 //OV5647 snps
int HSA_TIME      = 0x5;
int HBP_TIME      = 0xA;
//int HSD_TIME      = 0x32;
int HSD_TIME 	  = 0x3D0;
int HLINE_TIME    = HSA_TIME+HBP_TIME+HSD_TIME+0x780; //HSA+HBP+HSD+HACTIVE 1920
int VSA_LINES     = 0x4;
int VBP_LINES     = 0x4;
int VFP_LINES     = 0x4;
int VACTIVE_LINES = VSA_LINES+VBP_LINES+VBP_LINES+0x439;
enum video_t VIDEO_FORMAT  = CSI_2_RAW8;
#elif defined rx_format_560x560_raw10 //ARX340
int HSA_TIME      = 0x5;
int HBP_TIME      = 0xA;
int HSD_TIME 	  = 0x118; //560/2 //camera mode
//int HSD_TIME 	  = 0x32; //controller mode
int HLINE_TIME    = HSA_TIME+HBP_TIME+HSD_TIME+0x230; //HSA+HBP+HSD+HACTIVE 1920
int VSA_LINES     = 0x4;
int VBP_LINES     = 0x4;
int VFP_LINES     = 0x4;
int VACTIVE_LINES = 0x230;
enum video_t VIDEO_FORMAT  = CSI_2_RAW10;
//enum video_t VIDEO_FORMAT  = CSI_2_RAW8;
#elif defined rx_format_3448x1763
int HSA_TIME      = 0x5;
int HBP_TIME      = 0xA;
int HSD_TIME      = 0xA;
int HLINE_TIME    = 0xD5F; //HSA+HBP+HSD+HACTIVE
int VSA_LINES     = 0x4;
int VBP_LINES     = 0x4;
int VFP_LINES     = 0x4;
int VACTIVE_LINES = 0x6E3;
#endif

int rd_data;

//Release DWC_mipi_csi2_host from reset
//release reset CSI2 Host
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_CSI2_RESETN);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_CSI2_RESETN 0x%0x \n", rd_data);

//Set CSI2_RESETN, bit[0] to remove the CSI-2 internal logic from the reset state.
HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_CSI2_RESETN)=0x00000001;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_CSI2_RESETN);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_CSI2_RESETN 0x%0x \n", rd_data);

//Configure DWC_mipi_csi2_host

// -- Configure IPIs
// Data Type
// Mode[24-Enable | 8-Color Mode | 0-Mode]
// Embedded Enable
// Virtual Channel
//ipi_idx          = 0;
//tb_data_type     = ipi_data_type[DT_DataWidth*(ipi_idx+1)-1 -: DT_DataWidth];
//MODE             = {7'd0,ipi_enable[ipi_idx], 8'd0, 7'd0, ipi_color_mode[ipi_idx], 7'd0, ipi_op_mode[ipi_idx]};
//EMBEDDED_EN      = {ipi_embedded[ipi_idx], 2'd0};
//VCID             = ipi_vcid[DataWidth*(ipi_idx+1)-1 -: DataWidth];
//
//apbWrite(`CSI2_HOST_IPI_ADV_FEATURES_OS , {18'd0,tb_adv_features});
//apbWrite(`CSI2_HOST_IPI_MODE_OS         , MODE);
//apbWrite(`CSI2_HOST_IPI_VCID_OS         , VCID);
//apbWrite(`CSI2_HOST_IPI_DATA_TYPE_OS    , {23'h00_0000,EMBEDDED_EN,tb_data_type});
//apbWrite(`CSI2_HOST_IPI_HSA_TIME_OS     ,HSA_TIME     );
//apbWrite(`CSI2_HOST_IPI_HBP_TIME_OS     ,HBP_TIME     );
//apbWrite(`CSI2_HOST_IPI_HSD_TIME_OS     ,HSD_TIME     );
//apbWrite(`CSI2_HOST_IPI_HLINE_TIME_OS   ,HLINE_TIME   );
//apbWrite(`CSI2_HOST_IPI_VSA_LINES_OS    ,VSA_LINES    );
//apbWrite(`CSI2_HOST_IPI_VBP_LINES_OS    ,VBP_LINES    );
//apbWrite(`CSI2_HOST_IPI_VFP_LINES_OS    ,VFP_LINES    );
//apbWrite(`CSI2_HOST_IPI_VACTIVE_LINES_OS,VACTIVE_LINES);

//Configure the IPI horizontal frame information
//1. Using IPI_HSA_TIME register, configure Horizontal Synchronism Active period in pixclk cycles.
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HSA_TIME);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_HSA_TIME 0x%0x \n", rd_data);

HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HSA_TIME)=HSA_TIME;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HSA_TIME);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_HSA_TIME 0x%0x \n", rd_data);

//2. Using IPI_HBP_TIME register, configure Horizontal Back Porch period in pixclk cycles.
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HBP_TIME);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_HBP_TIME 0x%0x \n", rd_data);

HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HBP_TIME)=HBP_TIME;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HBP_TIME);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_HBP_TIME 0x%0x \n", rd_data);

//3. Using IPI_HSD_TIME register, configure Horizontal Sync Porch delay period in pixclk cycles.
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HSD_TIME);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_HSD_TIME 0x%0x \n", rd_data);

//HFP
HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HSD_TIME)=HSD_TIME;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HSD_TIME);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_HSD_TIME 0x%0x \n", rd_data);

//4. Using IPI_HLINE_TIME register, configure the size of the line time counted in pixclk cycles.
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HLINE_TIME);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_HLINE_TIME 0x%0x \n", rd_data);

HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HLINE_TIME)=HLINE_TIME;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_HLINE_TIME);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_HLINE_TIME 0x%0x \n", rd_data);

//Configure the IPI vertical frame information
//1. Using IPI_VSA_LINES register, configure the Vertical Synchronism Active period measured in number of horizontal lines.
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VSA_LINES);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_VSA_LINES 0x%0x \n", rd_data);

HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VSA_LINES)=VSA_LINES;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VSA_LINES);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_VSA_LINES 0x%0x \n", rd_data);

//2. Using IPI_VBP_LINES register, configure the Vertical Back Porch period measured in number of horizontal lines.
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VBP_LINES);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_VBP_LINES 0x%0x \n", rd_data);

HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VBP_LINES)=VBP_LINES;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VBP_LINES);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_VBP_LINES 0x%0x \n", rd_data);

//3. Using IPI_VFP_LINES register, configure the Vertical Front Porch period measured in number of horizontal lines.
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VFP_LINES);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_VFP_LINES 0x%0x \n", rd_data);

HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VFP_LINES)=VFP_LINES;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VFP_LINES);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_VFP_LINES 0x%0x \n", rd_data);

//4. Using IPI_VACTIVE_LINES register, configure the Vertical Active period measured in number of horizontal lines
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VACTIVE_LINES);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_VACTIVE_LINES 0x%0x \n", rd_data);

//20 lines
HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VACTIVE_LINES)=VACTIVE_LINES;
//640 lines
//HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VACTIVE_LINES)=0x000001E0;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VACTIVE_LINES);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_VACTIVE_LINES 0x%0x \n", rd_data);

//Select the VCID
//1:0 ip_vcid R/W Virtual channel of data to be processed by pixel interface.
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VCID);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_VCID 0x%0x \n", rd_data);

HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VCID)=rd_data|0x0;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_VCID);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_VCID 0x%0x \n", rd_data);

//IPI data type
//1:0 ip_vcid R/W Virtual channel of data to be processed by pixel interface.
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_DATA_TYPE);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_DATA_TYPE 0x%0x \n", rd_data);

HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_DATA_TYPE)=rd_data|VIDEO_FORMAT;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_DATA_TYPE);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_DATA_TYPE 0x%0x \n", rd_data);
/*
//IPI_ADV_FEATURES
//This register configures advanced features for IPI mode.
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_ADV_FEATURES);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_ADV_FEATURES 0x%0x \n", rd_data);

HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_ADV_FEATURES)=0x01050000;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_ADV_FEATURES);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_ADV_FEATURES 0x%0x \n", rd_data);
*/
//Select the IPI mode
//24 ipi_enable R/W This register enables the interface
//8  ipi_color_com R/W This field indicates if color mode components are delivered
//   as follows: 0x0 (COLOR48): 48 bits, 0x1 (COLOR16): 16 bits
//0  ipi_mode R/W This field indicates the video mode transmission type as
//   follows: Values: ?? 0x0 (CAMMODE): Camera timing
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_MODE);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_MODE 0x%0x \n", rd_data);

// IPI 16-bit mode
//camera mode
HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_MODE)=rd_data|0x1<<8|0x0;
//controller mode
//HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_MODE)=rd_data|0x1<<24|0x1<<8|0x1;
// IPI 48-bit mode
//HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_MODE)=rd_data|0x1<<24|0x0<<8|0x0;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_MODE);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_MODE 0x%0x \n", rd_data);


  //CSI-2 Host check
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_MAIN);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_MAIN 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PHY_FATAL);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PHY_FATAL 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PKT_FATAL);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PKT_FATAL 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PHY);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PHY 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_IPI_FATAL);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_IPI_FATAL 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_BNDRY_FRAME_FATAL);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_BNDRY_FRAME_FATAL 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_SEQ_FRAME_FATAL);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_SEQ_FRAME_FATAL 0x%0x \n", rd_data);

//soft reset
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_SOFTRSTN);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_SOFTRSTN 0x%0x \n", rd_data);

HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_SOFTRSTN)=0x0;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_SOFTRSTN);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_SOFTRSTN 0x%0x \n", rd_data);

HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_SOFTRSTN)=0x1;

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_SOFTRSTN);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_SOFTRSTN 0x%0x \n", rd_data);


//CSI-2 Host check
rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_MAIN);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_MAIN 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PHY_FATAL);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PHY_FATAL 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PKT_FATAL);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PKT_FATAL 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PHY);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PHY 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_IPI_FATAL);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_IPI_FATAL 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_BNDRY_FRAME_FATAL);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_BNDRY_FRAME_FATAL 0x%0x \n", rd_data);

rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_SEQ_FRAME_FATAL);
DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_SEQ_FRAME_FATAL 0x%0x \n", rd_data);


}

void csi2_IPI_enable(void)
{
	HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_MODE) |= (1U << 24);

}

void csi2_status(void)
{
	int rd_data;
	  //CSI-2 Host check
	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_MAIN);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_MAIN 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PHY_FATAL);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PHY_FATAL 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PKT_FATAL);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PKT_FATAL 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PHY);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PHY 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_IPI_FATAL);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_IPI_FATAL 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_BNDRY_FRAME_FATAL);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_BNDRY_FRAME_FATAL 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_SEQ_FRAME_FATAL);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_SEQ_FRAME_FATAL 0x%0x \n", rd_data);

	//soft reset
	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_SOFTRSTN);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_SOFTRSTN 0x%0x \n", rd_data);

	HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_SOFTRSTN)=0x0;

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_SOFTRSTN);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_SOFTRSTN 0x%0x \n", rd_data);

	HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_SOFTRSTN)=0x1;

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_IPI_SOFTRSTN);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_IPI_SOFTRSTN 0x%0x \n", rd_data);

	//CSI-2 Host check
	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_MAIN);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_MAIN 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PHY_FATAL);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PHY_FATAL 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PKT_FATAL);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PKT_FATAL 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_PHY);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_PHY 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_IPI_FATAL);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_IPI_FATAL 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_BNDRY_FRAME_FATAL);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_BNDRY_FRAME_FATAL 0x%0x \n", rd_data);

	rd_data = HW_REG_WORD((mipi_csi_base_addr),CSI2_HOST_INT_ST_SEQ_FRAME_FATAL);
	DEBUG_PRINTF("MIPI CSI Host CSI2_HOST_INT_ST_SEQ_FRAME_FATAL 0x%0x \n", rd_data);
}


int rx_phyconfig(void)
{
	int rd_data =0,exp_data =0,err=0;
	uint8_t r_data_8;

#ifdef RX_FREQ_50M
  int hsfreqrange = 0x30;
  int pll_soc_m_7_0 = 0x4d;
  int pll_soc_m_9_8 = 0x1;
  int pll_soc_n = 0x2;
  uint8_t vco_cntrl = 0x28; //101000
  int below_450Mbps = 1;
  int pixel_clk_div = 0x4;
#elif defined(RX_FREQ_100M)
  //int hsfreqrange = 0x14;
  //int pll_soc_m_7_0 = 0x77;
  //int pll_soc_m_9_8 = 0x1;
  //int pll_soc_n = 0x3;
  ////uint8_t vco_cntrl = 0x1B; //011011
  //uint8_t vco_cntrl = 0x1F; //011111
  //int below_450Mbps = 1;
  int hsfreqrange = 0x14;
  int pll_soc_m_7_0 = 0xFA;
  int pll_soc_m_9_8 = 0x0;
  int pll_soc_n = 0x2;
  uint8_t vco_cntrl = 0x1B; //011011
  int below_450Mbps = 1;
  int pixel_clk_div = 0x4;
#elif defined(RX_FREQ_125M)
  int hsfreqrange = 0x33;
  int pll_soc_m_7_0 = 0x71;
  int pll_soc_m_9_8 = 0x2;
  int pll_soc_n = 0x3;
  uint8_t vco_cntrl = 0x1F; //101000
  int below_450Mbps = 1;
  int pixel_clk_div = 0x4; //camera mode 200Mhz
#elif defined(RX_FREQ_150M)
  //int hsfreqrange = 0x14;
  //int pll_soc_m_7_0 = 0x77;
  //int pll_soc_m_9_8 = 0x1;
  //int pll_soc_n = 0x3;
  ////uint8_t vco_cntrl = 0x1B; //011011
  //uint8_t vco_cntrl = 0x1F; //011111
  //int below_450Mbps = 1;
  int hsfreqrange = 0x14;
  int pll_soc_m_7_0 = 0x77;
  int pll_soc_m_9_8 = 0x1;
  int pll_soc_n = 0x2;
  uint8_t vco_cntrl = 0x1B; //011011
  int below_450Mbps = 1;
  int pixel_clk_div = 0x8;
  //int pixel_clk_div = 0x10;
#elif defined(RX_FREQ_200M)
  int hsfreqrange = 0x5;
  int pll_soc_m_7_0 = 0xF4;
  int pll_soc_m_9_8 = 0x1;
  int pll_soc_n = 0x2;
  uint8_t vco_cntrl = 0x18; //0110000
  int below_450Mbps = 0;
  int pixel_clk_div = 0x4;
#elif defined(RX_FREQ_250M)
  int hsfreqrange = 0x26;
  int pll_soc_m_7_0 = 0x38;
  int pll_soc_m_9_8 = 0x1;
  int pll_soc_n = 0x2;
  uint8_t vco_cntrl = 0x17; //010111
  int below_450Mbps = 1;
  int pixel_clk_div = 0x4; //68Mhz
#elif defined(RX_FREQ_371M)
  int hsfreqrange = 0x39;
  int pll_soc_m_7_0 = 0xCF;
  int pll_soc_m_9_8 = 0x1;
  int pll_soc_n = 0x2;
  uint8_t vco_cntrl = 0x10; //010000
  int below_450Mbps = 1;
  int pixel_clk_div = 0x4;
#elif defined(RX_FREQ_400M)
  int hsfreqrange = 0x9;
  int pll_soc_m_7_0 = 0xF4;
  int pll_soc_m_9_8 = 0x1;
  int pll_soc_n = 0x2;
  uint8_t vco_cntrl = 0x10; //010000
  int below_450Mbps = 0;
  #ifdef controller_mode
    int pixel_clk_div = 0x4; //controller mode 100Mhz
  #else
    int pixel_clk_div = 0x2; //camera mode 200Mhz
  #endif
  //int pixel_clk_div = 0x3; //controller mode
#else //1G
  int hsfreqrange = 0xA;
  int pll_soc_m_7_0 = 0x38;
  int pll_soc_m_9_8 = 0x1;
  int pll_soc_n = 0x2;
  uint8_t vco_cntrl = 0xF; //001111
  int below_450Mbps = 0;
  int pixel_clk_div = 0x4;
  //int hsfreqrange = 0xA;
  //int pll_soc_m_7_0 = 0xA1;
  //int pll_soc_m_9_8 = 0x1;
  //int pll_soc_n = 0x2;
  //uint8_t vco_cntrl = 0xF; //001111
  //int below_450Mbps = 0;
#endif

	  ////////////////////////////////////////////////////////////////
	  //CSI DPHY setup as slave
	  ////////////////////////////////////////////////////////////////

	  //MIPI csi pixclk cntrl
	  //EXPSLV1 Pixclk Control Register
	  //pixel clock divisor
	  //HW_REG_WORD(CFGSLV1_BASE,0x08)=0x00040001;
	  HW_REG_WORD(CFGSLV1_BASE,0x08)=(0x00000001|pixel_clk_div<<16);

	  //number of lines
	  HW_REG_WORD((mipi_csi_base_addr),0x04)=0x00000001;

	  //1. dphy_rstz -active low
	  HW_REG_WORD((mipi_csi_base_addr),0x44)=0x00000000;
	  //2. phy_shutdownz - active low
	  HW_REG_WORD((mipi_csi_base_addr),0x40)=0x00000000;

	  //testport_sel [4]	RW	0x0	testport select; 0: select tx_testport; 1: select rx_testport	( rx_dphy_ctrl0.testport_sel = 0x0 )
	  //txrxz [8]	RW	0x0	control tx_rxz pin of DPHY	( rx_dphy_ctrl0.txrxz = 0x0 )
	  //Master/Slave Configuration Signal.
	  //This signal controls whether the PHY functionalities are configured
	  //for a Master-side D-PHY implementation (tx_rxz=1) or for a Slaveside
	  //D- PHY implementation (tx_rxz=0).
	  //Note: This signal configures all the PHY lanes (data and clock).
	  //Synchronous To: Asynchronous
	  //Active State: High
	  //3. set tx_rxz = 1'b0
	  HW_REG_WORD((CFGSLV1_BASE),0x38)=0x00000000;

	  // testclr = 1
	  // 0 phy_testclr R/W When active, performs vendor specific interface initialization.
	  // Active High.
	  // Note: This line needs an initial high pulse after power up for
	  // analog programmability default values to be preset.
	  // Value After Reset: 0x1
	  // Exists: Always
	  // wire                csi_phy_tx_testclr  = reg_csi_testport_sel ? 1'b0 : mipi_csi_ctrl_testclr;
	  // wire                csi_phy_tx_testclk  = reg_csi_testport_sel ? 1'b0 : mipi_csi_ctrl_testclk;
	  // wire                csi_phy_tx_testen   = reg_csi_testport_sel ? 1'b0 : mipi_csi_ctrl_testen;
	  // wire [7:0]          csi_phy_tx_testdin  = reg_csi_testport_sel ? 8'h0 : mipi_csi_ctrl_testdin;

	  // wire                csi_phy_rx_testclr  = reg_csi_testport_sel ? mipi_csi_ctrl_testclr : 1'b0;
	  // wire                csi_phy_rx_testclk  = reg_csi_testport_sel ? mipi_csi_ctrl_testclk : 1'b0;
	  // wire                csi_phy_rx_testen   = reg_csi_testport_sel ? mipi_csi_ctrl_testen  : 1'b0;
	  // wire [7:0]          csi_phy_rx_testdin  = reg_csi_testport_sel ? mipi_csi_ctrl_testdin : 8'h0;
	  // 4.1 set tx_testclr = 1
	  HW_REG_WORD((mipi_csi_base_addr),0x50)=0x00000000;
	  HW_REG_WORD((mipi_csi_base_addr),0x50)=0x00000001;
	  HW_REG_WORD((mipi_csi_base_addr),0x50)=0x00000000;

	  //bist_on [0]	RW	0x0	Bist ON	( rx_dphy_ctrl0.bist_on = 0x0 )
	  //bist_done [1]	RO	0x0	Bist Done	( rx_dphy_ctrl0.bist_done = 0x0 )
	  //bist_ok [2]	RO	0x0	Bist OK	( rx_dphy_ctrl0.bist_ok = 0x0 )
	  //cont_en [3]	RW	0x0	Cont enable	( rx_dphy_ctrl0.cont_en = 0x0 )
	  //testport_sel [4]	RW	0x0	testport select; 0: select tx_testport; 1: select rx_testport	( rx_dphy_ctrl0.testport_sel = 0x0 )
	  //txrxz [8]	RW	0x0	control tx_rxz pin of DPHY	( rx_dphy_ctrl0.txrxz = 0x0 )
	  //basedir [13:12]	RW	0x0	Base dir	( rx_dphy_ctrl0.basedir = 0x0 )
	  //hsfreqrange [22:16]	RW	0x0	IP operating frequency	( rx_dphy_ctrl0.hsfreqrange = 0x0 )
	  //cfgclkfreqrange [31:24]	RW	0x0	Input reference clock frequency	( rx_dphy_ctrl0.cfgclkfreqrange = 0x0 )
	  //Configures the Base Direction for Lane N.
	  //basedir_N = 1: Configures lane as RX upon startup of the PHY
	  //basedir_N = 0: Configures lane as TX
	  HW_REG_WORD((CFGSLV1_BASE),0x38)=0x00000010;

	  //4.2 set rx_testclr = 1
	  //just toggle rx_testclr
	  HW_REG_WORD((mipi_csi_base_addr),0x50)=0x00000000;
	  HW_REG_WORD((mipi_csi_base_addr),0x50)=0x00000001;
	  HW_REG_WORD((mipi_csi_base_addr),0x50)=0x00000000;

	  //5. Wait for 15 ns;
	  //for(int i=0;i<5;i++);

	  //6.1 testclr = 0 for tx
	  //HW_REG_WORD((CFGSLV1_BASE),0x38)=0x00000100;
	  //HW_REG_WORD((mipi_csi_base_addr),0x50)=0x00000000;

	  //6.2 test clr = 0 for rx
	  //HW_REG_WORD((CFGSLV1_BASE),0x38)=0x00003010;
	  //HW_REG_WORD((mipi_csi_base_addr),0x50)=0x00000000;

	  //[22:16] - 1G = hsfreqrange[6:0] = 0x0A
	  //[31:24] - cfgclkfreqrange[7:0] = round[(Fcfg_clk(MHz)-17)*4] = (25-17)*4 = 8'b001000000
	  //cfgclk = tb_top.DUT.u_sse700.u_sse700_f0_expslv1.mipi_rx_dphy_cfg_clk = 25MHz

	  //7.1 TX
	  rd_data = HW_REG_WORD((CFGSLV1_BASE),0x38);
	  DEBUG_PRINTF("EXPSLV1 0x38 0x%0x \n", rd_data);
	  HW_REG_WORD((CFGSLV1_BASE),0x38)=0x20000100|(hsfreqrange<<16);
	  rd_data = HW_REG_WORD((CFGSLV1_BASE),0x38);
	  DEBUG_PRINTF("MIPI CSI Hosthsfreqrange 0x%0x \n", rd_data);

	  DEBUG_PRINTF("MIPI CSI DPHY-Write task started....\n");

	  //dphy4rxtester_DIG_RD_RX_SYS_1
	  //6:0 hsfreqrange__6__0__ R Description: HS frequency range selected (volatile)
	  r_data_8 = CSI_DPHY_REG_READ(0x1F);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x1F 0x%0x \n", r_data_8);

	  //8. Configure TX register 0x16A to set pll_mpll_prog_rw (bits1:0) to 2'b11;
	  //dphy4txtester_DIG_RDWR_TX_PLL_13
	  r_data_8 = CSI_DPHY_REG_READ(0x16A);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x16A 0x%0x \n", r_data_8);

	  CSI_DPHY_REG_WRITE(0x16A,0x03);

	  r_data_8 = CSI_DPHY_REG_READ(0x16A);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x16A 0x%0x \n", r_data_8);

	  //9. Configure TX register 0x1AB to set cb_sel_vref_lprx_rw (bits 1:0) to 2'b10;
	  r_data_8 = CSI_DPHY_REG_READ(0x1AB);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x1AB 0x%0x \n", r_data_8);

	  CSI_DPHY_REG_WRITE(0x1AB,0x06);

	  r_data_8 = CSI_DPHY_REG_READ(0x1AB);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x1AB 0x%0x \n", r_data_8);

	  //10. Configure TX register 0x1AA to set cb_sel_vrefcd_lprx_rw (bits 6:5) to 2'b10;
	  //dphy4txtester_DIG_RDWR_TX_CB_0
	  //4:2 cb_sel_v400_prog_rw__2__0__ R/W Description: Select value of cb_v400
	  r_data_8 = CSI_DPHY_REG_READ(0x1AA);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x1AA 0x%0x \n", r_data_8);

	  CSI_DPHY_REG_WRITE(0x1AA,0x4F);

	  r_data_8 = CSI_DPHY_REG_READ(0x1AA);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x1AA 0x%0x \n", r_data_8);

	  //switch to RX
	  HW_REG_WORD((CFGSLV1_BASE),0x38)=0x20003010|(hsfreqrange<<16);
	  rd_data = HW_REG_WORD((CFGSLV1_BASE),0x38);
	  DEBUG_PRINTF("EXPSLV1 0x38 0x%0x \n", rd_data);

	  //11. Configure RX register 0x307 to set rxclk_rxhs_pull_long_channel_if_rw signal (bit 7) to 1'b1;
	  r_data_8 = CSI_DPHY_REG_READ(0x307);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x307 0x%0x \n", r_data_8);

	  CSI_DPHY_REG_WRITE(0x307,0x80);

	  r_data_8 = CSI_DPHY_REG_READ(0x307);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x307 0x%0x \n", r_data_8);

	  //dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_17
	  r_data_8 = CSI_DPHY_REG_READ(0xf1);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0xf1 0x%0x \n", r_data_8);

	  CSI_DPHY_REG_WRITE(0xf1,0x85);

	  r_data_8 = CSI_DPHY_REG_READ(0xf1);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0xf1 0x%0x \n", r_data_8);

	  //12.
	  r_data_8 = CSI_DPHY_REG_READ(0xe2);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0xe2 0x%0x \n", r_data_8);

	  CSI_DPHY_REG_WRITE(0xe2,0xe9);//osc freq 489(1gbps) , hence e9 for 0 to 7 bits

	  r_data_8 = CSI_DPHY_REG_READ(0xe2);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0xe2 0x%0x \n", r_data_8);

	  //12.
	  r_data_8 = CSI_DPHY_REG_READ(0xe3);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0xe3 0x%0x \n", r_data_8);

	  //12.
	  CSI_DPHY_REG_WRITE(0xe3,0x01);//osc freq 489(1gbps) , hence 1 for 8 to 11 bits

	  r_data_8 = CSI_DPHY_REG_READ(0xe3);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0xe3 0x%0x \n", r_data_8);

	  //12.
	  r_data_8 = CSI_DPHY_REG_READ(0xe4);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0xe4 0x%0x \n", r_data_8);

	  CSI_DPHY_REG_WRITE(0xe4,0x01);//override en 1

	  r_data_8 = CSI_DPHY_REG_READ(0xe4);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0xe4 0x%0x \n", r_data_8);

	  //16. forcerxmode_n = 1'b1 //for both lane 0, 1
	  HW_REG_WORD((CFGSLV1_BASE),0x3C)=0x00000003;

	  //17. Set all requests inputs to zero;
	  //18. Wait for 15 ns;
	  for(int i=0;i<5;i++);

	  //19. Set enable_n and enableclk=1'b1;
	  //These signals already tied to high in design

	  //20. Wait 5 ns;
	  for(int i=0;i<5;i++);

	  //21. Set shutdownz=1'b1;
	  HW_REG_WORD((mipi_csi_base_addr),0x40)=0x00000001;

	  //22. Wait 5ns;
	  for(int i=0;i<5;i++);

	  //23. Set rstz=1'b1;
	  HW_REG_WORD((mipi_csi_base_addr),0x44)=0x00000001;

	  static volatile int retry = 100;
	  //24. Wait until stopstatedata_n and stopstateclk outputs are asserted indicating PHY is receiving LP11 in
	  //enabled datalanes and clocklanes.
	  do {
	    rd_data = HW_REG_WORD((mipi_csi_base_addr),0x4C);
	    DEBUG_PRINTF("MIPI CSI DPHY Read 0x%0x \n", rd_data);

	    sleep_or_wait_msec(1);
	    if (--retry <= 0)
	  	  return -1;

	  } while( (rd_data & 0x00000003) != 0x00000003);

	  //25. Set forcerxmode_n = 1'b0;
	  HW_REG_WORD((CFGSLV1_BASE),0x3C)=0x00000000;

	  DEBUG_PRINTF("MIPI CSI DPHY setup completed....\n");

	  //0x39d: dphy2rxtester_DIG_RD_RX_CLKLANE_OFFSET_CAL_0
	  //Read bits [4:1] � rxclk_errcal[3:0]
	  //Read bits [0] � rxclk_caldone
	  r_data_8 = CSI_DPHY_REG_READ(0x39d);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x39d 0x%0x \n", r_data_8);

	  //Calibration value:
	  //0x39c: dphy4rxtester_DIG_RD_RX_CLKLANE_OFFSET_CAL_3
	  //Read bits [6:0] - rxclk_rxhs_clk_offset_cal[6:0]]
	  r_data_8 = CSI_DPHY_REG_READ(0x39c);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x39c 0x%0x \n", r_data_8);

	  //datalane0:
	  //0x59f: dphy2rxtester_DIG_RD_RX_LANE0_OFFSET_CAL_0
	  //Read bits [2] - rx0_cal_error
	  //Read bits [1] - rx0_cal_done
	  r_data_8 = CSI_DPHY_REG_READ(0x59f);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x59f 0x%0x \n", r_data_8);

	  //datalane1:
	  //0x79f: dphy2rxtester_DIG_RD_RX_LANE1_OFFSET_CAL_0
	  //Read bits [2] � rx1_cal_error
	  //Read bits [1] � rx1_cal_done
	  r_data_8 = CSI_DPHY_REG_READ(0x79f);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x79f 0x%0x \n", r_data_8);

	  //datalane0:
	  //0x5a1: dphy2rxtester_DIG_RD_RX_LANE0_OFFSET_CAL_2
	  //Read bits [6:0] - rx0_rxhs_data_offset_cal[6:0]
	  r_data_8 = CSI_DPHY_REG_READ(0x5a1);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x5a1 0x%0x \n", r_data_8);

	  //datalane1:
	  //0x7a1: dphy2rxtester_DIG_RD_RX_LANE1_OFFSET_CAL_2
	  //Read bits [6:0] � rx1_rxhs_data_offset_cal[6:0]
	  r_data_8 = CSI_DPHY_REG_READ(0x7a1);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x7a1 0x%0x \n", r_data_8);

	  //datalane0:
	  //0x5e0: dphy2rxtester_DIG_RD_RX_LANE0_DDL_0
	  //Read bits[2] � rx0_ddl_finished_if
	  //Read bits[1] � rx0_ddl_error_flag_if
	  r_data_8 = CSI_DPHY_REG_READ(0x5e0);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x5e0 0x%0x \n", r_data_8);

	  //datalane1:
	  //0x7e0: dphy2rxtester_DIG_RD_dphy4rxtester_DIG_RD_RX_LANE1_DDL_0
	  //Read bits[2] � rx1_ddl_finished_if
	  //Read bits[1] � rx1_ddl_error_flag_if
	  r_data_8 = CSI_DPHY_REG_READ(0x7e0);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x7e0 0x%0x \n", r_data_8);

	  //datalane0:
	  //0x5e5: dphy2rxtester_DIG_RD_RX_LANE0_DDL_5
	  //Read bits[4:0] � rx0_rxhs_ddl_tune_if[4:0]
	  r_data_8 = CSI_DPHY_REG_READ(0x5e5);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x5e5 0x%0x \n", r_data_8);

	  //datalane1:
	  //0x7e5: dphy2rxtester_DIG_RD_RX_LANE1_DDL_5
	  //Read bits[4:0] � rx1_rxhs_ddl_tune_if[4:0]
	  r_data_8 = CSI_DPHY_REG_READ(0x7e5);
	  DEBUG_PRINTF("MIPI CSI DPHY Read 0x7e5 0x%0x \n", r_data_8);

	  return 0;
}

static uint8_t CSI_DPHY_REG_READ(int start_add)
{

  //50
  //[0] phy_testclk R/W Clock to capture testdin bus contents into the macro, with
  //    testen signal controlling the operation selection.
  //[1] phy_testclr R/W When active, performs vendor specific interface initialization.
  //    Active High.
  //
  //54
  //[16] phy_testen R/W When asserted high, it configures an address write operation
  //     on the falling edge of testclk. When asserted low, it
  //     configures a data write operation on the rising edge of
  //     testclk
  //[15:8] phy_testdout R Vendor-specific 8-bit data output for reading data and other
  //      probing functionalities
  //[7:0] phy_testdin R/W Test interface 8-bit data input for programming internal
  //      registers and accessing test functionalities.

  int lsb_addr,read_data;
  uint8_t r_data_8;

  //1.
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000000;//a. testclk
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00000000;//a. testen
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00010000;//b. testen_high
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000002;//c. testclk high
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00010000;//d. place testdin 0x00
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000000;//e. testclk low
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00000000;//f. place testen low
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = ((start_add >> 8));//g. MSB testdin
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000002;//h. set testclk high

  //2.
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000000;//a. testclk low
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00010000;//b. testen_high
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000002;//c. set testclk high
  lsb_addr = ((start_add & 0x00ff) | 0x00010000);
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = lsb_addr; //d. LSB testdin
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000000;//e. set testclk low

  read_data = HW_REG_WORD(mipi_csi_base_addr,0x0054);

  r_data_8 = read_data >> 8;

  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00000000;//f. testen_low

  return r_data_8;
}

static void CSI_DPHY_REG_WRITE(int start_add,uint8_t write_data)
{

  //50
  //[1] phy_testclk R/W Clock to capture testdin bus contents into the macro, with
  //    testen signal controlling the operation selection.
  //[0] phy_testclr R/W When active, performs vendor specific interface initialization.
  //    Active High.
  //
  //54
  //[16] phy_testen R/W When asserted high, it configures an address write operation
  //     on the falling edge of testclk. When asserted low, it
  //     configures a data write operation on the rising edge of
  //     testclk
  //[15:8] phy_testdout R Vendor-specific 8-bit data output for reading data and other
  //      probing functionalities
  //[7:0] phy_testdin R/W Test interface 8-bit data input for programming internal
  //      registers and accessing test functionalities.

  int lsb_addr;

  //1.
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000000;//a. testclk
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00000000;//a. testen
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00010000;//b. testen_high
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000002;//c. testclk high
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00010000;//d. place testdin 0x00
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000000;//e. testclk low
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00000000;//f. place testen low
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = ((start_add >> 8));//g. MSB testdin
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000002;//h. set testclk high

  //2.
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000000;//a. testclk low
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00010000;//b. testen_high
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000002;//c. set testclk high
  lsb_addr = ((start_add & 0x00ff) | 0x00010000);
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = lsb_addr; //d. LSB testdin
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000000;//e. set testclk low
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00000000;//b. testen_low

  //3.
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = write_data; //a.
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000002; //b. testclk high

  //turn off for clean exit
  HW_REG_WORD(mipi_csi_base_addr,0x0050) = 0x00000000;//a. testclk
  HW_REG_WORD(mipi_csi_base_addr,0x0054) = 0x00000000;//a. testen
}
