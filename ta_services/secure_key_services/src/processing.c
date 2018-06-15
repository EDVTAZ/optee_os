// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2017-2018, Linaro Limited
 */

#include <assert.h>
#include <sks_ta.h>
#include <string.h>
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <util.h>

#include "object.h"
#include "processing.h"
#include "serializer.h"
#include "sks_helpers.h"

uint32_t entry_import_object(uintptr_t teesess __unused,
			     TEE_Param *ctrl, TEE_Param *in, TEE_Param *out)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;
	struct sks_object_head *template = NULL;

	if (!ctrl || in || !out)
		return SKS_BAD_PARAM;

	if (out->memref.size < sizeof(uint32_t))
		return SKS_SHORT_BUFFER;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	rv = serialargs_alloc_get_attributes(&ctrlargs, &template);
	if (rv)
		return rv;

	// Release template if required on completion
	return SKS_NOT_IMPLEMENTED;
}

/*
 * ctrl = [session-handle][key-handle][mechanism-parameters]
 * in = none
 * out = none
 */
uint32_t entry_cipher_init(uintptr_t teesess __unused, TEE_Param *ctrl,
			   TEE_Param *in, TEE_Param *out,
			   int decrypt __unused)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;
	uint32_t key_handle;
	struct sks_attr_head *proc_params = NULL;

	if (!ctrl || in || out)
		return SKS_BAD_PARAM;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	rv = serialargs_get(&ctrlargs, &key_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	rv = serialargs_alloc_get_one_attribute(&ctrlargs, &proc_params);
	if (rv)
		return rv;

	// Release proc_param if required on completion
	return SKS_NOT_IMPLEMENTED;
}

/*
 * ctrl = [session-handle]
 * in = data buffer
 * out = data buffer
 */
uint32_t entry_cipher_update(uintptr_t teesess __unused, TEE_Param *ctrl,
			     TEE_Param *in __unused, TEE_Param *out __unused,
			     int decrypt __unused)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;

	/* May or may not provide input and/or output data */
	if (!ctrl)
		return SKS_BAD_PARAM;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	return SKS_NOT_IMPLEMENTED;
}

/*
 * ctrl = [session-handle]
 * in = none
 * out = data buffer
 */
uint32_t entry_cipher_final(uintptr_t teesess __unused, TEE_Param *ctrl,
			    TEE_Param *in __unused, TEE_Param *out __unused,
			    int decrypt __unused)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;

	/* May or may not provide input and/or output data */
	if (!ctrl)
		return SKS_BAD_PARAM;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	return SKS_NOT_IMPLEMENTED;
}

uint32_t entry_generate_object(uintptr_t teesess __unused,
			       TEE_Param *ctrl, TEE_Param *in, TEE_Param *out)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;
	struct sks_attr_head *proc_params = NULL;
	struct sks_object_head *template = NULL;

	if (!ctrl || in || !out)
		return SKS_BAD_PARAM;

	if (out->memref.size < sizeof(uint32_t))
		return SKS_SHORT_BUFFER;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	rv = serialargs_alloc_get_one_attribute(&ctrlargs, &proc_params);
	if (rv)
		return rv;

	rv = serialargs_alloc_get_attributes(&ctrlargs, &template);
	if (rv)
		goto bail;

bail:
	// Release proc_param and template if required on completion
	return SKS_NOT_IMPLEMENTED;
}

/*
 * ctrl = [session-handle][key-handle][mechanism-parameters]
 * in = none
 * out = none
 */
uint32_t entry_signverify_init(uintptr_t teesess __unused, TEE_Param *ctrl,
				TEE_Param *in, TEE_Param *out,
				int sign __unused)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;
	uint32_t key_handle;
	struct sks_attr_head *proc_params = NULL;

	if (!ctrl || in || out)
		return SKS_BAD_PARAM;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	rv = serialargs_get(&ctrlargs, &key_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	rv = serialargs_alloc_get_one_attribute(&ctrlargs, &proc_params);
	if (rv)
		return rv;

	// Release proc_param if required on completion
	return SKS_NOT_IMPLEMENTED;
}

/*
 * ctrl = [session-handle]
 * in = input data
 * out = none
 */
uint32_t entry_signverify_update(uintptr_t teesess __unused,
				 TEE_Param *ctrl,
				 TEE_Param *in __unused,
				 TEE_Param *out __unused,
				 int sign __unused)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;

	/* May or may not provide input and/or output data */
	if (!ctrl)
		return SKS_BAD_PARAM;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	return SKS_NOT_IMPLEMENTED;
}

/*
 * ctrl = [session-handle]
 * in = none
 * out = data buffer
 */
uint32_t entry_signverify_final(uintptr_t teesess __unused, TEE_Param *ctrl,
				TEE_Param *in, TEE_Param *out,
				int sign __unused)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;

	if (in || !ctrl || !out)
		return SKS_BAD_PARAM;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	return SKS_NOT_IMPLEMENTED;
}

uint32_t entry_derive(uintptr_t teesess __unused,
			TEE_Param *ctrl, TEE_Param *in, TEE_Param *out)
{
	uint32_t rv;
	struct serialargs ctrlargs;
	uint32_t session_handle;
	struct sks_attr_head *proc_params = NULL;
	uint32_t parent_handle;
	struct sks_object_head *template = NULL;

	if (!ctrl || in || !out)
		return SKS_BAD_PARAM;

	if (out->memref.size < sizeof(uint32_t))
		return SKS_SHORT_BUFFER;

	serialargs_init(&ctrlargs, ctrl->memref.buffer, ctrl->memref.size);

	rv = serialargs_get(&ctrlargs, &session_handle, sizeof(uint32_t));
	if (rv)
		return rv;

	rv = serialargs_alloc_get_one_attribute(&ctrlargs, &proc_params);
		return rv;

	rv = serialargs_get(&ctrlargs, &parent_handle, sizeof(uint32_t));
	if (rv)
		goto bail;

	rv = serialargs_alloc_get_attributes(&ctrlargs, &template);
	if (rv)
		goto bail;

bail:
	// Release proc_param and template if required on completion
	return SKS_NOT_IMPLEMENTED;
}
