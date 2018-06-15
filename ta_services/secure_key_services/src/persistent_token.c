/*
 * Copyright (c) 2018, Linaro Limited
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <assert.h>
#include <sks_ta.h>
#include <string.h>
#include <string_ext.h>
#include <tee_internal_api_extensions.h>
#include <util.h>

#include "pkcs11_token.h"
#include "sks_helpers.h"

/* 'X' will be replaced by the token decimal id (up to 9!) */
#define TOKEN_DB_FILE_BASE		"token.db.X"

void close_persistent_db(struct ck_token *token)
{
	int n;

	for (n = 0; n < SKS_MAX_USERS; n++) {
		TEE_CloseObject(token->pin_hdl[n]);
		token->pin_hdl[n] = TEE_HANDLE_NULL;
	}

	TEE_CloseObject(token->db_hdl);
	token->db_hdl = TEE_HANDLE_NULL;
}

int update_persistent_db(struct ck_token *token, size_t offset, size_t size)
{
	unsigned int token_id = get_token_id(token);
	char file[] = TOKEN_DB_FILE_BASE;
	uint8_t *field = (uint8_t *)token->db_main + offset;
	TEE_Result res;

	if (snprintf(file + sizeof(file) - 2, 2, "%1d", token_id) >= 2)
		TEE_Panic(0);

	if (token->db_hdl == TEE_HANDLE_NULL)
		return 1;

	res = TEE_SeekObjectData(token->db_hdl, offset, TEE_DATA_SEEK_SET);
	if (res)
		return tee2sks_error(res);

	res = TEE_WriteObjectData(token->db_hdl, field, size);
	if (res)
		return tee2sks_error(res);

	return 0;
}

static void init_pin_keys(struct ck_token *token, unsigned int uid)
{
	TEE_Result res;
	unsigned int token_id = get_token_id(token);
	TEE_ObjectHandle *key_hdl = &token->pin_hdl[uid];
	char file[32] = { 0 };

	assert(token_id < 10 && uid < 10);

	if (snprintf(file, 32, "token.db.%1d-pin%1d", token_id, uid) >= 32)
		TEE_Panic(0);

	res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
					file, sizeof(file), 0, key_hdl);
	if (res == TEE_SUCCESS) {
		MSG("pin key found");
		return;
	}

	if (res == TEE_ERROR_ITEM_NOT_FOUND) {
		TEE_Attribute attr;
		TEE_ObjectHandle hdl = TEE_HANDLE_NULL;
		uint8_t pin_key[16];

		TEE_GenerateRandom(pin_key, sizeof(pin_key));
		TEE_InitRefAttribute(&attr, TEE_ATTR_SECRET_VALUE,
				     pin_key, sizeof(pin_key));

		res = TEE_AllocateTransientObject(TEE_TYPE_AES, 128, key_hdl);
		if (res)
			TEE_Panic(0);

		res = TEE_PopulateTransientObject(*key_hdl, &attr, 1);
		if (res)
			TEE_Panic(0);

		res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
						 file, sizeof(file), 0,
						 *key_hdl,
						 pin_key, sizeof(pin_key),
						 &hdl);
		if (res)
			TEE_Panic(0);

		TEE_CloseObject(hdl);
		return;
	}

	TEE_Panic(0);
}

/*
 * Return the token instance, either initialized from reset or initialized
 * from the token persistent state if found.
 */
struct ck_token *init_token_db(unsigned int token_id)
{
	struct ck_token *token = get_token(token_id);
	TEE_Result res;
	char db_file[] = TOKEN_DB_FILE_BASE;
	TEE_ObjectHandle db_hdl = TEE_HANDLE_NULL;
	struct token_persistent_main *db_main;
	int n;

	if (!token)
		return NULL;

	for (n = 0; n < SKS_MAX_USERS; n++)
		init_pin_keys(token, n);

	db_main = TEE_Malloc(sizeof(*db_main), 0);
	if (!db_main)
		goto error;

	if (snprintf(db_file + sizeof(db_file) - 2, 2, "%1d", token_id) >= 2)
		TEE_Panic(0);

	res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
					db_file, sizeof(db_file),
					TEE_DATA_FLAG_ACCESS_READ |
					TEE_DATA_FLAG_ACCESS_WRITE,
					&db_hdl);
	if (res == TEE_SUCCESS) {
		uint32_t size;

		size = sizeof(*db_main);
		res = TEE_ReadObjectData(db_hdl, db_main, size, &size);
		if (res || size != sizeof(*db_main))
			TEE_Panic(0);

	} else if (res == TEE_ERROR_ITEM_NOT_FOUND) {

		IMSG("Init SKS persistent database for token #%d", token_id);

		TEE_MemFill(db_main, 0, sizeof(*db_main));
		TEE_MemFill(db_main->label, '*', sizeof(db_main->label));

		/*
		 * Not yet supported:
		 * SKS_TOKEN_FULLY_RESTORABLE
		 * SKS_TOKEN_HAS_CLOCK => related to TEE time secure level
		 */
		db_main->flags = SKS_CKFT_SO_PIN_TO_BE_CHANGED |
				 SKS_CKFT_USER_PIN_TO_BE_CHANGED |
				 SKS_CKFT_RNG |
				 SKS_CKFT_WRITE_PROTECTED |
				 SKS_CKFT_LOGIN_REQUIRED |
				 SKS_CKFT_DUAL_CRYPTO_OPERATIONS;

		/* 2 files: persistent state + persistent object references */
		res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
						 db_file, sizeof(db_file),
						 TEE_DATA_FLAG_ACCESS_READ |
						 TEE_DATA_FLAG_ACCESS_WRITE,
						 TEE_HANDLE_NULL,
						 db_main, sizeof(*db_main),
						 &db_hdl);
		if (res)
			TEE_Panic(0);
	} else {
		/* Can't do anything... */
		return NULL;
	}

	token->db_main = db_main;
	token->db_hdl = db_hdl;
	TEE_SeekObjectData(token->db_hdl, 0, TEE_DATA_SEEK_SET);

	return token;

error:
	TEE_Free(db_main);
	if (db_hdl != TEE_HANDLE_NULL)
		TEE_CloseObject(db_hdl);

	return NULL;
}
