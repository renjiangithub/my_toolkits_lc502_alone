#ifndef CRC16_SANSI_H
#define CRC16_SANSI_H

#include <stdint.h>

// At Sansi, we use CRC-16-CCITT for 2-byte checksums.
// Here are some implementations that have identical outputs.

// Found in previous protocol documentation.
// Readable but slow. Use this as a reference.
uint16_t gen_crc16_ref(const uint8_t *buffer, long buffer_length);

// Implementation used in Redis. Much faster than reference.
uint16_t gen_crc16_redis(const uint8_t *buffer, long buffer_length);


#endif // CRC16_SANSI_H
