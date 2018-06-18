/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2017-2018, Linaro Limited
 */

#ifndef __SKS_HELPERS_H
#define __SKS_HELPERS_H

#include <sks_ta.h>
#include <stdint.h>
#include <stddef.h>
#include <tee_internal_api.h>

/* Short aliases for return code */
#define SKS_OK				SKS_CKR_OK
#define SKS_ERROR			SKS_CKR_GENERAL_ERROR
#define SKS_MEMORY			SKS_CKR_DEVICE_MEMORY
#define SKS_BAD_PARAM			SKS_CKR_ARGUMENT_BAD
#define SKS_SHORT_BUFFER		SKS_CKR_BUFFER_TOO_SMALL
#define SKS_FAILED			SKS_CKR_FUNCTION_FAILED

/*
 * Convert SKS retrun code into a GPD TEE result ID when matching.
 * If not, return a TEE success (_noerr) or generic error (_error).
 */
TEE_Result sks2tee_noerr(uint32_t rv);
TEE_Result sks2tee_error(uint32_t rv);
uint32_t tee2sks_error(TEE_Result res);

/* Id-to-string conversions when CFG_TEE_TA_LOG_LEVEL > 0 */
const char *sks2str_slot_flag(uint32_t id);
const char *sks2str_token_flag(uint32_t id);
const char *sks2str_rc(uint32_t id);
const char *sks2str_skscmd(uint32_t id);

#endif /*__CK_HELPERS_H*/
