//
// Created by xhy on 2021/8/20.
//

#ifndef CUBIOMES_FINDER_H
#define CUBIOMES_FINDER_H

typedef int i1;

#include <ostream>
#include "structure.h"

struct PosBox {
    Pos min;
    Pos max;
    friend std::ostream &operator<<(std::ostream &os, const PosBox &box) {
        os << "min: " << box.min.x << " " << box.min.z << " max: " << box.max.x << " " << box.max.z;
        return os;
    }
};


#endif //CUBIOMES_FINDER_H

