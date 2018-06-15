/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2017-2018, Linaro Limited
 */
#ifndef __SKS_PKCS11_TOKEN_H__
#define __SKS_PKCS11_TOKEN_H__

uint32_t entry_ck_slot_list(TEE_Param *ctrl, TEE_Param *in, TEE_Param *out);
uint32_t entry_ck_slot_info(TEE_Param *ctrl, TEE_Param *in, TEE_Param *out);
uint32_t entry_ck_token_info(TEE_Param *ctrl, TEE_Param *in, TEE_Param *out);

uint32_t entry_ck_token_initialize(TEE_Param *ctrl,
				   TEE_Param *in, TEE_Param *out);

uint32_t entry_ck_token_mecha_ids(TEE_Param *ctrl,
				  TEE_Param *in, TEE_Param *out);

uint32_t entry_ck_token_mecha_info(TEE_Param *ctrl,
				   TEE_Param *in, TEE_Param *out);

#endif /*__SKS_PKCS11_TOKEN_H__*/
