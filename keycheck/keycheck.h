#ifndef __KEYCHECK_H__
#define __KEYCHECK_H__

// Headers
#include <unistd.h>

// Constants
#define KEYCHECK_TIMED 1
#define KEYCHECK_UNLIMITED 2

// Prototypes
int keycheck_timed(uint8_t mode, uint8_t timeout);

#endif
