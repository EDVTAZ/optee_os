/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
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

#include <arm32_macros.S>
#include <arm32.h>
#include <asm.S>
#include <libpsci/libpsci_optee.h>

	.weak plat_disable_acp
FUNC plat_disable_acp , :
	bx	lr
END_FUNC plat_disable_acp

	/* ---------------------------------------------
	 * void arch_disable_smp(void)
	 *
	 * Disable intra-cluster coherency
	 * Clobbers: r0-r1
	 * ---------------------------------------------
	 */
FUNC arch_disable_smp , :
	read_actlr r0
	bic     r0, r0, #ACTLR_SMP
	write_actlr r0
	isb
	dsb	sy
	bx	lr
END_FUNC arch_disable_smp

	/* -------------------------------------------------
	 * void arch_enable_smp(void)
	 *
	 * Enable intra-cluster coherency
	 * Clobbers: r0-r1
	 * -------------------------------------------------
	 */
FUNC arch_enable_smp , :
	/* Enable SMP bit */
	read_actlr r0
	orr     r0, r0, #ACTLR_SMP
	write_actlr r0
	isb
	bx	lr
END_FUNC arch_enable_smp

	/* ----------------------------------------------------
	 * The CPU Ops core power down function for Cortex-A32.
	 * Clobbers: r0-r3
	 * ----------------------------------------------------
	 */
FUNC arch_cpu_power_down , :
	/* r12 is pushed to meet the 8 byte stack alignment requirement */
	push	{r12, lr}

	/* Assert if cache is enabled */
#if ASM_ASSERTION
	read_sctlr  r0
	tst	r0, #SCTLR_C_BIT
	ASM_ASSERT(eq)
#endif

	/* ---------------------------------------------
	 * Flush L1 caches.
	 * ---------------------------------------------
	 */
	bl	flush_inner_cache

	/* ---------------------------------------------
	 * Come out of intra cluster coherency
	 * ---------------------------------------------
	 */
	bl	arch_disable_smp

	pop	{r12, pc}
END_FUNC arch_cpu_power_down

	/* -------------------------------------------------------
	 * The CPU Ops cluster power down function for Cortex-A32.
	 * Clobbers: r0-r3
	 * -------------------------------------------------------
	 */
FUNC arch_cluster_power_down , :
	/* r12 is pushed to meet the 8 byte stack alignment requirement */
	push	{r12, lr}

	/* Assert if cache is enabled */
#if ASM_ASSERTION
	read_sctlr  r0
	tst	r0, #SCTLR_C_BIT
	ASM_ASSERT(eq)
#endif

	/* ---------------------------------------------
	 * Flush L1 caches.
	 * ---------------------------------------------
	 */
	bl	flush_inner_cache

	/* ---------------------------------------------
	 * Disable the optional ACP.
	 * ---------------------------------------------
	 */
	bl	plat_disable_acp

	/* ---------------------------------------------
	 * Flush L2 caches.
	 * ---------------------------------------------
	 */
	bl	flush_outer_cache

	/* ---------------------------------------------
	 * Come out of intra cluster coherency
	 * ---------------------------------------------
	 */
	bl	arch_disable_smp

	pop	{r12, pc}
END_FUNC arch_cluster_power_down
