#include <stdio.h>
#include <string.h>
#include <tee_client_api.h>
#include "digital_envelope_ta.h"

int main(void) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_Result res;
    TEEC_UUID uuid = TA_DIGITAL_ENVELOPE_UUID;
    uint32_t err_origin;

    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) return res;
    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    if (res != TEEC_SUCCESS) return res;

    char *plaintext = "Hello OP-TEE Digital Envelope!";
    uint8_t out[512];

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT,
                                     TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = plaintext;
    op.params[0].tmpref.size = strlen(plaintext);
    op.params[1].tmpref.buffer = out;
    op.params[1].tmpref.size = sizeof(out);

    res = TEEC_InvokeCommand(&sess, CMD_ENCRYPT, &op, &err_origin);
    if (res == TEEC_SUCCESS) {
        printf("Encryption success! Output size: %zu bytes\n", op.params[1].tmpref.size);
        printf("Ciphertext + tag + wrapped key + IV:\n");
        for (size_t i = 0; i < op.params[1].tmpref.size; i++)
            printf("%02x", out[i]);
        printf("\n");
    } else {
        printf("Encryption failed: 0x%x, origin: 0x%x\n", res, err_origin);
    }

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}
