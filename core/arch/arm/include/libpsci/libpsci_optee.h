/*
 * Copyright (c) 2017, Linaro Limited
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

#ifndef LIBPSCI_OPTEE_H
#define LIBPSCI_OPTEE_H

#include <arm32.h>
#include <compiler.h>
#include <libpsci/libpsci_optee_config.h>

#define plat_get_my_stack_size	optee_get_tmp_stack_size
#define plat_get_my_stack	optee_get_tmp_stack_top

#define SCR_NS_BIT	SCR_NS
#define SCR_IRQ_BIT	SCR_IRQ
#define SCR_FIQ_BIT	SCR_FIQ
#define SCR_ABT_BIT	SCR_EA
#define SCR_HCE_BIT	SCR_HCE

#define SCTLR_EE_BIT	SCTLR_EE
#define SCTLR_C_BIT	SCTLR_C

#ifndef __ASSEMBLY__
#include <kernel/panic.h>
#include <kernel/spinlock.h>
#include <sm/optee_smc.h>
#include <string.h>
#include <sys/types.h>
#include <tee_api_types.h>		/* TEE_Result */
#include <tee/cache.h>			/* xxx_dcache_range() */
#include <trace.h>

#define	__dead2			__noreturn

#define FUNCID_TYPE_SHIFT		31
#define FUNCID_CC_SHIFT			30
#define FUNCID_OEN_SHIFT		OPTEE_SMC_OWNER_SHIFT
#define FUNCID_NUM_SHIFT		0

#define FUNCID_TYPE_MASK		0x1
#define FUNCID_CC_MASK			0x1
#define FUNCID_OEN_MASK			OPTEE_SMC_OWNER_MASK
#define FUNCID_NUM_MASK			OPTEE_SMC_FUNC_MASK

#define SMC_64				1
#define SMC_32				0
#define SMC_UNK				OPTEE_SMC_RETURN_UNKNOWN_FUNCTION
#define SMC_TYPE_FAST			1ull
#define SMC_TYPE_STD			0
#define SMC_PREEMPTED			0xfffffffe

#if (OPTEE_SMC_64 != (SMC_64 << FUNCID_CC_SHIFT)) || \
	(OPTEE_SMC_FAST_CALL != (SMC_TYPE_FAST << FUNCID_TYPE_SHIFT))
#error "Issue with the SMC ABI"
#endif

#define is_valid_fast_smc(id)		!!OPTEE_SMC_IS_FAST_CALL(id)
#define is_std_svc_call(id)		(OPTEE_SMC_OWNER_NUM(id) == \
						OPTEE_SMC_OWNER_STANDARD)
/* Stub libpsci arguments */
#define VERIFY_PSCI_LIB_ARGS_V1(a)	1

typedef unsigned int u_register_t;
typedef unsigned int spinlock_t;

#include <libpsci/platform_apis.h>

#define ERROR(...)		EMSG(__VA_ARGS__)
#define INFO(...)		IMSG(__VA_ARGS__)
#define WARN(...)		IMSG("Warning" __VA_ARGS__)

#define LOG_LEVEL		TRACE_LEVEL
#define LOG_LEVEL_INFO		TRACE_INFO

#define CASSERT(cond, msg)	typedef char msg[(cond) ? 1 : -1] __unused

static inline void print_errata_status(void)
{
}

static inline void spin_lock(spinlock_t *lock)
{
	cpu_spin_lock((unsigned int *)lock);
}
static inline void spin_unlock(spinlock_t *lock)
{
	cpu_spin_unlock((unsigned int *)lock);
}

static inline uint32_t read_mpidr_el1(void)
{
	return read_mpidr();
}

static inline uint32_t read_isr_el1(void)
{
	return read_isr();
}

static inline void write_cntfrq_el0(uint32_t cntfrq)
{
	write_cntfrq(cntfrq);
}

static inline void dsbish(void)
{
	dsb();
}

static inline void zeromem(void *addr, size_t size)
{
	memset(addr, 0, size);
}

#endif /* __ASSEMBLY__ */

#endif /* LIBPSCI_OPTEE_H */
