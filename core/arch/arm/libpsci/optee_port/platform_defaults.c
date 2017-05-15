/*
 * Copyright (c) 2013-2015, ARM Limited and Contributors. All rights reserved.
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

#include <compiler.h>
#include <kernel/generic_boot.h>
#include <kernel/misc.h>
#include <libpsci/libpsci_optee.h>
#include <libpsci/psci.h>

#ifdef PSCI_PLATFORM_CLUSTER_COUNT
/*
 * PSCI_PLATFORM_CLUSTER_COUNT is expected to come with macros
 * PSCI_PLATFORM_CLUSTERx_CORE_COUNT defined for each cluster
 * with x from 0 to PSCI_PLATFORM_CLUSTER_COUNT - 1.
 * We expect at least 1 cluster. 4 clusters max supported.
 */
static const unsigned char default_power_domain_tree_desc[] = {
	/* No of children for the root node */
	PSCI_PLATFORM_CLUSTER_COUNT,
	/* No of children for the first cluster node */
	PSCI_PLATFORM_CLUSTER0_CORE_COUNT,
	/* No of children for the second cluster node */
#if PSCI_PLATFORM_CLUSTER_COUNT > 1
	PSCI_PLATFORM_CLUSTER1_CORE_COUNT,
#if PSCI_PLATFORM_CLUSTER_COUNT > 2
	PSCI_PLATFORM_CLUSTER2_CORE_COUNT,
#if PSCI_PLATFORM_CLUSTER_COUNT > 3
	PSCI_PLATFORM_CLUSTER3_CORE_COUNT,
#endif
#endif
#endif
};

__weak const unsigned char *plat_get_power_domain_tree_desc(void)
{
	return default_power_domain_tree_desc;
}
#endif

/* TODO: replace 'plat_my_core_pos' with 'get_core_pos' in PSCI sequences */
unsigned int plat_my_core_pos(void)
{
	return get_core_pos();
}

/*  TODO: are the below verif usefull before calling get_core_pos_by_mpidr() */
int plat_core_pos_by_mpidr(u_register_t mpidr)
{
#ifdef PSCI_PLATFORM_CLUSTER_COUNT
	unsigned int cluster_id;
	unsigned int cpu_id;

	mpidr &= MPIDR_AFFINITY_MASK;

	if (mpidr & ~(MPIDR_CLUSTER_MASK | MPIDR_CPU_MASK))
		return -1;

	cluster_id = (mpidr >> MPIDR_AFF1_SHIFT) & MPIDR_AFFLVL_MASK;
	cpu_id = (mpidr >> MPIDR_AFF0_SHIFT) & MPIDR_AFFLVL_MASK;

	if (cluster_id >= PSCI_PLATFORM_CLUSTER_COUNT)
		return -1;

	if (cpu_id >= default_power_domain_tree_desc[cluster_id + 1])
		return -1;
#endif

	return get_core_pos_by_mpidr(mpidr);
}

uint32_t shared_cntfrq __coherent_ram;

__weak unsigned int plat_get_syscnt_freq2(void)
{
	uint32_t cntfrq;

	if (!plat_my_core_pos()) {
		/* Assume early boot stage has set CNTFRQ */
		cntfrq = read_cntfrq();
		shared_cntfrq = cntfrq;
	} else {
		/* read CNTFRQ value from coherent RAM */
		cntfrq = shared_cntfrq;
	}
	return (unsigned int)cntfrq;
}


/*******************************************************************************
 * TEE power handlers called from PSCI sequence through the registered structure
 * struct spd_pm_ops. They to be given a chance to perform any OPTEE bookkeeping
 * before PSCI executes a power management operation.
 *
 * TODO: change the PSCI sequence to remove this useless inderection.
 ******************************************************************************/

static void opteed_cpu_on_handler(uint32_t target_cpu __unused)
{
	/* nothing required */
}

static int32_t opteed_cpu_off_handler(uint32_t unused __unused)
{
	const struct thread_handlers *hdl = generic_boot_get_handlers();


	if (hdl->cpu_off)
		return hdl->cpu_off(0, 0);
	return 0;
}

static void opteed_cpu_suspend_handler(uint32_t max_off_pwrlvl __unused)
{
	const struct thread_handlers *hdl = generic_boot_get_handlers();

	if (hdl->cpu_suspend)
		hdl->cpu_suspend(0, 0);
}

static void opteed_cpu_on_finish_handler(uint32_t unused __unused)
{
	const struct thread_handlers *hdl = generic_boot_get_handlers();

	if (hdl->cpu_on)
		hdl->cpu_on(0, 0);
}

