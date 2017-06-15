/*
 * Copyright (c) 2015-2016, ARM Limited and Contributors. All rights reserved.
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

#ifndef LIBPSCI_PLATFORM_APIS_H
#define LIBPSCI_PLATFORM_APIS_H

#include <compiler.h>
#include <libpsci/psci.h>

#ifndef __ASSEMBLY__

/*
 * Mandatory PSCI functions
 */

/* Platform initialisations for the libpsci */
int plat_setup_psci_ops(uintptr_t sec_entrypoint,
			const struct plat_psci_ops **psci_ops);

/* Get the power domain tree from the platform */
const unsigned char *plat_get_power_domain_tree_desc(void);

/* Convert mpidr value into a cpu index */
unsigned int plat_my_core_pos(void);
int plat_core_pos_by_mpidr(u_register_t mpidr);

/* Validate an mpidr value based on power domain tree description */
int plat_validate_mpidr(u_register_t mpidr);

/*
 * Optional PSCI functions
 */
void plat_psci_stat_accounting_start(const psci_power_state_t *state_info);
void plat_psci_stat_accounting_stop(const psci_power_state_t *state_info);
u_register_t plat_psci_stat_get_residency(unsigned int lvl,
			const psci_power_state_t *state_info,
			int last_cpu_index);
plat_local_state_t plat_get_target_pwr_state(unsigned int lvl,
			const plat_local_state_t *states,
			unsigned int ncpu);

/*
 * Plaform default handler provided by OP-TEE for PSCI sequences.
 */
void plat_def_cpu_standby(plat_local_state_t cpu_state);
int plat_def_pwr_domain_on(u_register_t mpidr);

#endif /* __ASSEMBLY__ */

#endif /* LIBPSCI_PLATFORM_APIS_H */

