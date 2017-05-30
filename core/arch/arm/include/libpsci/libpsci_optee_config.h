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

#ifndef LIBPSCI_OPTEE_CONFIG_H
#define LIBPSCI_OPTEE_CONFIG_H

/* core relies on ASM while libpsci relies on __ASSEMBLY__ */
#if defined(ASM) && !defined(__ASSEMBLY__)
#define __ASSEMBLY__
#endif

#include <platform_config.h>

/*
 * Platform description macros
 */
#define AARCH32

#define PLATFORM_CORE_COUNT		CFG_TEE_CORE_NB_CORE

#ifndef CACHE_WRITEBACK_SHIFT
/* Cortex-A9/A5 shall override CACHE_WRITEBACK_SHIFT to 5 */
#define CACHE_WRITEBACK_SHIFT		6
#endif
#define CACHE_WRITEBACK_GRANULE		(1 << CACHE_WRITEBACK_SHIFT)

/*
 * PSCI library generic directives and support
 */

#define ASM_ASSERTION				0
#define CRASH_REPORTING				0
#define ENABLE_PLAT_COMPAT			0

/*
 * No HW assistance from CPU instruction set,
 * force enabling coherent memory support
 */
#define HW_ASSISTED_COHERENCY			0
#define USE_COHERENT_MEM			1

/* Disable libpsci extended state IDs */
#define PSCI_EXTENDED_STATE_ID			0

/* Performance monitoring are not (yet) supported */
#define ENABLE_RUNTIME_INSTRUMENTATION		0
#define ENABLE_PSCI_STAT			0
#define ENABLE_PMF				0

#endif /* LIBPSCI_OPTEE_CONFIG_H */
