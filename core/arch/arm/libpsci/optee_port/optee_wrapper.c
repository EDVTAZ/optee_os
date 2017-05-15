/*
 * optee_wrapper.c
 *
 * Copyright (C) STMicroelectronics SA 2017
 * Author: etienne carriere <etienne.carriere@st.com> for STMicroelectronics.
 */

#include <libpsci/libpsci_optee.h>
#include <mm/core_mmu.h>
#include <tee/cache.h>

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
