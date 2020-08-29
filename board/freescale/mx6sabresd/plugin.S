/*
 * Copyright (C) 2012-2014 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>

/* DDR script */
.macro imx6dqsabresd_ddr_setting
	ldr r0, =IOMUXC_BASE_ADDR

	/* STROBE */
	ldr r1, =0x00020038
	str r1, [r0, #0x5a8]
	str r1, [r0, #0x5b0]
	str r1, [r0, #0x524]
	str r1, [r0, #0x51c]
	str r1, [r0, #0x518]
	str r1, [r0, #0x50c]
	str r1, [r0, #0x5b8]
	str r1, [r0, #0x5c0]


	/* DATA */
	ldr r1, =0x00000030
	str r1, [r0, #0x784]
	str r1, [r0, #0x788]
	str r1, [r0, #0x794]
	str r1, [r0, #0x79c]
	str r1, [r0, #0x7a0]
	str r1, [r0, #0x7a4]
	str r1, [r0, #0x7a8]
	str r1, [r0, #0x748]


	/* ADDRESS */
	str r1, [r0, #0x56c]
	str r1, [r0, #0x578]

	/* CLOCK */
	str r1, [r0, #0x588]
	str r1, [r0, #0x594]

	/* CONTROL */
	str r1, [r0, #0x57c]

	ldr r1, =0x00003000
	str r1, [r0, #0x590]
	str r1, [r0, #0x598]


	ldr r1, =0x00000000
	str r1, [r0, #0x58c]

	ldr r1, =0x00003030
	str r1, [r0, #0x59c]
	str r1, [r0, #0x5a0]


	ldr r1, =0x00000038
	str r1, [r0, #0x784]
	str r1, [r0, #0x788]
	str r1, [r0, #0x794]
	str r1, [r0, #0x79c]
	str r1, [r0, #0x7a0]
	str r1, [r0, #0x7a4]
	str r1, [r0, #0x7a8]
	str r1, [r0, #0x748]


	ldr r1, =0x00000030
	str r1, [r0, #0x74c]

	ldr r1, =0x00020000
	str r1, [r0, #0x750]

	ldr r1, =0x00000000
	str r1, [r0, #0x758]

	ldr r1, =0x00020000
	str r1, [r0, #0x774]

	ldr r1, =0x00000030
	str r1, [r0, #0x78c]

	ldr r1, =0x000c0000
	str r1, [r0, #0x798]

	/* CALIBRATION */
	ldr r0, =MMDC_P0_BASE_ADDR
	ldr r1, =0x44444444
	str r1, [r0, #0x81c]
	str r1, [r0, #0x820]
	str r1, [r0, #0x824]
	str r1, [r0, #0x828]
	str r1, [r0, #0x481c]
	str r1, [r0, #0x4820]
	str r1, [r0, #0x4824]
	str r1, [r0, #0x4828]

	ldr r1, =0xD1111111
	str r1, [r0, #0x82c]
	str r1, [r0, #0x830]
	str r1, [r0, #0x834]
	str r1, [r0, #0x838]
	str r1, [r0, #0x482c]
	str r1, [r0, #0x4830]
	str r1, [r0, #0x4834]
	str r1, [r0, #0x4838]

	//For i.mx6qd parts of versions A & B (v1.0, v1.1), uncomment the following lines. For version C (v1.2), keep commented
	ldr r1, =0x24911492
	str r1, [r0, #0x8c0]
	str r1, [r0, #0x48c0]

// according to RefMan 44.4.2
// 1.
	ldr r1, =0x00008000
	str r1, [r0, #0x01c]

// 2.
	ldr r1, =0x555A7975
	str r1, [r0, #0x00c]
	ldr r1, =0xFF538F64
	str r1, [r0, #0x010]
	ldr r1, =0x01ff00db
	str r1, [r0, #0x014]
	ldr r1, =0x09444040
	str r1, [r0, #0x008]

// 3.
	ldr r1, =0x00081740
	str r1, [r0, #0x018]

// 4.
	ldr r1, =0x005A1023
	str r1, [r0, #0x030]
	ldr r1, =0x000026d2
	str r1, [r0, #0x02c]


//in DDR3, 64-bit mode, only MMDC0 is initiated:
	ldr r1, =0x00020036
	str r1, [r0, #0x004]
	ldr r1, =0x00000027
	str r1, [r0, #0x040]
	ldr r1, =0x831a0000
	str r1, [r0, #0x000]

// Initialize 1GB DDR3 - Micron MT41J128M , but fit wide range of other DDR3 devices
//MR2:
	ldr r1, =0x02088032
	str r1, [r0, #0x01c]
	ldr r1, =0x0208803a
	str r1, [r0, #0x01c]

//MR3:
	ldr r1, =0x00008033
	str r1, [r0, #0x01c]
	ldr r1, =0x0000803b
	str r1, [r0, #0x01c]

//MR1:
	ldr r1, =0x00048031
	str r1, [r0, #0x01c]
	ldr r1, =0x00048039
	str r1, [r0, #0x01c]
	ldr r1, =0x00068031
	str r1, [r0, #0x01c]
	ldr r1, =0x00068039
	str r1, [r0, #0x01c]

//MR0:
	ldr r1, =0x09408030
	str r1, [r0, #0x01c]
	ldr r1, =0x09408038
	str r1, [r0, #0x01c]

//DDR device ZQ calibration:
	ldr r1, =0x04008040
	str r1, [r0, #0x01c]
	ldr r1, =0x04008048
	str r1, [r0, #0x01c]

// ZQ:
	ldr r1, =0xA5380003
	str r1, [r0, #0x800]
	str r1, [r0, #0x4800]

	ldr r1, =0x00005800
	str r1, [r0, #0x020]

	ldr r1, =0x00022227
	str r1, [r0, #0x818]
	str r1, [r0, #0x4818]

//Read DQS Gating calibration
	ldr r1, =0x43500364
	str r1, [r0, #0x83c]
	ldr r1, =0x03440344
	str r1, [r0, #0x840]
	ldr r1, =0x43380348
	str r1, [r0, #0x483c]
	ldr r1, =0x033C0310
	str r1, [r0, #0x4840]

//Read calibration

	ldr r1, =0x30303030
	str r1, [r0, #0x848]
	str r1, [r0, #0x4848]

//Write calibration
	ldr r1, =0x34343434
	str r1, [r0, #0x850]
	ldr r1, =0x38383838
	str r1, [r0, #0x4850]

// write leveling
	ldr r1, =0x0025001F
	str r1, [r0, #0x80c]
	ldr r1, =0x002F0028
	str r1, [r0, #0x810]

	ldr r1, =0x00160027
	str r1, [r0, #0x480c]
	ldr r1, =0x0014001F
	str r1, [r0, #0x4810]

	ldr r1, =0x00000800
	str r1, [r0, #0x8b8]
	str r1, [r0, #0x48b8]


	ldr r1, =0x00000000
	str r1, [r0, #0x01c]

// Local additions

	ldr r0, =IOMUXC_BASE_ADDR
	ldr r1, =0xf00000cf
	str r1, [r0, #0x010]

	ldr r1, =0x007f007f
	str r1, [r0, #0x018]
	str r1, [r0, #0x01c]
.endm


.macro imx6dlsabresd_ddr_setting
	ldr r0, =IOMUXC_BASE_ADDR
	ldr r1, =0x000c0000
	str r1, [r0, #0x774]
	ldr r1, =0x00000000
	str r1, [r0, #0x754]

	ldr r1, =0x00000030
	str r1, [r0, #0x4ac]
	str r1, [r0, #0x4b0]
	str r1, [r0, #0x464]
	str r1, [r0, #0x490]
	str r1, [r0, #0x74c]
	str r1, [r0, #0x494]

	ldr r1, =0x00000000
	str r1, [r0, #0x4a0]

	ldr r1, =0x00000030
	str r1, [r0, #0x4b4]
	str r1, [r0, #0x4b8]
	str r1, [r0, #0x76c]

	ldr r1, =0x00020000
	str r1, [r0, #0x750]

	ldr r1, =0x00000030
	str r1, [r0, #0x4bc]
	str r1, [r0, #0x4c0]
	str r1, [r0, #0x4c4]
	str r1, [r0, #0x4c8]
	str r1, [r0, #0x4cc]
	str r1, [r0, #0x4d0]
	str r1, [r0, #0x4d4]
	str r1, [r0, #0x4d8]

	ldr r1, =0x00020000
	str r1, [r0, #0x760]

	ldr r1, =0x00000030
	str r1, [r0, #0x764]
	str r1, [r0, #0x770]
	str r1, [r0, #0x778]
	str r1, [r0, #0x77c]
	str r1, [r0, #0x780]
	str r1, [r0, #0x784]
	str r1, [r0, #0x78c]
	str r1, [r0, #0x748]
	str r1, [r0, #0x470]
	str r1, [r0, #0x474]
	str r1, [r0, #0x478]
	str r1, [r0, #0x47c]
	str r1, [r0, #0x480]
	str r1, [r0, #0x484]
	str r1, [r0, #0x488]
	str r1, [r0, #0x48c]

	ldr r0, =MMDC_P0_BASE_ADDR
	ldr r2, =0xa1390003
	str r2, [r0, #0x800]

	ldr r2, =0x001f001f
	str r2, [r0, #0x80c]
	str r2, [r0, #0x810]
	ldr r1, =MMDC_P1_BASE_ADDR
	str r2, [r1, #0x80c]
	str r2, [r1, #0x810]

	ldr r2, =0x4220021F
	str r2, [r0, #0x83c]
	ldr r2, =0x0207017E
	str r2, [r0, #0x840]

	ldr r2, =0x4201020C
	str r2, [r1, #0x83c]
	ldr r2, =0x01660172
	str r2, [r1, #0x840]

	ldr r2, =0x4A4D4E4D
	str r2, [r0, #0x848]
	ldr r2, =0x4A4F5049
	str r2, [r1, #0x848]

	ldr r2, =0x3F3C3D31
	str r2, [r0, #0x850]
	ldr r2, =0x3238372B
	str r2, [r1, #0x850]

	ldr r2, =0x33333333
	str r2, [r0, #0x81c]
	str r2, [r0, #0x820]
	str r2, [r0, #0x824]
	str r2, [r0, #0x828]
	str r2, [r1, #0x81c]
	str r2, [r1, #0x820]
	str r2, [r1, #0x824]
	str r2, [r1, #0x828]

	ldr r2, =0x00000800
	str r2, [r0, #0x8b8]
	str r2, [r1, #0x8b8]

	ldr r2, =0x0002002D
	str r2, [r0, #0x004]
	ldr r2, =0x00333030
	str r2, [r0, #0x008]

	ldr r2, =0x3F435313
	str r2, [r0, #0x00c]
	ldr r2, =0xB66E8B63
	str r2, [r0, #0x010]

	ldr r2, =0x01FF00DB
	str r2, [r0, #0x014]
	ldr r2, =0x00001740
	str r2, [r0, #0x018]

	ldr r2, =0x00008000
	str r2, [r0, #0x01c]
	ldr r2, =0x000026d2
	str r2, [r0, #0x02c]
	ldr r2, =0x00431023
	str r2, [r0, #0x030]
	ldr r2, =0x00000027
	str r2, [r0, #0x040]

	ldr r2, =0x831A0000
	str r2, [r0, #0x000]

	ldr r2, =0x04008032
	str r2, [r0, #0x01c]
	ldr r2, =0x00008033
	str r2, [r0, #0x01c]
	ldr r2, =0x00048031
	str r2, [r0, #0x01c]
	ldr r2, =0x05208030
	str r2, [r0, #0x01c]
	ldr r2, =0x04008040
	str r2, [r0, #0x01c]

	ldr r2, =0x00005800
	str r2, [r0, #0x020]
	ldr r2, =0x00011117
	str r2, [r0, #0x818]
	str r2, [r1, #0x818]
	ldr r2, =0x0002556D
	str r2, [r0, #0x004]
	ldr r2, =0x00011006
	str r2, [r0, #0x404]
	ldr r2, =0x00000000
	str r2, [r0, #0x01c]
.endm

.macro imx6solosabresd_ddr_setting
	ldr r0, =IOMUXC_BASE_ADDR
	ldr r1, =0x000c0000
	str r1, [r0, #0x774]
	ldr r1, =0x00000000
	str r1, [r0, #0x754]

	ldr r1, =0x00000030
	str r1, [r0, #0x4ac]
	str r1, [r0, #0x4b0]
	str r1, [r0, #0x464]
	str r1, [r0, #0x490]
	str r1, [r0, #0x74c]
	str r1, [r0, #0x494]

	ldr r1, =0x00000000
	str r1, [r0, #0x4a0]

	ldr r1, =0x00000030
	str r1, [r0, #0x4b4]
	str r1, [r0, #0x4b8]
	str r1, [r0, #0x76c]

	ldr r1, =0x00020000
	str r1, [r0, #0x750]

	ldr r1, =0x00000030
	str r1, [r0, #0x4bc]
	str r1, [r0, #0x4c0]
	str r1, [r0, #0x4c4]
	str r1, [r0, #0x4c8]

	ldr r1, =0x00020000
	str r1, [r0, #0x760]

	ldr r1, =0x00000030
	str r1, [r0, #0x764]
	str r1, [r0, #0x770]
	str r1, [r0, #0x778]
	str r1, [r0, #0x77c]
	str r1, [r0, #0x470]
	str r1, [r0, #0x474]
	str r1, [r0, #0x478]
	str r1, [r0, #0x47c]

	ldr r0, =MMDC_P0_BASE_ADDR
	ldr r2, =0xa1390003
	str r2, [r0, #0x800]

	ldr r2, =0x001F001F
	str r2, [r0, #0x80c]
	str r2, [r0, #0x810]

	ldr r2, =0x42190219
	str r2, [r0, #0x83c]
	ldr r2, =0x017B0177
	str r2, [r0, #0x840]

	ldr r2, =0x4B4D4E4D
	str r2, [r0, #0x848]

	ldr r2, =0x3F3E2D36
	str r2, [r0, #0x850]

	ldr r2, =0x33333333
	str r2, [r0, #0x81c]
	str r2, [r0, #0x820]
	str r2, [r0, #0x824]
	str r2, [r0, #0x828]

	ldr r2, =0x00000800
	str r2, [r0, #0x8b8]

	ldr r2, =0x0002002D
	str r2, [r0, #0x004]
	ldr r2, =0x00333030
	str r2, [r0, #0x008]

	ldr r2, =0x3F435313
	str r2, [r0, #0x00c]
	ldr r2, =0xB66E8B63
	str r2, [r0, #0x010]

	ldr r2, =0x01FF00DB
	str r2, [r0, #0x014]
	ldr r2, =0x00001740
	str r2, [r0, #0x018]

	ldr r2, =0x00008000
	str r2, [r0, #0x01c]
	ldr r2, =0x000026d2
	str r2, [r0, #0x02c]
	ldr r2, =0x00431023
	str r2, [r0, #0x030]
	ldr r2, =0x00000017
	str r2, [r0, #0x040]

	ldr r2, =0x83190000
	str r2, [r0, #0x000]

	ldr r2, =0x04008032
	str r2, [r0, #0x01c]
	ldr r2, =0x00008033
	str r2, [r0, #0x01c]
	ldr r2, =0x00048031
	str r2, [r0, #0x01c]
	ldr r2, =0x05208030
	str r2, [r0, #0x01c]
	ldr r2, =0x04008040
	str r2, [r0, #0x01c]

	ldr r2, =0x00005800
	str r2, [r0, #0x020]
	ldr r2, =0x00011117
	str r2, [r0, #0x818]
	ldr r2, =0x0002556D
	str r2, [r0, #0x004]
	ldr r2, =0x00011006
	str r2, [r0, #0x404]
	ldr r2, =0x00000000
	str r2, [r0, #0x01c]
.endm
.macro imx6_clock_gating
	ldr r0, =CCM_BASE_ADDR
	ldr r1, =0x00C03F3F
	str r1, [r0, #0x068]
	ldr r1, =0x0030FC03
	str r1, [r0, #0x06c]
	ldr r1, =0x0FFFC000
	str r1, [r0, #0x070]
	ldr r1, =0x3FF00000
	str r1, [r0, #0x074]
	ldr r1, =0x00FFF300
	str r1, [r0, #0x078]
	ldr r1, =0x0F0000C3
	str r1, [r0, #0x07c]
	ldr r1, =0x000003FF
	str r1, [r0, #0x080]
.endm

.macro imx6_qos_setting
	ldr r0, =IOMUXC_BASE_ADDR
	ldr r1, =0xF00000CF
	str r1, [r0, #0x10]

	ldr r1, =0x007F007F
	str r1, [r0, #0x18]
	str r1, [r0, #0x1c]
.endm

.macro imx6_ddr_setting
#if defined (CONFIG_MX6SOLO)
	imx6solosabresd_ddr_setting
#elif defined (CONFIG_MX6DL)
	imx6dlsabresd_ddr_setting
#elif defined (CONFIG_MX6Q)
	imx6dqsabresd_ddr_setting
#else
	#error "SOC not configured"
#endif

.endm

/* include the common plugin code here */
#include <asm/arch/mx6_plugin.S>
