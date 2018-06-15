// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2017-2018, Linaro Limited
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <string_ext.h>
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <trace.h>

#include "serializer.h"
#include "sks_helpers.h"

/*
 * Util routines for serializes unformatted arguments in a client memref
 */
void serialargs_init(struct serialargs *args, void *in, size_t size)
{
	args->start = in;
	args->next = in;
	args->size = size;
}

uint32_t serialargs_get(struct serialargs *args, void *out, size_t size)
{
	if (args->next + size > args->start + args->size) {
		EMSG("arg too short: full %zd, remain %zd, expect %zd",
		     args->size, args->size - (args->next - args->start), size);
		return SKS_BAD_PARAM;
	}

	TEE_MemMove(out, args->next, size);

	args->next += size;

	return SKS_OK;
}

uint32_t serialargs_alloc_and_get(struct serialargs *args,
				  void **out, size_t size)
{
	void *ptr;

	if (!size) {
		*out = NULL;
		return SKS_OK;
	}

	if (args->next + size > args->start + args->size) {
		EMSG("arg too short: full %zd, remain %zd, expect %zd",
		     args->size, args->size - (args->next - args->start), size);
		return SKS_BAD_PARAM;
	}

	ptr = TEE_Malloc(size, 0);
	if (!ptr)
		return SKS_MEMORY;

	TEE_MemMove(ptr, args->next, size);

	args->next += size;
	*out = ptr;

	return SKS_OK;
}

uint32_t serialargs_get_ptr(struct serialargs *args, void **out, size_t size)
{
	void *ptr = args->next;

	if (!size) {
		*out = NULL;
		return SKS_OK;
	}

	if (args->next + size > args->start + args->size) {
		EMSG("arg too short: full %zd, remain %zd, expect %zd",
		     args->size, args->size - (args->next - args->start), size);
		return SKS_BAD_PARAM;
	}

	args->next += size;
	*out = ptr;

	return SKS_OK;
}

uint32_t serialargs_alloc_get_one_attribute(struct serialargs *args __unused,
					    struct sks_attr_head **out __unused)
{
	return SKS_NOT_IMPLEMENTED;
}

uint32_t serialargs_alloc_get_attributes(struct serialargs *args __unused,
					 struct sks_object_head **out __unused)
{
	return SKS_NOT_IMPLEMENTED;
}
