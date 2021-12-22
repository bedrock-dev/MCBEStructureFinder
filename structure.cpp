//
// Created by xhy on 2021/8/3.
//

#include "structure.h"
#include <string>

#include "be_random.h"
#include "generator.h"
#include "layers.h"
#include "util.h"
#include <cmath>
#include <cstdio>

namespace {
int max(int x, int z) { return x < z ? z : x; }

int find_in_filter(BiomeID target, const BiomeID biomes[], size_t n) {
  for (int i = 0; i < n; i++) {
    if (target == biomes[i]) {
      return 1;
    }
  }
  return 0;
}
} // namespace

static bool area_contain_biome_only(struct Generator *g, int px, int pz, int r,
                                    const BiomeID filter[], size_t n) {
  if (!g)
    return false;
  // use a 4*4 resolution to check
  int x0 = (px - r) >> 2;
  int z0 = (pz - r) >> 2;
  int w = ((r + px) >> 2) - x0 + 1;
  int h = ((r + pz) >> 2) - z0 + 1;
  struct Range range {};
  // 1:16, a.k.a. horizontal chunk scaling
  range.scale = 4;
  // Define the position and size for a horizontal area:
  range.x = x0;               //中心x
  range.z = z0;               //中心x
  range.sx = w, range.sz = h; // size (width,height)
  // Set the vertical range as a plane near sea level at scale 1:4.
  range.y = 15, range.sy = 1;
  int *biomeIds = allocCache(g, range);
  genBiomes(g, biomeIds, range);

  //   int pix4cell = 4;
  //   int imgWidth = pix4cell * range.sx, imgHeight = pix4cell * range.sz;
  //   unsigned char biomeColors[256][3];
  //   initBiomeColors(biomeColors);
  //   auto *rgb = (unsigned char *)malloc(3 * imgWidth * imgHeight);
  //   biomesToImage(rgb, biomeColors, biomeIds, range.sx, range.sz, pix4cell,
  //   2);

  //   // Save the RGB buffer to a PPM image file.
  //   auto name = "map-" + std::to_string(px) + "-" + std::to_string(pz) +
  //   ".ppm"; savePPM(name.c_str(), rgb, imgWidth, imgHeight);

  const int area = w * h;
  for (int i = 0; i < area; i++) {
    if (!find_in_filter(static_cast<BiomeID>(biomeIds[i]), filter, n)) {
      free(biomeIds);
      return false;
    }
  }
  free(biomeIds);
  return true;
}

int is_valid_spawn_biome(int biome_id) {
  return find_in_filter(static_cast<BiomeID>(biome_id), WORLD_SPAWN_FILTER,
                        sizeof(WORLD_SPAWN_FILTER) / BSZ);
}

