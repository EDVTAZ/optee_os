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

/*
 * Define a platform specific structure at start of coherent RAM.
 * This structure can be shared with the bootloader and hence must be located
 * at a known physical address.
 *
 * @warmboot_ep		Physical entry point for warm boot of OP-TEE.
 */
struct plat_coherent_data {
	paddr_t warmboot_ep;
};

static struct plat_coherent_data plat_coherent_data
	__section(".plat_private_coherent_structure");


#ifdef PSCI_PLATFORM_SYSTEM_COUNT
/* Generic current supports when PSCI_PLATFORM_SYSTEM_COUNT is defined */
#if (PLAT_MAX_PWR_LVL != 2) || \
	(PSCI_PLATFORM_SYSTEM_COUNT != 1) || \
	(PSCI_PLATFORM_CLUSTER_COUNT == 0) || \
	(PSCI_PLATFORM_CLUSTER_COUNT > 4)
#error "Invalid generic power domain tree configuration"
#endif

/*
 * PSCI_PLATFORM_SYSTEM_COUNT is expected to come with macros.
 * PSCI_PLATFORM_CLUSTER_COUNT defines the number of clusters.
 * PSCI_PLATFORM_CLUSTERx_CORE_COUNT defined for each cluster
 * with x from 0 to PSCI_PLATFORM_CLUSTER_COUNT - 1.
 * We expect at least 1 cluster. 4 clusters max supported.
 */
static const unsigned char default_power_domain_tree_desc[] = {
	/* No of children for the root node */
	PSCI_PLATFORM_SYSTEM_COUNT,
	/* No of children for the cluster node */
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

__weak int plat_validate_mpidr(u_register_t mpidr)
{
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

	return 0;
}
#endif /* PSCI_PLATFORM_SYSTEM_COUNT */

/*
 * Weak support for retrieving CNTFRQ configuration:
 * - primary boot core expects earlier boot stage loaded it.
 * - secondary boot cores get the value saved in RAM by primary core.
 */
static uint32_t shared_cntfrq __coherent;

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

/* Default consider 3 power domain levels at most: cpu, cluster and system */
#define PWR_LEVEL0	0
#define PWR_LEVEL1	1
#define PWR_LEVEL2	2

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
		 * It's possible to enter standby only on power level 0 (cpus)
		 * Ignore any other power level.
		 */
		if (pwr_lvl != PWR_LEVEL0)
			return PSCI_E_INVALID_PARAMS;

		req_state->pwr_domain_state[PWR_LEVEL0] = PWR_LOCAL_STATE_RET;
	} else {
		/*
		 * Targetting a power off: all levels shall swwitch off
		 */
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
#ifdef CFG_BOOT_SECONDARY_REQUEST
	size_t core_idx = get_core_pos_by_mpidr(mpidr);

	if (generic_boot_core_release(core_idx, ns_entry_addrs[core_idx]))
		return PSCI_E_INVALID_PARAMS;
	return PSCI_E_SUCCESS;
#else
	// TODO: wake secondary from its hold-in-pen */
	return PSCI_E_NOT_SUPPORTED;
#endif
}

static const plat_psci_ops_t plat_def_psci_pm_ops = {
	.cpu_standby = plat_def_cpu_standby,
	.pwr_domain_on = plat_def_pwr_domain_on,
	.validate_power_state = plat_def_validate_power_state,
};

__weak int plat_setup_psci_ops(paddr_t sec_entrypoint,
				const plat_psci_ops_t **psci_ops)
{
	assert(&plat_coherent_data == (void *)CFG_TEE_COHERENT_START);

	plat_coherent_data.warmboot_ep = (paddr_t)sec_entrypoint;
	*psci_ops = &plat_def_psci_pm_ops;

	return PSCI_E_SUCCESS;
}

