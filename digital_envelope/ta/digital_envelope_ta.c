#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "digital_envelope_ta.h"

/* AES-GCM encryption helper */
static TEE_Result aes_gcm_encrypt(uint8_t *key, size_t key_len,
                                  uint8_t *plaintext, size_t plaintext_len,
                                  uint8_t *iv, size_t iv_len,
                                  uint8_t *ciphertext, uint8_t *tag, size_t tag_len)
{
    TEE_OperationHandle op;
    TEE_Result res;

    res = TEE_AllocateOperation(&op, TEE_ALG_AES_GCM, TEE_MODE_ENCRYPT, key_len * 8);
    if (res != TEE_SUCCESS) return res;

    res = TEE_SetOperationKey(op, key, key_len);
    if (res != TEE_SUCCESS) return res;

    res = TEE_AEEncryptInit(op, iv, iv_len, tag_len * 8);
    if (res != TEE_SUCCESS) return res;

    size_t out_len = plaintext_len;
    res = TEE_AEEncryptFinal(op, plaintext, plaintext_len, ciphertext, &out_len, tag, tag_len);

    TEE_FreeOperation(op);
    return res;
}

/* Simulated ECDH AES key wrap (placeholder for real ECDH) */
static TEE_Result wrap_aes_key(uint8_t *aes_key, size_t key_len,
                               uint8_t *wrapped_key, size_t *wrapped_len)
{
    for (size_t i = 0; i < key_len; i++)
        wrapped_key[i] = aes_key[i] ^ 0xAA;  /* simple XOR for demo */
    *wrapped_len = key_len;
    return TEE_SUCCESS;
}

/* Standard TA entry points */
TEE_Result TA_CreateEntryPoint(void) { return TEE_SUCCESS; }
void TA_DestroyEntryPoint(void) {}
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, TEE_Param params[4], void **sess_ctx) {
    (void)param_types; (void)params; (void)sess_ctx;
    return TEE_SUCCESS;
}
void TA_CloseSessionEntryPoint(void *sess_ctx) { (void)sess_ctx; }

/* Invoke command */
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id,
                                      uint32_t param_types, TEE_Param params[4])
{
    (void)sess_ctx;
    TEE_Result res;

    if (cmd_id == CMD_ENCRYPT) {
        uint8_t *input = params[0].memref.buffer;
        size_t input_len = params[0].memref.size;
        uint8_t *out_buf = params[1].memref.buffer;
        size_t out_len = params[1].memref.size;

        uint8_t aes_key[32], iv[12], tag[16], wrapped_key[32];
        size_t wrapped_len;

        /* 1. Generate AES key */
        res = TEE_GenerateRandom(aes_key, sizeof(aes_key));
        if (res != TEE_SUCCESS) return res;

        /* 2. Generate IV */
        res = TEE_GenerateRandom(iv, sizeof(iv));
        if (res != TEE_SUCCESS) return res;

        /* 3. AES-GCM encrypt plaintext */
        res = aes_gcm_encrypt(aes_key, sizeof(aes_key), input, input_len,
                              iv, sizeof(iv), out_buf, tag, sizeof(tag));
        if (res != TEE_SUCCESS) return res;

        /* 4. Wrap AES key using simulated ECDH */
        res = wrap_aes_key(aes_key, sizeof(aes_key), wrapped_key, &wrapped_len);
        if (res != TEE_SUCCESS) return res;

        /* 5. Append tag + wrapped key + IV after ciphertext */
        if (out_len < input_len + sizeof(tag) + wrapped_len + sizeof(iv))
            return TEE_ERROR_SHORT_BUFFER;

        uint8_t *p = out_buf + input_len;
        TEE_MemMove(p, tag, sizeof(tag));
        p += sizeof(tag);
        TEE_MemMove(p, wrapped_key, wrapped_len);
        p += wrapped_len;
        TEE_MemMove(p, iv, sizeof(iv));

        return TEE_SUCCESS;
    }

    return TEE_ERROR_NOT_SUPPORTED;
}