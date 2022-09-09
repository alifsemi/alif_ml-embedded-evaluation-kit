


#include <stdio.h>
#include <stdint.h>
#include "global_map.h"
#include "base_def.h"
#include "mipi_dsi_host.h"
#include "display.h"
#include "delay.h"

static void DSI_DPHY_REG_WRITE(uint16_t,uint8_t);
static uint8_t DSI_DPHY_REG_READ(uint16_t);

void dsi_command_mode_initialization(void)
{
	////////////////////////////////////////////////////////////////
	// DSI Controller Setup
	////////////////////////////////////////////////////////////////

	//1.1 configure Vdieo Mode type VID_MODE_CFG[1:0]
	//1:0 vid_mode_type R/W This field indicates the video mode transmission type
	//15 lp_cmd_en R/W When set to 1, this bit enables the command transmission only in low-power mode.
	HW_REG_WORD(MIPI_DSI_BASE,0x38) = 0x00003F02; //burst mode

	//The following are added for ESC mode LPDT
	//CLKMGR_CFG
	//7:0 tx_esc_clk_division R/W
	//  This field indicates the division factor for the TX Escape clock source (lanebyteclk).
	//  The values 0 and 1 stop the TX_ESC clock generation.
	HW_REG_WORD(MIPI_DSI_BASE,0x8) = 4;

	//CMD_MODE_CFG
	//24 max_rd_pkt_size
	//19 dcs_lw_tx
	//18 dcs_sr_0p_tx
	//17 dcs_sw_1p_tx
	//16 dcs_sw_0p_tx
	//14 gen_lw_tx
	//13 gen_sr_2p_tx
	//12 gen_sr_1p_tx R/W This bit configures the Generic short read packet with one parameter command transmission type:
	//11 gen_sr_0p_tx R/W This bit configures the Generic short read packet with zero parameter command transmission type:
	//10 gen_sw_2p_tx R/W This bit configures the Generic short write packet with two parameters command transmission type:
	//9 gen_sw_1p_tx R/W This bit configures the Generic short write packet with one parameter command transmission type:
	//8 gen_sw_0p_tx R/W This bit configures the Generic short write packet with zero parameter command transmission type:
	//low power = 1; high speed = 0;
	HW_REG_WORD(MIPI_DSI_BASE,0x68)=0x010F7F00;

	HW_REG_WORD(MIPI_DSI_BASE,0x4) = 1;	// DSI controller enable
}

void dsi_video_mode_initialization(void)
{
	////////////////////////////////////////////////////////////////////////////
	// Video Mode
	////////////////////////////////////////////////////////////////////////////

	//This register configures the possibility for using non continuous clock in the clock lane.
	//0 phy_txrequestclkhs R/W This bit controls the D-PHY PPI txrequestclkhs signal.
	//from Synopsys
	HW_REG_WORD(MIPI_DSI_BASE,0x94) = 1;

	//1. configure MODE_CFG register to enable video mode
	HW_REG_WORD(MIPI_DSI_BASE,0x34) = 0;

	//2. configure DPI_COLOR_CODING register
	// 0x5 (CC05): 24-bit
	HW_REG_WORD(MIPI_DSI_BASE,0x10) = 5;

	//3. vid_pkt_size
	// pixel perline
	HW_REG_WORD(MIPI_DSI_BASE,0x3c) = HACT;

	//4. vid_num_chunks
	// chunks per line
	HW_REG_WORD(MIPI_DSI_BASE,0x40) = 0; //no chunks

	//5. null packet
	//12:0 vid_null_size R/W This register configures the number of bytes inside a null packet. Setting to 0 disables null packets.
	HW_REG_WORD(MIPI_DSI_BASE,0x44) = 0; //disable

	//6. This register configures the video HSA time.
	//11:0 vid_hsa_time R/W This field configures the Horizontal Sync Active period in lane byte clock cycles.
	HW_REG_WORD(MIPI_DSI_BASE,0x48) = HSA<<1;

	//7. This register configures the video HBP time.
	//11:0 vid_hbp_time R/W This field configures the Horizontal Back Porch period in lane byte clock cycles
	HW_REG_WORD(MIPI_DSI_BASE,0x4C) = HBP<<1;

	//8. This register configures the overall time for each video line.
	//14:0 vid_hline_time R/W This field configures the size of the total line time (HSA+HBP+HACT+HFP) counted in lane byte clock cycles.
	HW_REG_WORD(MIPI_DSI_BASE,0x50) = (HSA+HBP+HACT+HFP)<<1;

	//9. This register configures the VSA period.
	//9:0 vsa_lines R/W This field configures the Vertical Sync Active period measured in number of horizontal lines.
	HW_REG_WORD(MIPI_DSI_BASE,0x54) = VSA;

	//10. This register configures the VBP period.
	//9:0 vbp_lines R/W This field configures the Vertical Back Porch period measured in number of horizontal lines.
	HW_REG_WORD(MIPI_DSI_BASE,0x58) = VBP;

	//11. This register configures the VFP period.
	//9:0 vfp_lines R/W This field configures the Vertical Front Porch period measured in number of horizontal lines.
	HW_REG_WORD(MIPI_DSI_BASE,0x5C) = VFP;

	//12. This register configures the vertical resolution of video.
	//13:0 v_active_lines R/W This field configures the Vertical Active period measured in number of horizontal lines.
	HW_REG_WORD(MIPI_DSI_BASE,0x60) = VACT;
}

