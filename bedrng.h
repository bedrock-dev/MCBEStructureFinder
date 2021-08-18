//
// Created by xhy on 2021/8/3.
//

#ifndef BSSTRUCTURRE_BEDRNG_H
#define BSSTRUCTURRE_BEDRNG_H

#include "stdint.h"

uint32_t *mt4_get(uint32_t seed);


uint32_t *mt2_get(uint32_t seed);

uint32_t *mt_n_get(uint32_t seed, int n);

double int_2_float(uint32_t);

#endif //BSSTRUCTURRE_BEDRNG_H
