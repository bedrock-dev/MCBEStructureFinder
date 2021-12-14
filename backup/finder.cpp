//
// Created by xhy on 2021/8/20.
//

#include "finder.h"
#include "structure.h"
#include <cstdio>
#include <vector>
#include <exception>
#include <cassert>
#include <iostream>
#include <algorithm>
#include "be_random.h"
#include <cmath>
#include <thread>
#include <chrono>
#include <fstream>
#include <set>
#include <map>
#include <utility>

constexpr Pos CENTER = {0, 0};
constexpr int SEARCH_RADIUS = 4;
constexpr int AREA_NUM = (SEARCH_RADIUS * 2 + 1) * (SEARCH_RADIUS * 2 + 1);

struct SearchArea {
    constexpr SearchArea() : poses() {
        int index = 0;
        for (int i = -SEARCH_RADIUS; i <= SEARCH_RADIUS; i++) {
            for (int j = -SEARCH_RADIUS; j <= SEARCH_RADIUS; j++) {
                poses[index].x = CENTER.x + i;
                poses[index].z = CENTER.z + j;
                ++index;
            }
        }
    }

    Pos poses[AREA_NUM];
};


uint32_t cal_candicate_seed(int x, int z) {
    return 10387313 - 245998635 * z - 1724254968 * x;
}

struct CachedCandicateAreaSeed {
    constexpr CachedCandicateAreaSeed() : seeds() {

    }

    uint32_t seeds[AREA_NUM];
};


struct CandicateStructurePos {
    Pos p{};
    int valid = -1;
};

namespace {

    uint32_t CACHED_SEED[AREA_NUM];

    void init_cached_structure_seed() {
        int index = 0;
        for (int i = -SEARCH_RADIUS; i <= SEARCH_RADIUS; i++) {
            for (int j = -SEARCH_RADIUS; j <= SEARCH_RADIUS; j++) {
                CACHED_SEED[index] = cal_candicate_seed(CENTER.x + i, CENTER.z + j);
                ++index;
            }
        }
    }
}

constexpr auto SEARCH_AREA = SearchArea();


bool in_range(int min, int max, int val) {
    return val >= min && val < max;
}


int get_cong_in_range(int begin, int modulo, int remain) {
    int r = begin - (begin % modulo);
    if (begin > 0) {
        r += modulo;
    }
    int c1 = r + remain;
    if (in_range(begin, begin + modulo, c1)) {
        return c1;
    } else {
        return c1 - modulo;
    }
}


uint64_t dis2(Pos p1, Pos p2) {
    int64_t dx = p1.x - p2.x;
    int64_t dz = p1.z - p2.z;
    return static_cast<uint64_t>(dx * dx) + static_cast<uint64_t>(dz * dz);
}

template<int SCALA>
Pos coord_map(const Pos &origin) {
    static_assert(SCALA > 0);
    auto cx = origin.x < 0 ? origin.x + 1 - SCALA : origin.x;
    auto cz = origin.z < 0 ? origin.z + 1 - SCALA : origin.z;
    return {cx / SCALA, cz / SCALA};
}


template<int SCALA>
PosBox get_box(const Pos &origin) {
    auto x = origin.x * SCALA;
    auto z = origin.z * SCALA;
    return {
            {x,             z},
            {x + SCALA - 1, z + SCALA - 1}
    };
}

//p是区域坐标
template<int SCALA>
Pos get_candicate_structure_in_area(Pos areaPos, int spawn_range, Layer *layer, uint32_t seed, int index) {
    auto box = get_box<SCALA>(areaPos);
    const BEStructureConfig cfg = BE_OCEAN_MONUMENT;
    auto s_seed = CACHED_SEED[index] + seed;
    uint32_t *mt = mt_n_get(s_seed, 4);
    uint32_t r1 = mt[0] % cfg.spawnRange;
    uint32_t r2 = mt[1] % cfg.spawnRange;
    uint32_t r3 = mt[2] % cfg.spawnRange;
    uint32_t r4 = mt[3] % cfg.spawnRange;
    int avg_x = static_cast<int>((r1 + r2) / 2);
    int avg_z = static_cast<int>((r3 + r4) / 2);
    int i = get_cong_in_range(box.min.x, cfg.spacing, avg_x);
    int j = get_cong_in_range(box.min.z, cfg.spacing, avg_z);
    free(mt);
    return {i, j};
}

