//
// Created by xhy on 2021/8/23.
//

#ifndef CUBIOMES_NEW_FINDER_H
#define CUBIOMES_NEW_FINDER_H

#include <vector>

#include "structure.h"

struct CachedArea {
    Vec2i area_pos;
    uint32_t candicate_area_seed = 0;

    CachedArea(const Vec2i &p, uint32_t seed) : area_pos(p), candicate_area_seed(seed) {}
};


class StructureFinder {
private:
    Pos center_{};
    int radius_{};
    BEStructureConfig config_{};
    std::vector<CachedArea> areas_;

    Pos get_candicate_pos_in_area(const Pos &p, uint32_t area_seed) const;

public:
    /**
     * 结构查找器的输入
     * @param search_center 输入中心(方块坐标)
     * @param range 输入半径
     * @param config 结构参数数据
     */
    StructureFinder(const Pos &search_center, size_t range, BEStructureConfig config);

    StructureFinder() = delete;

    std::vector<Pos> get_candicate_positions(uint32_t seed);
};


#endif //CUBIOMES_NEW_FINDER_H
