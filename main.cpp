//
// Created by xhy on 2021/12/16.
//
#include<iostream>
#include "tools.h"
#include "be_finder.h"

int main() {
    const Pos center = {1240, 5680};
    StructureFinder finder(center, 1000, BE_OCEAN_MONUMENT);
    auto p_list = finder.get_candicate_positions(0);
    for (auto p: p_list) {
        LOG("%d %d", p.x * 16 + 8, p.z * 16 + 8);
    }
    return 0;
}