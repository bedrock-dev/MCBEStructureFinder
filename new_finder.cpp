//
// Created by xhy on 2021/8/23.
//
#include "utils.h"
#include "new_finder.h"
#include "cstdio"
#include <vector>
#include "be_random.h"
#include <cassert>

namespace {

    uint32_t cal_candicate_seed(const Vec2i &p, int salt) {
        return salt - 245998635 * p.z - 1724254968 * p.x;
    }


}

StructureFinder::StructureFinder(const Pos &search_center, size_t range, BEStructureConfig config) {
    //init search center
    auto chunk_pos_center = scala_down(search_center, 16);
    this->center_ = scala_down(chunk_pos_center, config.spacing);
    this->config_ = config;
    LOG("search center's area position is[%d,%d]", center_.x, center_.z);
    LOG("structure args is % %d %d", config_.spacing, config_.spawnRange, config_.salt);
    //init search chunk pos;
    range >>= 4; //covert to chunk distance
    radius_ = static_cast<int>(range) / config.spacing;
    if (range % config.spacing != 0)
        radius_++;

    LOG("serach radius is %d", radius_);
    for (int i = -radius_; i <= radius_; i++) {
        for (int j = -radius_; j <= radius_; j++) {
            Vec2i area_pos = {search_center.x + i, search_center.z + j};
            areas_.emplace_back(area_pos, cal_candicate_seed(area_pos, config_.salt));
        }
    }
    auto min = scala_up(areas_[0].area_pos, config.spacing).min;
    auto max = scala_up(areas_[areas_.size() - 1].area_pos, config_.spacing).max;
    LOG("total %zu areas need search, range is [%d %d] -- [%d %d]", areas_.size(), min.x << 4, min.z << 4,
        max.x << 4, max.z << 4);
}

std::vector<Pos> StructureFinder::find_candicate_positions(uint32_t seed) {
    std::vector<Pos> p;
    for (auto &area: this->areas_) {
        auto pos = get_candicate_pos_in_area(area.area_pos, area.candicate_area_seed + seed);
        p.push_back(pos);
    }
    return p;
}

Pos StructureFinder::get_candicate_pos_in_area(const Pos &p, uint32_t area_seed) const {
    assert(config_.num == 2 || config_.num == 4);
    auto box = scala_up(p, config_.spacing);
    uint32_t *mt = mt_n_get(area_seed, config_.num);
    uint32_t r1 = mt[0] % config_.spawnRange;
    uint32_t r2 = mt[1] % config_.spawnRange;
    uint32_t avg_x, avg_z;
    if (config_.num == 2) {
        avg_x = r1;
        avg_z = r2;
    } else {
        uint32_t r3 = mt[2] % config_.spawnRange;
        uint32_t r4 = mt[3] % config_.spawnRange;
        avg_x = static_cast<int>((r1 + r2) / 2);
        avg_z = static_cast<int>((r3 + r4) / 2);

    }
    int i = get_cong_with_module(box.min.x, config_.spacing, avg_x);
    int j = get_cong_with_module(box.min.z, config_.spacing, avg_z);
    return {i, j};
}


