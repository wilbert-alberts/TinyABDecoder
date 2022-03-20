#include <stdint.h>

#include "quaddecoder.h"

extern void decodeAB(
    uint8_t newA,
    uint8_t newB,
    uint8_t *A,
    uint8_t *B,
    int16_t *position)
{
 // Capture state
  uint8_t state = (*A << 3) | (newA << 2) | (*B << 1) | (newB);

  /*
    aold	anew	bold	bnew
  0	  0	    0	    0	    0		0
  1	  0	    0	    0   	1		-1
  2	  0	    0	    1	    0		+1
  3	  0	    0	    1	    1		0
  4	  0	    1	    0	    0		+1
  5	  0	    1	    0	    1		err
  6	  0	    1	    1	    0		err
  7	  0	    1	    1	    1		-1
  8	  1	    0	    0   	0		-1
  9	  1	    0	    0	    1		err
  10	1	    0	    1	    0		err
  11	1	    0	    1	    1		+1
  12	1	    1	    0   	0		0
  13	1	    1	    0	    1		+1
  14	1	    1	    1	    0		-1
  15	1	    1	    1	    1		0

  */
  // Process new state
  switch (state)
  {
  case 0:
  case 3:
  case 12:
  case 15:
    // No change
    break;
  case 1:
  case 7:
  case 8:
  case 14:
    (*position)--;
    break;
  case 2:
  case 4:
  case 11:
  case 13:
    (*position)++;
  case 5:
  case 6:
  case 9:
  case 10:
    // Error transition
    break;
  }

  // Update AB values
  *A = newA;
  *B = newB;

}
