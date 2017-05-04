/*
 * Copyright (c) 2015, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef ARM_H
#define ARM_H

#define MIDR_IMPL_MASK		0xff
#define MIDR_IMPL_SHIFT		0x18
#define MIDR_VAR_SHIFT		20
#define MIDR_VAR_BITS		4
#define MIDR_VAR_MASK		0xf
#define MIDR_REV_SHIFT		0
#define MIDR_REV_BITS		4
#define MIDR_REV_MASK		0xf
#define MIDR_PN_MASK		0xfff
#define MIDR_PN_SHIFT		0x4

#define MPIDR_CPU_MASK		MPIDR_AFFLVL_MASK
#define MPIDR_CLUSTER_MASK	MPIDR_AFFLVL_MASK << MPIDR_AFFINITY_BITS
#define MPIDR_AFFINITY_BITS	8
#define MPIDR_AFFLVL_MASK	0xff
#define MPIDR_AFFLVL_SHIFT	3
#define MPIDR_AFF0_SHIFT	0
#define MPIDR_AFF1_SHIFT	8
#define MPIDR_AFF2_SHIFT	16
#ifdef CFG_ARM32_core
#define MPIDR_AFFINITY_MASK	0x00ffffff
#else
#define MPIDR_AFF3_SHIFT	32
#define MPIDR_AFFINITY_MASK	0xff00ffffff
#endif
#define MPIDR_AFFLVL0		0
#define MPIDR_AFFLVL1		1
#define MPIDR_AFFLVL2		2
#define MPIDR_AFFLVL3		3
#define MPIDR_AFFLVL0_VAL(mpidr) \
		((mpidr >> MPIDR_AFF0_SHIFT) & MPIDR_AFFLVL_MASK)
#define MPIDR_AFFLVL1_VAL(mpidr) \
		((mpidr >> MPIDR_AFF1_SHIFT) & MPIDR_AFFLVL_MASK)
#define MPIDR_AFFLVL2_VAL(mpidr) \
		((mpidr >> MPIDR_AFF2_SHIFT) & MPIDR_AFFLVL_MASK)

#ifdef CFG_ARM64_core
#define MPIDR_AFFLVL3_VAL(mpidr) \
		((mpidr >> MPIDR_AFF3_SHIFT) & MPIDR_AFFLVL_MASK)
#endif

/*
 * The MPIDR_MAX_AFFLVL count starts from 0. Take care to
 * add one while using this macro to define array sizes.
 */
#define MPIDR_MAX_AFFLVL	2

/* OP-TEE legacy macros */
#define MPIDR_CLUSTER_SHIFT	MPIDR_AFFINITY_BITS


/* CLIDR definitions */
#define CLIDR_LOUIS_SHIFT	21
#define CLIDR_LOC_SHIFT		24
#define CLIDR_FIELD_WIDTH	3

/* CSSELR definitions */
#define CSSELR_LEVEL_SHIFT	1

/* CTR definitions */
#define CTR_CWG_SHIFT		24
#define CTR_CWG_MASK		0xf
#define CTR_ERG_SHIFT		20
#define CTR_ERG_MASK		0xf
#define CTR_DMINLINE_SHIFT	16
#define CTR_DMINLINE_WIDTH	4
#define CTR_DMINLINE_MASK	((1 << 4) - 1)
#define CTR_L1IP_SHIFT		14
#define CTR_L1IP_MASK		0x3
#define CTR_IMINLINE_SHIFT	0
#define CTR_IMINLINE_MASK	0xf

#define ARM32_CPSR_MODE_MASK	0x1f
#define ARM32_CPSR_MODE_USR	0x10
#define ARM32_CPSR_MODE_FIQ	0x11
#define ARM32_CPSR_MODE_IRQ	0x12
#define ARM32_CPSR_MODE_SVC	0x13
#define ARM32_CPSR_MODE_MON	0x16
#define ARM32_CPSR_MODE_ABT	0x17
#define ARM32_CPSR_MODE_UND	0x1b
#define ARM32_CPSR_MODE_SYS	0x1f

#define ARM32_CPSR_T		(1 << 5)
#define ARM32_CPSR_F_SHIFT	6
#define ARM32_CPSR_F		(1 << 6)
#define ARM32_CPSR_I		(1 << 7)
#define ARM32_CPSR_A		(1 << 8)
#define ARM32_CPSR_E		(1 << 9)
#define ARM32_CPSR_FIA		(ARM32_CPSR_F | ARM32_CPSR_I | ARM32_CPSR_A)
#define ARM32_CPSR_IT_MASK	(ARM32_CPSR_IT_MASK1 | ARM32_CPSR_IT_MASK2)
#define ARM32_CPSR_IT_MASK1	0x06000000
#define ARM32_CPSR_IT_MASK2	0x0000fc00


#ifdef ARM32
#include <arm32.h>
#endif

#ifdef ARM64
#include <arm64.h>
#endif

#endif /*ARM_H*/
