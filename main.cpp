//
// Created by xhy on 2021/12/16.
//
#include "be_finder.h"
#include "structure.h"
#include "tools.h"
#include <iostream>

int main() {
  //   Generator g;

  //   setupGenerator(&g, MC_1_18, 0);
  //   uint64_t seed = 123LL;
  //   applySeed(&g, 0, seed);
  //   const Pos center = {5384, 15608};
  //   StructureFinder finder(center, 1000, BE_RANDOM_SCATTERED);
  //   auto p_list = finder.get_candicate_positions(seed);
  //   for (auto p : p_list) {
  //     if (extra_feature_check(&g, BEWitchHut, p)) {
  //       LOG("%d %d", p.x * 16 + 8, p.z * 16 + 8);
  //     }
  //   }
  auto pos = get_world_spawn_position(123);
  LOG("%d %d", pos.x, pos.z);
  return 0;
}
