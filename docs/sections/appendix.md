# Appendix

## Arm® Cortex®-M55 Memory map overview for Corstone™-300 reference design

The following table refers to the memory mapping information specific to the Arm® Cortex®-M55.

| Name  | Base address | Limit address |  Size     | IDAU |  Remarks                                                  |
|-------|--------------|---------------|-----------|------|-----------------------------------------------------------|
| ITCM  | 0x0000_0000  |  0x0007_FFFF  |  512 kiB  |  NS  |   ITCM code region                                        |
| BRAM  | 0x0100_0000  |  0x0110_0000  |   1 MiB   |  NS  |   FPGA data SRAM region                                   |
| DTCM  | 0x2000_0000  |  0x2007_FFFF  |  512 kiB  |  NS  |   4 banks for 128 kiB each                                |
| SRAM  | 0x2100_0000  |  0x2120_0000  |   2 MiB   |  NS  |   2 banks of 1 MiB each as SSE-300 internal SRAM region   |
| DDR   | 0x6000_0000  |  0x6FFF_FFFF  |   256 MiB |  NS  |   DDR memory region                                       |
| ITCM  | 0x1000_0000  |  0x1007_FFFF  |   512 kiB |  S   |   ITCM code region                                        |
| BRAM  | 0x1100_0000  |  0x1110_0000  |   1 MiB   |  S   |   FPGA data SRAM region                                   |
| DTCM  | 0x3000_0000  |  0x3007_FFFF  |   512 kiB |  S   |   4 banks for 128 kiB each                                |
| SRAM  | 0x3100_0000  |  0x3120_0000  |   2 MiB   |  S   |   2 banks of 1 MiB each as SSE-300 internal SRAM region   |
| DDR   | 0x7000_0000  |  0x7FFF_FFFF  |  256 MiB  |  S   |   DDR memory region                                       |

The default memory map can be found here: <https://developer.arm.com/documentation/101051/0002/Memory-model/Memory-map>.

## Arm® Cortex®-M55 Memory map overview for Corstone™-310 reference design

The following table refers to the memory mapping information specific to the Arm® Cortex®-M55.

| Name  | Base address | Limit address |  Size     | IDAU |  Remarks                                                  |
|-------|--------------|---------------|-----------|------|-----------------------------------------------------------|
| ITCM  | 0x0000_0000  |  0x0000_7FFF  |   32 kiB  |  NS  |   ITCM code region                                        |
| BRAM  | 0x0100_0000  |  0x0120_0000  |   2 MiB   |  NS  |   FPGA data SRAM region                                   |
| DTCM  | 0x2000_0000  |  0x2000_7FFF  |   32 kiB  |  NS  |   4 banks for 8 kiB each                                  |
| SRAM  | 0x2100_0000  |  0x213F_FFFF  |   4 MiB   |  NS  |   2 banks of 2 MiB each as SSE-310 internal SRAM region   |
| DDR   | 0x6000_0000  |  0x6FFF_FFFF  |   256 MiB |  NS  |   DDR memory region                                       |
| ITCM  | 0x1000_0000  |  0x1000_7FFF  |   32 kiB  |  S   |   ITCM code region                                        |
| BRAM  | 0x1100_0000  |  0x1120_0000  |   2 MiB   |  S   |   FPGA data SRAM region                                   |
| DTCM  | 0x3000_0000  |  0x3000_7FFF  |   32 kiB  |  S   |   4 banks for 8 kiB each                                  |
| SRAM  | 0x3100_0000  |  0x313F_FFFF  |   4 MiB   |  S   |   2 banks of 2 MiB each as SSE-310 internal SRAM region   |
| DDR   | 0x7000_0000  |  0x7FFF_FFFF  |  256 MiB  |  S   |   DDR memory region                                       |

