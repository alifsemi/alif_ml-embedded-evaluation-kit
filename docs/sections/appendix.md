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

The default Cortex®-M55 memory map can be found here: <https://developer.arm.com/documentation/101051/0002/Memory-model/Memory-map>.

The Corstone™-300 memory map can be found here: <https://developer.arm.com/documentation/100966/1126/Arm--Corstone-SSE-300-FVP/Memory-map-overview-for-Corstone-SSE-300>.

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

The Corstone™-310 memory map can be found here: <https://developer.arm.com/documentation/100966/1126/Arm--Corstone-SSE-310-FVP/Corstone-SSE-310-FVP-memory-map-overview>.

## Arm® Cortex®-M55 Memory map overview for Corstone™-315 reference design

The following table refers to the memory mapping information specific to the Arm® Cortex®-M55.

| Name | Base address | Limit address | Size    | IDAU | Remarks                                               |
|------|--------------|---------------|---------|------|-------------------------------------------------------|
| ITCM | 0x0000_0000  |  0x0000_7FFF  | 32 kiB  | NS   | ITCM code region                                      |
| BRAM | 0x0100_0000  |  0x0120_0000  | 2 MiB   | NS   | FPGA data SRAM region                                 |
| DTCM | 0x2000_0000  |  0x2000_7FFF  | 32 kiB  | NS   | 4 banks for 8 kiB each                                |
| SRAM | 0x2100_0000  |  0x213F_FFFF  | 4 MiB   | NS   | 2 banks of 2 MiB each as SSE-315 internal SRAM region |
| DDR  | 0x6000_0000  |  0x6FFF_FFFF  | 256 MiB | NS   | DDR memory region                                     |
| ITCM | 0x0A00_0000  |  0x0A00_7FFF  | 32 kiB  | S    | DMA ITCM CPU0 S-AHB Instruction TCM Access            |
| ITCM | 0x1000_0000  |  0x1000_7FFF  | 32 kiB  | S    | ITCM code region                                      |
| BOOT | 0x1100_0000  |  0x1101_FFFF  | 128 KiB | S    | Boot ROM                                              |
| BRAM | 0x1200_0000  |  0x121F_FFFF  | 2 MiB   | S    | FPGA data SRAM region                                 |
| ITCM | 0x1A00_0000  |  0x1A00_7FFF  | 32 kiB  | S    | DMA ITCM                                              |
| DTCM | 0x3000_0000  |  0x3000_7FFF  | 32 kiB  | S    | 4 banks for 8 kiB each                                |
| SRAM | 0x3100_0000  |  0x313F_FFFF  | 4 MiB   | S    | 2 banks of 2 MiB each as SSE-310 internal SRAM region |
| DDR  | 0x7000_0000  |  0x7FFF_FFFF  | 256 MiB | S    | DDR memory region                                     |

The Corstone™-315 memory map can be found here: <https://developer.arm.com/documentation/109395/0000/SSE-315-FVP/SSE-315-FVP-memory-map>.
