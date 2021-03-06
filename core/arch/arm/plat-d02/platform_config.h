/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2015, Linaro Limited
 */

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

/* Make stacks aligned to data cache line length */
#define STACK_ALIGNMENT		64

/* UART */
#define PERI_SUB_CTRL_ADDR	0x80000000
#define CONSOLE_UART_BASE       (PERI_SUB_CTRL_ADDR + 0x00300000)
#define CONSOLE_BAUDRATE	115200
#define CONSOLE_UART_CLK_IN_HZ	200000000

/* ALG sub-controller */
#define ALG_SC_BASE		0xD0000000
#define ALG_SC_REG_SIZE		0xF010

/* RNG */
#define RNG_BASE		0xD1010000
#define RNG_REG_SIZE		0x18

/*
 * HiSilicon D02 memory map
 *
 * Note: the physical address ranges below correspond to DRAM which is
 * non-secure by default. Therefore, the terms TZDRAM and TZSRAM may not
 * reflect the reality and only indicate areas that "would normally be"
 * secure DRAM and secure SRAM in a more complete implementation.
 * The memory map was defined like this for lack of better documentation.
 * It is good enough for development/testing purposes.
 *
 * CFG_WITH_PAGER=n
 *
 *  0x7FC0_0000                                  -
 *    Linux/other                                | DRAM1
 *  0x5180_0000                                  -
 *    TA RAM: 16 MiB                             |
 *  0x5080_0000                                  | TZDRAM
 *    TEE RAM: 4 MiB (TEE_RAM_VA_SIZE)		 |
 *  0x5040_0000 [TZDRAM_BASE, TEE_LOAD_ADDR]     -
 *    Shared memory: 4 MiB                       | SHMEM
 *  0x5000_0000                                  -
 *    Linux/other                                | DRAM0
 *  0x0000_0000 [DRAM0_BASE]                     -
 *
 * CFG_WITH_PAGER=y
 *
 *  0x7FC0_0000                                  -
 *    Linux/other                                | DRAM1
 *  0x5180_0000                                  -
 *    TA RAM: 20096 KiB (TZDRAM_SIZE)            | TZDRAM
 *  0x5046_0000                                  -
 *    TEE RAM: 384 KiB (TZSRAM_SIZE)             | TZSRAM
 *  0x5040_0000 [TZSRAM_BASE, TEE_LOAD_ADDR]     -
 *    Shared memory: 4 MiB                       | SHMEM
 *  0x5000_0000                                  -
 *    Linux/other                                | DRAM0
 *  0x0000_0000 [DRAM0_BASE]                     -
 */

#define DRAM0_BASE		0x00000000
#define DRAM0_SIZE		0x50000000

#define DRAM1_BASE		0x51800000
#define DRAM1_SIZE		0x2E400000

#ifdef CFG_WITH_PAGER

#define TZSRAM_BASE		0x50400000
#define TZSRAM_SIZE		CFG_CORE_TZSRAM_EMUL_SIZE

#define TZDRAM_BASE		0x50460000
#define TZDRAM_SIZE		(20096 * 1024)

#define TEE_RAM_START		TZSRAM_BASE
#define TEE_RAM_PH_SIZE		TZSRAM_SIZE
#define TA_RAM_START		ROUNDUP(TZDRAM_BASE, CORE_MMU_DEVICE_SIZE)
#define TA_RAM_SIZE		ROUNDDOWN(TZDRAM_SIZE, CORE_MMU_DEVICE_SIZE)

#define TEE_RAM_VA_SIZE		(2 * 1024 * 1024)

#else /* CFG_WITH_PAGER */

#define TZDRAM_BASE		0x50400000
#define TZDRAM_SIZE		(20 * 1024 * 1024)

#define TEE_RAM_START		TZDRAM_BASE
#define TEE_RAM_PH_SIZE		TEE_RAM_VA_SIZE
#define TA_RAM_START		ROUNDUP((TZDRAM_BASE + TEE_RAM_VA_SIZE), \
					CORE_MMU_DEVICE_SIZE)
#define TA_RAM_SIZE		ROUNDDOWN((TZDRAM_SIZE - TEE_RAM_VA_SIZE),\
					  CORE_MMU_DEVICE_SIZE)

#define TEE_RAM_VA_SIZE		(4 * 1024 * 1024)

#endif /* CFG_WITH_PAGER */

#define TEE_SHMEM_START		0x50000000
#define TEE_SHMEM_SIZE		(4 * 1024 * 1024)

#define CFG_TEE_CORE_NB_CORE	16

#define TEE_LOAD_ADDR		0x50400000

#endif /* PLATFORM_CONFIG_H */
