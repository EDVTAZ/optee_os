/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2017-2018, Linaro Limited
 */

#ifndef __SKS_PROCESSING_H__
#define __SKS_PROCESSING_H__

#include <tee_internal_api.h>

struct sks_object;

/*
 * Entry points frpom SKS TA invocation commands
 */

uint32_t entry_import_object(uintptr_t teesess, TEE_Param *ctrl,
			     TEE_Param *in, TEE_Param *out);

uint32_t entry_cipher_init(uintptr_t teesess, TEE_Param *ctrl,
			   TEE_Param *in, TEE_Param *out, int enc);

uint32_t entry_cipher_update(uintptr_t teesess, TEE_Param *ctrl,
			     TEE_Param *in, TEE_Param *out, int enc);

uint32_t entry_cipher_final(uintptr_t teesess, TEE_Param *ctrl,
			    TEE_Param *in, TEE_Param *out, int enc);

uint32_t entry_generate_object(uintptr_t teesess,
			       TEE_Param *ctrl, TEE_Param *in, TEE_Param *out);

uint32_t entry_signverify_init(uintptr_t teesess, TEE_Param *ctrl,
				TEE_Param *in, TEE_Param *out, int sign);

uint32_t entry_signverify_update(uintptr_t teesess, TEE_Param *ctrl,
				 TEE_Param *in, TEE_Param *out, int sign);

uint32_t entry_signverify_final(uintptr_t teesess, TEE_Param *ctrl,
				TEE_Param *in, TEE_Param *out, int sign);

uint32_t entry_derive(uintptr_t teesess,
			TEE_Param *ctrl, TEE_Param *in, TEE_Param *out);

#endif /*__SKS_PROCESSING_H__*/