static void opteed_cpu_suspend_finish_handler(uint32_t max_off_pwrlvl __unused)
{
	const struct thread_handlers *hdl = generic_boot_get_handlers();

	if (hdl->cpu_resume)
		hdl->cpu_resume(0, 0);
}

static int32_t opteed_cpu_migrate_info(uint32_t *resident_cpu __unused)
{
#if (CFG_TEE_CORE_NB_CORE == 1)
	return PSCI_TOS_NOT_UP_MIG_CAP;
#else
	return PSCI_TOS_NOT_PRESENT_MP;
#endif
}

static void opteed_system_off(void)
{
	const struct thread_handlers *hdl = generic_boot_get_handlers();

	if (hdl->system_off)
		hdl->system_off(0, 0);
}

static void opteed_system_reset(void)
{
	const struct thread_handlers *hdl = generic_boot_get_handlers();

	if (hdl->system_reset)
		hdl->system_reset(0, 0);
}

const spd_pm_ops_t opteed_pm = {
	.svc_on = opteed_cpu_on_handler,
	.svc_off = opteed_cpu_off_handler,
	.svc_suspend = opteed_cpu_suspend_handler,
	.svc_on_finish = opteed_cpu_on_finish_handler,
	.svc_suspend_finish = opteed_cpu_suspend_finish_handler,
	.svc_migrate = NULL,
	.svc_migrate_info = opteed_cpu_migrate_info,
	.svc_system_off = opteed_system_off,
	.svc_system_reset = opteed_system_reset,
};

/*
 * Platform sequences for power sequence
 * -------------------------------------
 *
 *  Default platform settings can only support CPU standby on WFI.
 *  Al other handlers do panic. Platform requiring these power sequence
 *  must implement their own and register through plat_setup_psci_ops().
 */

/* Default local power states: Running, retention or Off */
#define PWR_LOCAL_STATE_RUN	0
#define PWR_LOCAL_STATE_RET	1
#define PWR_LOCAL_STATE_OFF	2

/* Default consider 3 power domain levels: cpu, cluster and system */
#define PWR_LEVEL0	0
#define PWR_LEVEL1	1
#define PWR_LEVEL2	2
CASSERT(PLAT_MAX_PWR_LVL >= PWR_LOCAL_STATE_OFF, \
		invalid_platform_max_power_level);

static int plat_def_validate_power_state(unsigned int power_state,
					psci_power_state_t *req_state)
{
	int pstate = psci_get_pstate_type(power_state);
	int pwr_lvl = psci_get_pstate_pwrlvl(power_state);
	int i;

	assert(req_state);

	if (pwr_lvl > PLAT_MAX_PWR_LVL)
		return PSCI_E_INVALID_PARAMS;

	/* Sanity check the requested state */
	if (pstate == PSTATE_TYPE_STANDBY) {
		/*
		 * It's possible to enter standby only on power level 0
		 * Ignore any other power level.
		 */
		if (pwr_lvl != PWR_LEVEL0)
			return PSCI_E_INVALID_PARAMS;

		req_state->pwr_domain_state[PWR_LEVEL0] = PWR_LOCAL_STATE_RET;
	} else {
		for (i = 0; i <= pwr_lvl; i++)
			req_state->pwr_domain_state[i] = PWR_LOCAL_STATE_OFF;
	}

	/*
	 * We expect the 'state id' to be zero.
	 */
	if (psci_get_pstate_id(power_state))
		return PSCI_E_INVALID_PARAMS;

	return PSCI_E_SUCCESS;
}

/* Platform handler called when a CPU is about to enter standby */
void plat_def_cpu_standby(plat_local_state_t cpu_state __maybe_unused)
{
	assert(cpu_state == PLAT_LOCAL_STATE_RET);

	/*
	 * Enter standby state
	 * dsb is good practice before using wfi to enter low power states
	 */
	dsb();
	wfi();
}

/* Power on a domain. mpidr determines the CPU to be turned on */
int plat_def_pwr_domain_on(u_register_t mpidr __maybe_unused)
{
	// TODO: wake secondary from its hold-in-pen */
	return !PSCI_E_SUCCESS;
}

static const plat_psci_ops_t plat_def_psci_pm_ops = {
	.cpu_standby = plat_def_cpu_standby,
	.pwr_domain_on = plat_def_pwr_domain_on,
	.validate_power_state = plat_def_validate_power_state,
};

__weak int plat_setup_psci_ops(uintptr_t sec_entrypoint __unused,
				const plat_psci_ops_t **psci_ops)
{
	*psci_ops = &plat_def_psci_pm_ops;

	return 0;
}