#define DCS_DELAY  100

void DCSWN_S (uint8_t cmd)
{
	HW_REG_WORD(MIPI_DSI_BASE,A_GEN_HDR) = 0x05 | cmd<<8;
	sleep_or_wait_usec(DCS_DELAY);
}

void DCSW1_S (uint8_t cmd, uint8_t data)
{
	HW_REG_WORD(MIPI_DSI_BASE,A_GEN_HDR) = 0x15 | cmd<<8 | data<<16;
	sleep_or_wait_usec(DCS_DELAY);
}

void DCSRN_S (uint8_t cmd)
{
	HW_REG_WORD(MIPI_DSI_BASE,A_GEN_HDR) = 0x137;
	sleep_or_wait_usec(DCS_DELAY);
	HW_REG_WORD(MIPI_DSI_BASE,A_GEN_HDR) = 0x06 | cmd<<8;
	sleep_or_wait_usec(DCS_DELAY);
}

void SMRPS_S(uint8_t num_bytes)
{
	HW_REG_WORD(MIPI_DSI_BASE,A_GEN_HDR) = 0x37 | num_bytes<<8;
	sleep_or_wait_usec(DCS_DELAY);
}

void DCSW_L (uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
	HW_REG_WORD(MIPI_DSI_BASE,A_GEN_PLD_DATA) = data2<<24 | data1<<16 | cmd<<8 | cmd;
	HW_REG_WORD(MIPI_DSI_BASE,A_GEN_PLD_DATA) = data4<<8 | data3;
	HW_REG_WORD(MIPI_DSI_BASE,A_GEN_HDR) = 0x39 | 0x6<<8;
	sleep_or_wait_usec(DCS_DELAY);
}

