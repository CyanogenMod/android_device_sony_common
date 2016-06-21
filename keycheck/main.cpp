#include <unistd.h>

#include "keycheck.h"

int main(int __attribute__((unused)) argc,
        char __attribute__((unused)) *argv[])
{
    // Run the keycheck without limit
    return keycheck_timed(KEYCHECK_UNLIMITED, 0);
}
