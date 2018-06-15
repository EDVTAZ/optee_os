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
	LIST_ENTRY(sks_object) link;
	void *session_owner;
};

LIST_HEAD(object_list, sks_object);

/*
 * destroy_object - destroy an SKS object
 *
 * @session - session requesting object destruction
 * @object - reference to the sks object
 * @session_object_only - true is only session object shall be destroyed
 */
uint32_t destroy_object(struct pkcs11_session *session,
			struct sks_object *object,
			bool session_object_only);

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