int tx_phyconfig(void)
{
	uint32_t rd_data = 0;
	uint8_t r_data_8;

	////////////////////////////////////////////////////////////////
	// DSI D-PHY Setup
	////////////////////////////////////////////////////////////////

	/* Switch to 38.4MHz Crystal OSC */
	HW_REG_WORD(0x71007410,0x00) |= (1U << 1);

	/* Enable MIPI PHY LDO */
	HW_REG_WORD(0x7004001C,0x00) |= (1U << 10);

	sleep_or_wait_msec(100);

	HW_REG_WORD(MIPI_DSI_BASE,0x4) = 0;	// reset DSI controller

	HW_REG_WORD(MIPI_DSI_BASE,0xA0) = 0;	// reset DSI D-PHY and PLL

	HW_REG_WORD(MIPI_DSI_BASE,0xA4) = 1;	// number of lines: 0 for 1 lane data, 1 for 2 lane data

	HW_REG_WORD(CFGSLV1_BASE,0x30) =  0x00000100;
	HW_REG_WORD(MIPI_DSI_BASE,0xB4) = 0x00000000;//test_clr=0
	HW_REG_WORD(MIPI_DSI_BASE,0xB4) = 0x00000001;//test_clr=1
	HW_REG_WORD(MIPI_DSI_BASE,0xB4) = 0x00000000;//test_clr=0
	HW_REG_WORD(CFGSLV1_BASE,0x30) =  0x00000110;
	HW_REG_WORD(MIPI_DSI_BASE,0xB4) = 0x00000000;//test_clr=0
	HW_REG_WORD(MIPI_DSI_BASE,0xB4) = 0x00000001;//test_clr=1
	HW_REG_WORD(MIPI_DSI_BASE,0xB4) = 0x00000000;//test_clr=0
	HW_REG_WORD(CFGSLV1_BASE,0x30) =  0x00000100;

	//7. Set hsfreqrange[6:0] and cfgclkfreqrange[7:0]
	//[22:16] - hsfreqrange[6:0]
	//[31:24] - cfgclkfreqrange[7:0] = round[(cfgclk-17)*4] = (25-17)*4 = 8'b001000000
	//cfgclk = 25MHz

	//7.1 TX
	HW_REG_WORD(CFGSLV1_BASE,0x30) = (0x20 << 24) | (hsfreqrange << 16) | (1U << 8);

	//8. Configure TX register 0x16A to set pll_mpll_prog_rw (bits1:0) to 2'b11.
	//Before D-PHY power, set TX register 0x16A bits1:0 to 2'b11 (pll_mpll_prog_rw).
	r_data_8 = DSI_DPHY_REG_READ(0x16A);
	DSI_DPHY_REG_WRITE(0x16A, r_data_8 | 3);

	//9. Configure TX register 0x1AB to set cb_sel_vref_lprx_rw (bits 1:0) to 2'b10.
	r_data_8 = DSI_DPHY_REG_READ(0x1AB);
	DSI_DPHY_REG_WRITE(0x1AB, (r_data_8 & ~3) | 2);

	//10. Configure TX register 0x1AA to set cb_sel_vrefcd_lprx_rw (bits 6:5) to 2'b10.
	r_data_8 = DSI_DPHY_REG_READ(0x1AA);
	DSI_DPHY_REG_WRITE(0x1AA, (r_data_8 & ~0x60) | 0x40);

	//When operating as master or when in High-Speed BIST modes,
	//for datarates below 450 Mbps, clkdiv_clk_en must be enabled.
	//To do this, write 1'b1 in TX configuration register with address 0x1AC bit [4].
	r_data_8 = DSI_DPHY_REG_READ(0x1AC);
	DSI_DPHY_REG_WRITE(0x1AC, r_data_8 | (below_450Mbps << 4));

	//11. Configure TX register 0x402 to set txclk_term_lowcap_lp00_en_ovr_en_rw and
	//txclk_term_lowcap_lp00_en_ovr_rw(bits 1:0) to 2'b10;
	DSI_DPHY_REG_WRITE(0x402,0x2);

	//12. Refer to table "Supported rise/fall time limits" and configure TX test control registers
	//with appropriate values for the specified rise/fall time.
	//between 500 Mbps and 1 Gbps, 1 ns, 150 ps - 300 ps, 1, 12'd657, 225
	//under 500 Mbps, 2 ns, 150 ps - 300 ps, 1, 12'd657, 225
	//dphy4txtester_DIG_RDWR_TX_SLEW_5
	DSI_DPHY_REG_WRITE(0x270,0x91);

	//dphy4txtester_DIG_RDWR_TX_SLEW_6
	DSI_DPHY_REG_WRITE(0x271,0x2);

	//dphy4txtester_DIG_RDWR_TX_SLEW_7
	//13. Set bits [5:4] of TX control register with address 0x272 to 2'b01 to enable slew rate calibration. (Lanes
	//that are disabled should not run slew-rate calibration algorithm. For the disabled lanes, set
	//sr_finished_ovr_en = 1??b1, sr_finished_ovr = 1??b1, srcal_en_ovr_en = 1??b1 through test control
	//registers 0x310, 0x50b, 0x70b);
	DSI_DPHY_REG_WRITE(0x272,0x11);

	//For the disabled lanes, set
	//sr_finished_ovr_en = 1??b1, sr_finished_ovr = 1??b1, srcal_en_ovr_en = 1??b1 through test control registers 0x310, 0x50b, 0x70b, 0x90b, 0xbob);
	//bit2, bit1, bit0 are given as "reserved" in the Databook; but program as per the requirement.
	//Since you are not using lane2, lane3 - you can disable those bits in the registers for lane2 and lane3.

	//dphy4txtester_DIG_RDWR_TX_LANE2_SLEWRATE_0
	//lane 2
	DSI_DPHY_REG_WRITE(0x90b,0x0e);

	//lane 3
	DSI_DPHY_REG_WRITE(0xb0b,0x0e);

	//14. Set cfgclkfreqrange[7:0] = round[(Fcfg_clk(MHz)-17)*4] = 8'b00101000;
	//already set
	//15. Apply cfg_clk signal with the appropriate frequency with 25 Mhz frequency;
	//16. Configure PLL operating frequency through D-PHY test control registers or through PLL SoC

	//pll_soc_clksel [21:20]	RW	0x0	clkext div selection - clksel
	//should be set to 2'b01
	rd_data = HW_REG_WORD(CFGSLV1_BASE,0x10);
	rd_data |= 1UL << 20;
	HW_REG_WORD(CFGSLV1_BASE,0x10) = rd_data;

	//When the PHY is configured as a master (tx_rxz=1'b1) the PLL needs to always be properly configured for
	//the desired operating frequency before D-PHY Start-up.
	//Use pll shadow control (set to 0 if using SoC registers, 1 for D-PHY test interface)
	rd_data = HW_REG_WORD(CFGSLV1_BASE,0x10);
	rd_data |= 1UL << 4;
	HW_REG_WORD(CFGSLV1_BASE,0x10) = rd_data;

	//m[11:0] - pll_m_ovr_rw[7:0], pll_m_ovr_rw[11:8], pll_m_ovr_en_rw ?? 0x179, 0x17a, 0x17b
	//n[3:0] - pll_n_ovr_rw[3:0], pll_n_ovr_en_rw ?? 0x178
	//Foutmax [MHz] Foutmin [MHz] F VCO max [MHz] F VCO min [MHz] Output division factor P vco_cntrl[5:3]
	//1000          500           4000            2000            4                        001
	//500           250           4000            2000            8                        010
	//250           125           4000            2000            16                       011
	//125           62.5          4000            2000            32                       100
	//62.5          40            4000            2000            64                       101
	//24Mhz > fclkin/N > 8 Mhz
	//24Mhz > 38.4Mhz/N > 8 Mhz
	//N = 4
	//n = 4 - 1

	//fout = bit rate / 2
	//M/N*1/2*fclkin*1/P = M/3*1/2*38.4Mhz*1/64 = 40Mhz = 400 = 0x190
	//M = 400 = 0x190

	//MIPI-DPHY PLL Control Register 1
	//pll_soc_m [9:0]	RW	0x0	Control of the feedback multiplication ratio M (40 to 625) for SoC direct PLL control	( dphy_pll_ctrl1.pll_soc_m = 0x0 )
	//pll_soc_n [15:12]	RW	0x0	Control of the input frequency division ratio N (1 to 16) for SoC direct PLL control	( dphy_pll_ctrl1.pll_soc_n = 0x0 )
	//HW_REG_WORD(CFGSLV1_BASE,0x14) = 0x031A1;

	//dphy4txtester_DIG_RDWR_TX_PLL_28
	//7:0 pll_m_ovr_rw__7__0__ R/W Description: PLL feedback divider override
	DSI_DPHY_REG_WRITE(0x179,pll_soc_m_7_0);

	//dphy4txtester_DIG_RDWR_TX_PLL_29
	//1:0 pll_m_ovr_rw__9__8__ R/W Description: PLL feedback divider override
	DSI_DPHY_REG_WRITE(0x17a,pll_soc_m_9_8);

	//dphy4txtester_DIG_RDWR_TX_PLL_30
	//0 pll_m_ovr_en_rw R/W Description: PLL feedback divider override enable
	DSI_DPHY_REG_WRITE(0x17b,0x1);

	//dphy4txtester_DIG_RDWR_TX_PLL_27
	//7 pll_n_ovr_en_rw R/W Description: PLL input divider override enable
	//6:3 pll_n_ovr_rw__3__0__ R/W Description: PLL input divider override
	DSI_DPHY_REG_WRITE(0x178,(0x80|(pll_soc_n<<3)));

	// vco_cntrl[5:0] - pll_vco_cntrl_ovr_rw[5:0], pll_vco_cntrl_ovr_en_rw ?? 0x17b
	//dphy4txtester_DIG_RDWR_TX_PLL_30
	//7 pll_vco_cntrl_ovr_en_rw R/W Description: PLL VCO control override enable
	//6:1 pll_vco_cntrl_ovr_rw__5__0__ R/W Description: PLL VCO control override
	//0 pll_m_ovr_en_rw R/W Description: PLL feedback divider override enable
	//Table 3-6
	DSI_DPHY_REG_WRITE(0x17b,(0x81|vco_cntrl<<1));

	// cpbias_cntrl[6:0] - pll_cpbias_cntrl_rw[6:0] ?? 0x15e
	// dphy4txtester_DIG_RDWR_TX_PLL_1
	// 6:0 pll_cpbias_cntrl_rw__6__0__ R/W Description: PLL Charge Pump Bias Control
	DSI_DPHY_REG_WRITE(0x15e,0x0);

	// gmp_cntrl[1:0] - pll_gmp_cntrl_rw[1:0] ?? 0x162
	// int_cntrl[5:0] - pll_int_cntrl_rw[5:0] ?? 0x162
	// dphy4txtester_DIG_RDWR_TX_PLL_5
	// 7:2 pll_int_cntrl_rw__5__0__ R/W Description: PLL Integral Charge Pump control
	// 1:0 pll_gmp_cntrl_rw__1__0__ R/W Description: PLL GMP Control
	DSI_DPHY_REG_WRITE(0x162,0x11);

	// prop_cntrl[5:0] - pll_prop_cntrl_rw[5:0] ?? 0x16e
	// dphy4txtester_DIG_RDWR_TX_PLL_17
	// 5:0 pll_prop_cntrl_rw__5__0__ R/W Description: PLL Proportional Charge Pump control
	DSI_DPHY_REG_WRITE(0x16e,0x10);

	//SNPS: dphy4txtester_DIG_RDWR_TX_CB_3
	//2:0 cb_vref_mpll_reg_sel_rw__2__0__R/W Description: PLL reference voltage control
	DSI_DPHY_REG_WRITE(0x1AD,0x02);

	//17. Set basedir_n = 1'b0;
	//18. Set forcerxmode_n = 1'b0;
	HW_REG_WORD(CFGSLV1_BASE,0x34)=0x00000000;

	//19. Set all requests inputs to zero;
	//20. Wait for 15 ns;
	// PMU_delay_loop_ns(15);

	//21. Set enable_n and enableclk=1'b1;
	HW_REG_WORD(MIPI_DSI_BASE,0xa0)=0x00000004;

	//22. Wait 5 ns;
	// PMU_delay_loop_ns(5);

	//23. Set shutdownz=1'b1;
	HW_REG_WORD(MIPI_DSI_BASE,0xa0)=0x00000005;

	//24. Wait 5 ns;
	// PMU_delay_loop_ns(5);

	//25. Set rstz=1'b1;
	HW_REG_WORD(MIPI_DSI_BASE,0xa0)=0x00000007;

	//dphy4txtester_DIG_RDWR_TX_PLL_17
	// 7 pll_pwron_ovr_rw R/W Description: PLL poweron override
	// 6 pll_pwron_ovr_en_rw R/W Description: PLL poweron override enable control
	DSI_DPHY_REG_WRITE(0x16e,0xD0);

	//26. Wait until stopstatedata_n and stopstateclk outputs are asserted indicating PHY is driving LP11 in
	//enabled datalanes and clocklane.
	do {
		sleep_or_wait_usec(20);
		rd_data = HW_REG_WORD(MIPI_DSI_BASE,0xB0);

	} while ((rd_data & 0x00000094) != 0x00000094);

	return 0;
}


