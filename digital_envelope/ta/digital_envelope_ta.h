#ifndef DIGITAL_ENVELOPE_TA_H
#define DIGITAL_ENVELOPE_TA_H

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

/* UUID for this TA */
#define TA_DIGITAL_ENVELOPE_UUID \
    { 0x12345678, 0x90ab, 0xcdef, \
      { 0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef } }

/* Command IDs implemented in this TA */
#define CMD_ENCRYPT 0x00000001
#define CMD_DECRYPT 0x00000002

#endif /* DIGITAL_ENVELOPE_TA_H */