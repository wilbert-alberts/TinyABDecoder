#ifndef __QUADDECODER_H__
#define __QUADDECODER_H__

#include <stdint.h>

extern void decodeAB(
    uint8_t newA,
    uint8_t newB,
    uint8_t *A,
    uint8_t *B,
    int16_t *position);

#endif