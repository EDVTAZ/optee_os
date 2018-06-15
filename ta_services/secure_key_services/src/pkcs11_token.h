/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2017-2018, Linaro Limited
 */
#ifndef __SKS_PKCS11_TOKEN_H__
#define __SKS_PKCS11_TOKEN_H__

#include <sys/queue.h>
#include <tee_internal_api.h>

#include "handle.h"
#include "object.h"

/* Hard coded description */
#define SKS_CRYPTOKI_TOKEN_LABEL		"op-tee pkcs#11 token (dev...)"
#define SKS_CRYPTOKI_TOKEN_MANUFACTURER		"Linaro"
#define SKS_CRYPTOKI_TOKEN_MODEL		"OP-TEE SKS TA"
#define SKS_CRYPTOKI_TOKEN_SERIAL_NUMBER	"0000000000000000"
#define SKS_CRYPTOKI_TOKEN_HW_VERSION		{ 0, 0 }
#define SKS_CRYPTOKI_TOKEN_FW_VERSION		{ 0, 0 }

#define SKS_CRYPTOKI_SLOT_DESCRIPTION		"OP-TEE SKS TA"
#define SKS_CRYPTOKI_SLOT_MANUFACTURER		SKS_CRYPTOKI_TOKEN_MANUFACTURER
#define SKS_CRYPTOKI_SLOT_HW_VERSION		SKS_CRYPTOKI_TOKEN_HW_VERSION
#define SKS_CRYPTOKI_SLOT_FW_VERSION		SKS_CRYPTOKI_TOKEN_FW_VERSION

#define PADDED_STRING_COPY(_dst, _src) \
	do { \
		TEE_MemFill((char *)(_dst), ' ', sizeof(_dst)); \
		TEE_MemMove((char *)(_dst), (_src), \
			    MIN(strlen((char *)(_src)), sizeof(_dst))); \
	} while (0)

enum pkcs11_token_login_state {
	PKCS11_TOKEN_STATE_INVALID = 0,		/* token default state */
	PKCS11_TOKEN_STATE_PUBLIC_SESSIONS,
	PKCS11_TOKEN_STATE_SECURITY_OFFICER,
	PKCS11_TOKEN_STATE_USER_SESSIONS,
	PKCS11_TOKEN_STATE_CONTEXT_SPECIFIC,
};

enum pkcs11_token_session_state {
	PKCS11_TOKEN_STATE_SESSION_NONE = 0,	/* token default state */
	PKCS11_TOKEN_STATE_SESSION_READ_WRITE,
	PKCS11_TOKEN_STATE_SESSION_READ_ONLY,
};

/* List of toen sessions */
LIST_HEAD(session_list, pkcs11_session);

#define SKS_MAX_USERS			2
#define SKS_TOKEN_PIN_SIZE		128

/*
 * Persistent state of the token
 *
 * @version - currently unused...
 * @label - pkcs11 formatted token label, set by client
 * @flags - pkcs11 token flags
 * @so_pin_count - counter on security officer login failure
 * @so_pin_size - byte size of the provisionned SO PIN
 * @so_pin - stores the SO PIN
 * @user_pin_count - counter on user login failure
 * @user_pin_size - byte size of the provisionned user PIN
 * @user_pin - stores the user PIN
 */
struct token_persistent_main {
	uint32_t version;

	uint8_t label[SKS_TOKEN_LABEL_SIZE];
	uint32_t flags;

	uint32_t so_pin_count;
	uint32_t so_pin_size;
	uint8_t so_pin[SKS_TOKEN_PIN_SIZE];

	uint32_t user_pin_count;
	uint32_t user_pin_size;
	uint8_t user_pin[SKS_TOKEN_PIN_SIZE];
};

/*
 * Runtime state of the token, complies with pkcs11
 *
 * @login_state - Pkcs11 login is public, user, SO or custom
 * @db_hld - TEE handle on the persistent database object or TEE_HANDLE_NULL
 * @pin_hld - TEE handles on PIN ciphering keys
 * @db_main - Volatile copy of the persistent main database
 */
struct ck_token {
	enum pkcs11_token_login_state login_state;
	TEE_ObjectHandle db_hdl;
	TEE_ObjectHandle pin_hdl[SKS_MAX_USERS];
	struct token_persistent_main *db_main;
};

/* Initialize static token instance(s) from default/persistent database */
int pkcs11_init(void);
void pkcs11_deinit(void);

/* Return token instance from token identifier */
struct ck_token *get_token(unsigned int token_id);

/* Return token identified from token instance address */
unsigned int get_token_id(struct ck_token *token);

/* Initialize target token database */
struct ck_token *init_token_db(unsigned int token_id);

/* Persistent database update */
int update_persistent_db(struct ck_token *token, size_t offset, size_t size);
void close_persistent_db(struct ck_token *token);

/*
 * Entry point for the TA commands
 */
uint32_t entry_ck_slot_list(TEE_Param *ctrl, TEE_Param *in, TEE_Param *out);
uint32_t entry_ck_slot_info(TEE_Param *ctrl, TEE_Param *in, TEE_Param *out);
uint32_t entry_ck_token_info(TEE_Param *ctrl, TEE_Param *in, TEE_Param *out);

uint32_t entry_ck_token_initialize(TEE_Param *ctrl,
				   TEE_Param *in, TEE_Param *out);

uint32_t entry_ck_token_mecha_ids(TEE_Param *ctrl,
				  TEE_Param *in, TEE_Param *out);

uint32_t entry_ck_token_mecha_info(TEE_Param *ctrl,
				   TEE_Param *in, TEE_Param *out);

uint32_t entry_ck_token_ro_session(uintptr_t teesess, TEE_Param *ctrl,
				   TEE_Param *in, TEE_Param *out);
uint32_t entry_ck_token_rw_session(uintptr_t teesess, TEE_Param *ctrl,
				   TEE_Param *in, TEE_Param *out);
uint32_t entry_ck_token_close_session(uintptr_t teesess, TEE_Param *ctrl,
				      TEE_Param *in, TEE_Param *out);
uint32_t entry_ck_token_close_all(uintptr_t teesess, TEE_Param *ctrl,
				  TEE_Param *in, TEE_Param *out);

#endif /*__SKS_PKCS11_TOKEN_H__*/