// thanks for ddf
// There is an error within tens of meters
Vec2i get_world_spawn_position(uint32_t seed) {
  LayerStack g;
  //  setupOverworldGenerator(&g, MC_1_17);
  Layer *layer = &g.layers[L_NUM - 2];
  setLayerSeed(layer, seed);
  Vec2i pos{-1, -1};
  int step = 0;
  while (true) {
    int *biomeIds = nullptr;

    // allocCache(layer, 10, 10);
    genArea(layer, biomeIds, step, 0, 10, 10);
    for (int zo = 0; zo < 10; zo++) {
      for (int xo = 0; xo < 10; xo++) {
        int v7 = biomeIds[xo + 10 * zo];
        if (is_valid_spawn_biome(v7)) {
          if (xo) {
            int v6 = biomeIds[(xo - 1) + 10 * zo];
            if (is_valid_spawn_biome(v6) && xo + 1 < 10) {
              int v5 = biomeIds[(xo + 1) + 10 * zo];
              if (is_valid_spawn_biome(v5)) {
                if (zo) {
                  int v4 = biomeIds[xo * 10 * (zo - 1)];
                  if (is_valid_spawn_biome(v4) && zo + 1 < 10) {
                    int v3 = biomeIds[xo * 10 * (zo + 1)];
                    if (is_valid_spawn_biome(v3)) {
                      pos.x = 4 * (xo + step);
                      pos.z = zo * 4;
                      return pos;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    step += 10;
    free(biomeIds);
  }
}

static int check_ship_wreak() { return 1; }

static int check_ocean_ruin() { return 1; }

static bool check_random_scattered(Generator *g, enum BEStructureType type,
                                   ChunkPos pos) {
  int x = pos.x * 16 + 8;
  int z = pos.z * 16 + 8;
  switch (type) {
  case BEDesertTemple:
    return area_contain_biome_only(g, x, z, 0, DESERT_TEMPLE_FILTER, 2);
  case BEJungleTemple:
    return area_contain_biome_only(g, x, z, 0, JUNGLE_TEMPLE_FILTER, 2);
  case BEWitchHut:
    return area_contain_biome_only(g, x, z, 0, WITCH_HUT_FILTER, 2);
  case BEIgloo:
    return area_contain_biome_only(g, x, z, 0, IGLOO_FILTER, 2);
  default:
    return 0;
  }
}

static int check_ruined_portal() { return 1; }

// error
static int check_minshaft(uint32_t seed, ChunkPos pos) {
  uint32_t *mt = mt_n_get(seed, 2);
  uint32_t chunk_seed = seed ^ (mt[1] * pos.z) ^ (mt[0] * pos.x);
  uint32_t *mt2 = mt_n_get(chunk_seed, 3);
  double rf = int_2_float(mt2[1]);
  if (rf >= 0.004)
    return 0;
  return mt2[2] % 80 < max(abs(pos.x), abs(pos.z));
}

static bool struct_position_valid(struct Generator *g,
                                  enum BEStructureType type, ChunkPos pos) {
  int x = pos.x * 16 + 8;
  int z = pos.z * 16 + 8;
  switch (type) {
  case BEVillage:
    return area_contain_biome_only(g, x, z, 2, VILLAGE_FILTER,
                                   sizeof(VILLAGE_FILTER) / BSZ);
  case BEOceanMonument:
    if (!area_contain_biome_only(g, x, z, 16, OCEAN_MONUMENT_SPAWN_FILTER,
                                 sizeof(OCEAN_MONUMENT_SPAWN_FILTER) / BSZ)) {
      return 0;
    } else {
      return area_contain_biome_only(g, x, z, 29, OCEAN_MONUMENT_FILTER,
                                     sizeof(OCEAN_MONUMENT_FILTER) / BSZ);
    }
  case BEBuriedTreasure:
    return area_contain_biome_only(g, x, z, 3, BURIED_TREASURE_FILTER,
                                   sizeof(BURIED_TREASURE_FILTER) / BSZ);
  case BEWoodlandMansion:
    return area_contain_biome_only(g, x, z, 32, WOODLAND_MANSION_FILTER,
                                   sizeof(WOODLAND_MANSION_FILTER) / BSZ);
  case BEPillagerOutpost:
    return area_contain_biome_only(g, x, z, 0, PILLAGER_OUTPOST_FILTER,
                                   sizeof(PILLAGER_OUTPOST_FILTER) / BSZ);
  default:
    break;
  }
  return 1;
}

// int is_feature_chunk_nether(enum BEStructureType type, Layer *layer,

//                             uint32_t worldSeed, ChunkPos chunkPos) {
//   if (type == BERuindPortal) {
//     return structure_location_check_2(BE_RUIN_PORTAL_NETHER, worldSeed,
//                                       chunkPos);
//   }
//   if (type != BEBastion && type != BENetherFortress) {
//     return 0;
//   }
//   struct BEStructureConfig cfg = BE_NETHER_STRUCTURE;
//   if (chunkPos.x < 0)
//     chunkPos.x -= cfg.spacing - 1;
//   if (chunkPos.z < 0)
//     chunkPos.z -= cfg.spacing - 1;
//   uint32_t seed = cal_structure_seed(
//       worldSeed, cfg.salt, chunkPos.x / cfg.spacing, chunkPos.z /
//       cfg.spacing);

//   uint32_t *mt = mt_n_get(seed, 3);
//   uint32_t r1 = mt[0] % cfg.spawnRange;
//   uint32_t r2 = mt[1] % cfg.spawnRange;
//   int xOff = chunkPos.x % cfg.spacing;
//   int zOff = chunkPos.z % cfg.spacing;
//   if (xOff < 0)
//     xOff += cfg.spacing - 1;
//   if (zOff < 0)
//     zOff += cfg.spacing - 1;
//   if (r1 == xOff && r2 == zOff) {
//     if (mt[2] % 6 >= 2) {
//       free(mt);
//       return BEBastion;
//     } else {
//       free(mt);
//       return BENetherFortress;
//     }
//   } else {
//     free(mt);
//     return 0;
//   }
// }

bool extra_feature_check(struct Generator *g, enum BEStructureType type,
                         ChunkPos pos) {
  if (type == BEJungleTemple || type == BEDesertTemple || type == BEWitchHut ||
      type == BEIgloo) {
    return check_random_scattered(g, type, pos);
  } else if (type == BEOceanMonument || type == BEVillage ||
             type == BEOceanRuin || type == BEWoodlandMansion ||
             type == BEBuriedTreasure || type == BEPillagerOutpost) {
    return struct_position_valid(g, type, pos);
  } else {
    // todo
    switch (type) {
    case BERuindPortal:
      return true;
    case BEStronghold:
      return false;
    case BEMineshaft:
      return check_minshaft(0, pos);
    case BEShipwreck:
      return check_ship_wreak();
    case BEEndcity:
      return false;
    }
  }
  return true;
}

ChunkPos *generate_stronghold_positions(uint32_t seed, struct Layer *layer) {
  auto *positions = new ChunkPos[3];
  uint32_t *mt = mt_n_get(seed, 2);
  int count = 0;
  double angle = 6.2831855 * int_2_float(mt[0]);
  uint32_t chunk_dist = mt[1] % 16 + 40;
  while (count < 3) {
    double cos_v = cos(angle);
    int cx = floor(cos_v * chunk_dist);
    double sin_v = sin(angle);
    int cz = floor(sin_v * chunk_dist);
    int found = 0;
    for (int chunkX = cx - 8; chunkX < cx + 8; ++chunkX) {
      for (int chunkZ = cz - 8; chunkZ < cz + 8; ++chunkZ) {
        // village near
        ChunkPos pos = {chunkX, chunkZ};
        //这儿需要重写先不管
        //  if (is_feature_chunk_overworld(BEVillage, layer, seed, pos)) {
        found = 1;
        positions[count].x = chunkX * 16;
        positions[count].z = chunkZ * 16;
        ++count;
        if (count == 3) {
          return positions;
        }
        break;
      }

      // }
    }
    if (found) {
      angle += 1.8849558;
      chunk_dist += 8;
    } else {
      angle += 0.78539819;
      chunk_dist += 4;
    }
  }
  return positions;
}

BiomeID get_biome_at_pos(Layer *layer, int x, int z) {
  int *ids = nullptr;
  // allocCache(layer, 1, 1);
  genArea(layer, ids, x, z, 1, 1);
  auto biomeID = static_cast<BiomeID>(ids[0]);
  free(ids);
  return biomeID;
}

int check_nether_fortress_1_14(uint32_t worldseed, ChunkPos pos) {
  int cx = pos.x >> 4;
  int cz = pos.z >> 4;
  uint32_t seed = worldseed ^ (cz << 4) ^ cx;
  uint32_t *mt = mt_n_get(seed, 4);
  if (mt[1] % 3 != 0) {
    return 0;
  }
  return pos.x == mt[2] % 8 + 16 * cx + 4 && pos.z == mt[3] % 8 + 16 + cz + 4;
}