template<int SCALA_AREA, int SPAWN_RANGE>
int check_seed(uint32_t seed, LayerStack *layerStack, int search_range, int dis, FILE *fp, int thread_idx) {
    int num = 0;
    if (fp == nullptr) {
        fp = stdout;
    }

    std::vector<PosBox> res;
    Layer *layer = &layerStack->layers[L_NUM - 2];
    setLayerSeed(layer, seed);
    int w = (search_range << 1) + 1;
    const int area_num = w * w;
    auto *cp = new CandicateStructurePos[area_num];
    int found_index = 0;
    auto poses = SEARCH_AREA.poses;
    for (int i = 0; i < area_num; i++) {
        auto pos = get_candicate_structure_in_area<SCALA_AREA>(poses[i], SPAWN_RANGE, layer, seed, i);
        cp[found_index].p = pos;
        cp[found_index].valid = -1;
        ++found_index;
    }

    const size_t max_dis_2 = dis * dis;
    for (int i = 0; i < found_index - 1; i++) {
        for (int j = i + 1; j < found_index; j++) {
            auto d2 = dis2(cp[i].p, cp[j].p);
            //距离合法
            if (d2 <= max_dis_2) {
                //如果第一个的群系还没有计算，就计算并缓存
                if (cp[i].valid == -1) {
                    cp[i].valid = struct_position_valid(BEOceanMonument, layer, cp[i].p);
                }

                //算出来群系是对的
                if (cp[i].valid == 1) {
                    //第二个的群系没有计算
                    if (cp[j].valid == -1) {
                        cp[j].valid = struct_position_valid(BEOceanMonument, layer, cp[j].p);
                    }
                    //如果第二个也满足
                    if (cp[j].valid == 1) {
                        ++num;
                        PosBox box{};
                        box.min = cp[i].p;
                        box.max = cp[j].p;
                        res.push_back(box);
                        if (d2 <= 36) {
                            fprintf(fp, "seed: %u ([%d %d]-[%d %d] %.3f)\n", seed,
                                    cp[i].p.x * 16 + 8, cp[i].p.z * 16 + 8,
                                    cp[j].p.x * 16 + 8, cp[j].p.z * 16 + 8,
                                    16 * sqrt(static_cast<double >(d2))
                            );
                        }

                    }
                }
            }
        }
    }

    free(cp);
//    if (res.size() >= 3) {
//
//        if (res.size() > 3) {
//            printf("===============seed %u > 3=====================\n", seed);
//        }
//        std::set<std::pair<int, int>> pairs;
//        for (auto &i :res) {
//            pairs.insert({i.min.x, i.min.z});
//            pairs.insert({i.max.x, i.max.z});
//            //printf("[(%d %d)-(%d %d)]\n", i.min.x * 16 + 8, i.min.z * 16 + 8, i.max.x * 16 + 8, i.max.z * 16 + 8);
//        }
//        if (pairs.size() < 6 || res.size() > 3) {
//            printf("seed: %u\n", seed);
//            std::map<std::pair<int, int>, std::vector<std::pair<int, int>>> map;
//            for (auto &i :res) {
//                int x1 = i.min.x * 16 + 8;
//                int z1 = i.min.z * 16 + 8;
//                int x2 = i.max.x * 16 + 8;
//                int z2 = i.max.z * 16 + 8;
//                // printf("[(%d %d)-(%d %d) %.3f]\n", x1, z1, x2, z2, sqrt((double) dis2({x1, z1}, {x2, z2})));
//                auto p1 = std::pair<int, int>(x1, z1);
//                auto p2 = std::pair<int, int>(x2, z2);
//                map[p1].push_back(p2);
//                map[p2].push_back(p1);
//            }
//            for (auto &p :map) {
//                if (p.second.size() >= 2) {
//                    int x1 = p.second[0].first;
//                    int z1 = p.second[0].second;
//                    int x2 = p.second[1].first;
//                    int z2 = p.second[1].second;
//                    auto d = sqrt((double) dis2({x1, z1}, {x2, z2}));
//                    if (d <= 280) {
//                        printf("[%d %d] <==> [(%d %d)-(%d %d) %.3f]\n", p.first.first, p.first.second,
//                               x1, z1, x2, z2, d);
//                    }
//                }
//
//            }
//        }
//
//
//    }
    return num;
}


void multi_finder(uint32_t seedMin, uint32_t seedMax) {
    size_t threadNum = std::thread::hardware_concurrency();
    if (threadNum == 0) {
        fprintf(stderr, "the program can not decate the core num of this computer, 4 thread will be used\n");
        threadNum = 4;
    }

    auto range = seedMax - seedMin + 1;
    auto length = range / threadNum;
    if (range % threadNum != 0)
        length++;
    std::vector<std::thread> threads;
    threads.reserve(threadNum);
    for (int i = 0; i < threadNum; ++i) {
        threads.emplace_back(
                [seedMax, seedMin, i, length, threadNum]() {
                    uint32_t min = seedMin + i * length;
                    uint32_t max = seedMin + (i + 1) * length - 1;
                    if (i == threadNum - 1 && max != seedMax)
                        max = seedMax;
                    const size_t percent_1 = length / 100;
                    printf("thread %d's range is :[%zu -- %zu]\n", i, min, max);
                    LayerStack g;
                    setupOverworldGenerator(&g, MC_1_17);
                    const auto file_name = std::to_string(i) + ".txt";
                    FILE *fp = fopen(file_name.c_str(), "a+");
                    int index = 0;
                    for (uint32_t s = min; s <= max; s++) {
                        index++;
                        if (index % percent_1 == 0) {
                            printf("thread %d:  %d%% finish\n", i, index / percent_1);
                        }
                        int n = check_seed<32, 26>(s, &g, SEARCH_RADIUS, 12, fp, i);
                    }
                    fclose(fp);
                    printf("thread %d: finished\n", i);
                });
    }


    for (auto &t:threads) {
        t.join();
    }
}

