/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2014-2018, Linaro Limited
 */

#ifndef __HANDLE_H__
#define __HANDLE_H__

#include <stddef.h>

struct handle_db {
	uint32_t *ptrs;
	uint32_t max_ptrs;
};

#define HANDLE_DB_INITIALIZER { NULL, 0 }

/* From user_ta_header.c, built within TA */
extern uint8_t ta_heap[];

/*
 * Frees all internal data structures of the database, but does not free
 * the db pointer. The database is safe to reuse after it's destroyed, it
 * just be empty again.
 */
void handle_db_destroy(struct handle_db *db);

/*
 * Allocates a new handle and assigns the supplied pointer to it,
 * ptr must not be NULL.
 * The function returns
 * ~0 on failure, and a valid handle on success
 */
uint32_t handle_get(struct handle_db *db, void *ptr);

/*
 * Deallocates a handle. Returns the assiciated pointer of the handle
 * the the handle was valid or NULL if it's invalid.
 */
void *handle_put(struct handle_db *db, uint32_t handle);

/*
 * Returns the assiciated pointer of the handle if the handle is a valid
 * handle.
 * Returns NULL on failure.
 */
void *handle_lookup(struct handle_db *db, uint32_t handle);

#endif /*__HANDLE_H__*/

