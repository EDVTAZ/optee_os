/*
 * optee_wrapper.c
 *
 * Copyright (C) STMicroelectronics SA 2017
 * Author: etienne carriere <etienne.carriere@st.com> for STMicroelectronics.
 */

#include <initcall.h>
#include <kernel/generic_boot.h>
#include <kernel/misc.h>
#include <libpsci/libpsci_optee.h>
#include <mm/core_mmu.h>
#include <tee/cache.h>
#include <tee_api_types.h>

static TEE_Result libpsci_init(void)
{
	const psci_lib_args_t arg = {
		.warmboot_ep = (paddr_t)warmboot_entrypoint,
	};

	if (psci_setup(&arg))
		panic("libpsci init failure");

	return TEE_SUCCESS;
}
service_init(libpsci_init);

unsigned int plat_my_core_pos(void)
{
	return get_core_pos();
}

int plat_core_pos_by_mpidr(u_register_t mpidr)
{
	if (plat_validate_mpidr(mpidr))
		return -1;

	return get_core_pos_by_mpidr(mpidr);
}

void flush_dcache_range(unsigned long int va, size_t len)
{
	cache_operation(TEE_CACHEFLUSH, (void *)(va), len);
}

void inv_dcache_range(unsigned long int va, size_t len)
{
	cache_operation(TEE_CACHEINVALIDATE, (void *)(va), len);
}

void flush_inner_cache(void)
{
	(void)cache_op_inner(DCACHE_CLEAN_INV, NULL, 0);
}

void flush_outer_cache(void)
{
	(void)cache_op_outer(DCACHE_CLEAN_INV, 0, 0);
}
