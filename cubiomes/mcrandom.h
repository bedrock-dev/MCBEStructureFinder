#ifndef MCRANDOM_H_
#define MCRANDOM_H_

#define __STDC_LIMIT_MACROS
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Extract a pseudo-random unsigned 32-bit integer in the range 0 ... UINT32_MAX
 */
uint32_t mcrandom_next();

/*
 * Initialize Mersenne Twister with given seed value.
 */
void mcrandom_seed(uint32_t seed_value);


static inline void setSeed(uint32_t value)
{
    mcrandom_seed(value);
}

static inline int nextInt(const int n)
{
    return mcrandom_next() % n;
}

static inline double nextDouble() {
    return mcrandom_next()/4294967296.0;
}

static inline float nextFloat() {
    return nextDouble();
}

static inline void skipNextN(int n) {
    for(int i=0;i<n;++i) {
        mcrandom_next();
    }
}


#ifdef __cplusplus
} // extern "C"
#endif

#endif
