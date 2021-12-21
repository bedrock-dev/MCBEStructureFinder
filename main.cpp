//
// Created by xhy on 2021/12/16.
//
#include<iostream>
#include "tools.h"
#include "be_finder.h"

int main() {
    Generator g;
    setupGenerator(&g, MC_1_18, 0);
    uint64_t seed = 123LL;
    applySeed(&g, 0, seed);
    const Pos center = {3100, 2200};
    StructureFinder finder(center, 5000, BE_OCEAN_MONUMENT);
    auto p_list = finder.get_candicate_positions(seed);
    for (auto p: p_list) {
        if (extra_feature_check(&g, BEOceanMonument, p)) {
            LOG("%d %d", p.x * 16 + 8, p.z * 16 + 8);
        }
    }
    return 0;
}