uint8_t DSI_DPHY_REG_READ(uint16_t reg_addr)
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

	uint32_t read_data;
	uint8_t r_data_8;

	//1.
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000000;//a. testclk
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00000000;//a. testen
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00010000;//b. testen_high
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000002;//c. testclk high
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00010000;//d. place testdin 0x00
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000000;//e. testclk low
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00000000;//f. place testen low
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = reg_addr >> 8;//g. MSB testdin
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000002;//h. set testclk high

	//2.
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000000;//a. testclk low
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00010000 | (reg_addr >> 8);//b. testen_high
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000002;//c. set testclk high
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00010000 | (reg_addr & 0xff); //d. LSB testdin
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000000;//e. set testclk low
	read_data = HW_REG_WORD(MIPI_DSI_BASE,0x00b8);
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00000000;//f. testen_low

	r_data_8 = (read_data >> 8) & 0xff;

	return r_data_8;
}

void DSI_DPHY_REG_WRITE(uint16_t reg_addr,uint8_t write_data)
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

	uint8_t r_data_8;

	//1.
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000000;//a. testclk
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00000000;//a. testen
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00010000;//b. testen_high
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000002;//c. testclk high
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00010000;//d. place testdin 0x00
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000000;//e. testclk low
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00000000;//f. place testen low
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = reg_addr >> 8;//g. MSB testdin
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000002;//h. set testclk high

	//2.
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000000;//a. testclk low
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00010000 | (reg_addr >> 8);//b. testen_high
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000002;//c. set testclk high
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00010000 | (reg_addr & 0xff); //d. LSB testdin
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000000;//e. set testclk low
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00000000 | (reg_addr & 0xff);//b. testen_low

	//3.
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = write_data; //a.
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000002; //b. testclk high

	//turn off for clean exit
	HW_REG_WORD(MIPI_DSI_BASE,0x00b4) = 0x00000000;//a. testclk
	HW_REG_WORD(MIPI_DSI_BASE,0x00b8) = 0x00000000;//a. testen
}
