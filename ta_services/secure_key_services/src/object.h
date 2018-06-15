/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2017-2018, Linaro Limited
 */

#ifndef __SKS_OBJECT_H__
#define __SKS_OBJECT_H__

#include <sys/queue.h>
#include <tee_internal_api.h>

struct pkcs11_session;

struct sks_object {
	int foo;
};

/*
 * Entry function called from the SKS command parser
 */
uint32_t entry_destroy_object(uintptr_t teesess, TEE_Param *ctrl,
			      TEE_Param *in, TEE_Param *out);

uint32_t entry_find_objects_init(uintptr_t teesess, TEE_Param *ctrl,
				 TEE_Param *in, TEE_Param *out);

uint32_t entry_find_objects(uintptr_t teesess, TEE_Param *ctrl,
			    TEE_Param *in, TEE_Param *out);

uint32_t entry_find_objects_final(uintptr_t teesess, TEE_Param *ctrl,
				  TEE_Param *in, TEE_Param *out);

#endif /*__SKS_OBJECT_H__*/
