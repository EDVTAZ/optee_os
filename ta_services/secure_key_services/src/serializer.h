/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2017-2018, Linaro Limited
 */

#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include <sks_ta.h>
#include <stdint.h>
#include <stddef.h>

/*
 * Util routines for serializes unformated arguments in a client memref
 */
struct serialargs {
	char *start;
	char *next;
	size_t size;
};

void serialargs_init(struct serialargs *args, void *in, size_t size);

uint32_t serialargs_get(struct serialargs *args, void *out, size_t sz);

uint32_t serialargs_get_ptr(struct serialargs *args, void **out, size_t size);

uint32_t serialargs_alloc_get_one_attribute(struct serialargs *args,
					    struct sks_attr_head **out);

uint32_t serialargs_alloc_get_attributes(struct serialargs *args,
					 struct sks_object_head **out);

uint32_t serialargs_alloc_and_get(struct serialargs *args,
				  void **out, size_t size);

#endif /*__SERIALIZER_H__*/

