// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2017-2018, Linaro Limited
 */

#include <inttypes.h>
#include <string_ext.h>
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include "handle.h"
#include "object.h"
#include "processing.h"
#include "serializer.h"
#include "sks_helpers.h"

/*
 * Destroy an object
 *
 * @session - session requesting object destruction
 * @obj - object to destroy
 * @session_only - Destroy only the session resources
 */
uint32_t destroy_object(struct pkcs11_session *session __unused,
			  struct sks_object *obj __unused,
			  bool session_only __unused)
{
	return SKS_NOT_IMPLEMENTED;
}

uint32_t entry_destroy_object(uintptr_t __unused teesess, TEE_Param *ctrl,
				TEE_Param *in, TEE_Param *out)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;
	uint32_t object_handle;

	if (!ctrl || in || out)
		return SKS_BAD_PARAM;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	rv = serialargs_get(&ctrlargs, &object_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	return SKS_NOT_IMPLEMENTED;
}

/*
 * Entry for command SKS_CMD_FIND_OBJECTS_INIT
 */
uint32_t entry_find_objects_init(uintptr_t teesess __unused, TEE_Param *ctrl,
				 TEE_Param *in, TEE_Param *out)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;
	struct sks_object_head *template = NULL;

	if (!ctrl || in || out)
		return SKS_BAD_PARAM;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	rv = serialargs_alloc_get_attributes(&ctrlargs, &template);
	if (rv)
		return rv;

	return SKS_NOT_IMPLEMENTED;
}

/*
 * Entry for command SKS_CMD_FIND_OBJECTS
 */
uint32_t entry_find_objects(uintptr_t teesess __unused, TEE_Param *ctrl,
			    TEE_Param *in, TEE_Param *out)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;

	if (!ctrl || in || !out)
		return SKS_BAD_PARAM;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	return SKS_NOT_IMPLEMENTED;
}

/*
 * Entry for command SKS_CMD_FIND_OBJECTS_FINAL
 */
uint32_t entry_find_objects_final(uintptr_t teesess __unused, TEE_Param *ctrl,
				  TEE_Param *in, TEE_Param *out)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;

	if (!ctrl || in || out)
		return SKS_BAD_PARAM;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	return SKS_NOT_IMPLEMENTED;
}
