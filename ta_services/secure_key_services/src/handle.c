// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2014-2018, Linaro Limited
 */

#include <assert.h>
#include <stdlib.h>
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <user_ta_header_defines.h>

#include "handle.h"

/* From user_ta_header.c, built within TA */
extern uint8_t ta_heap[];

/*
 * Define the initial capacity of the database. It should be a low number
 * multiple of 2 since some databases a likely to only use a few handles.
 * Since the algorithm is to doubles up when growing it shouldn't cause a
 * noticable overhead on large databases.
 */
#define HANDLE_DB_INITIAL_MAX_PTRS	4

void handle_db_destroy(struct handle_db *db)
{
	if (!db)
		return;

	TEE_Free(db->ptrs);
	db->ptrs = NULL;
	db->max_ptrs = 0;
}

static uint32_t ptr2hdl(void *ptr)
{
	/* Add 1 to insure 0 relates to invalid pointer */
	size_t hld = (uintptr_t)ptr - (uintptr_t)ta_heap + 1;

	assert(hld < INT32_MAX);
	return (uint32_t)hld;
}

static void *hdl2ptr(uint32_t hdl)
{
	uint8_t *ptr = ta_heap + hdl - 1;

	if (!hdl)
		return NULL;

	return ptr;
}

uint32_t handle_get(struct handle_db *db, void *ptr)
{
	uint32_t n;
	void *p;
	uint32_t new_max_ptrs;

	COMPILE_TIME_ASSERT((uint64_t)TA_DATA_SIZE < (uint64_t)UINT32_MAX);
	assert(ptr >= (void *)ta_heap && ptr < (void *)&ta_heap[TA_DATA_SIZE]);

	if (!db || !ptr)
		return 0;

	/* Try to find an empty location */
	for (n = 0; n < db->max_ptrs; n++) {
		if (!db->ptrs[n]) {
			db->ptrs[n] = ptr2hdl(ptr);

			return db->ptrs[n];
		}
	}

	/* No location available, grow the ptrs array */
	if (db->max_ptrs)
		new_max_ptrs = db->max_ptrs * 2;
	else
		new_max_ptrs = HANDLE_DB_INITIAL_MAX_PTRS;

	p = TEE_Realloc(db->ptrs, new_max_ptrs * sizeof(void *));
	if (!p)
		return 0;
	db->ptrs = p;

	TEE_MemFill(db->ptrs + db->max_ptrs, 0,
		    (new_max_ptrs - db->max_ptrs) * sizeof(void *));
	db->max_ptrs = new_max_ptrs;

	/* Since n stopped at db->max_ptrs there is an empty location there */
	db->ptrs[n] = ptr2hdl(ptr);

	return db->ptrs[n];
}

static int hdl2idx(size_t *idx, struct handle_db *db, uint32_t handle)
{
	size_t n;

	if (!db || !handle)
		return -1;

	/* Try to find an empty location (index 0 is reserved as invalid) */
	for (n = 0; n < db->max_ptrs; n++) {
		if (db->ptrs[n] != handle)
			continue;

		*idx = n;
		return 0;
	}

	return -1;
}

void *handle_put(struct handle_db *db, uint32_t handle)
{
	size_t idx = 0;

	if (hdl2idx(&idx, db, handle))
		return NULL;

	db->ptrs[idx] = 0;

	return hdl2ptr(handle);
}

void *handle_lookup(struct handle_db *db, uint32_t handle)
{
	size_t idx = 0;

	if (hdl2idx(&idx, db, handle))
		return NULL;

	return hdl2ptr(handle);
}


