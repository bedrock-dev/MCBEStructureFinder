//
// Created by xhy on 2021/8/3.
//

#pragma once
#ifndef BSSTRUCTURRE_STRUCTURE_H
#define BSSTRUCTURRE_STRUCTURE_H

#endif //BSSTRUCTURRE_STRUCTURE_H

#include "cubiomes/layers.h"
#include "cubiomes/generator.h"

enum BEStructureType {
    BEVillage,
    BEStronghold,
    BEMineshaft,
    BEDesertTemple,
    BEWitchHut,
    BEJungleTemple,
    BEIgloo,
    BEOceanMonument,
    BEOceanRuin,
    BEWoodlandMansion,
    BEShipwreck,
    BERuindPortal,
    BEBuriedTreasure,
    BEPillagerOutpost,
    BENetherFortress,
    BEBastion,
    BEEndcity
};


struct BEStructureConfig {
    int spacing;
    int spawnRange;
    int salt;
};

struct ChunkPos {
    int x;
    int z;
};


typedef struct ChunkPos Pos;


static const struct BEStructureConfig BE_RANDOM_SCATTERED = {32, 24, 14357617}; //女巫房子，丛林，沙漠神殿
static const struct BEStructureConfig BE_BURIED_TREASURE = {4, 2, 16842397}; //海底宝藏
static const struct BEStructureConfig BE_PILLAGER_OUTPOST = {80, 56, 165745296}; //哨塔
static const struct BEStructureConfig BE_VILLAGE = {27, 17, 10387312}; //村庄
static const struct BEStructureConfig BE_WOODLAND_MANSION = {80, 60, 10387319}; //林地府邸
static const struct BEStructureConfig BE_END_CITY = {20, 9, 10387313}; //末地城市
static const struct BEStructureConfig BE_OCEAN_MONUMENT = {32, 27, 10387313}; //海底神殿
static const struct BEStructureConfig BE_NETHER_STRUCTURE = {30, 26, 30084232};
static const struct BEStructureConfig BE_SHIP_WREAK = {10, 5, 1};
static const struct BEStructureConfig BE_OCEAN_RUIN = {12, 5, 14357621};
static const struct BEStructureConfig BE_RUIN_PORTAL_NETHER = {25, 15, 40552231};
static const struct BEStructureConfig BE_RUIN_PORTAL_OVERWORLD = {40, 25, 40552231};

void biome_test(uint32_t seed, int l, const char *path, int width);

static const uint16_t BSZ = sizeof(enum BiomeID);

static const enum BiomeID WORLD_SPAWN_ALLOW_BIOME[] = {
        forest,
        plains,
        taiga_hills,
        forestHills,
        jungle_hills,
        jungle
};

static const enum BiomeID VILLAGE_ALLOW_BIOME[] = {plains, savanna, icePlains, taiga, taiga_hills, coldTaiga,
                                                   coldTaigaHills, desert};

static const enum BiomeID OCEAN_MONUMENT_ALLOW_BIOME[] = {
        ocean,
        deep_ocean,
        lukewarm_ocean,
        cold_ocean,
        frozen_ocean,
        warm_ocean,

        deep_lukewarm_ocean,
        deep_warm_ocean,
        deep_frozen_ocean,
        deep_cold_ocean,

        river,
        frozen_river
};

static const enum BiomeID OCEAN_MONUMENT_ALLOW_SPAWN_BIOME[] = {
        deep_ocean,
        deep_cold_ocean,
        deep_warm_ocean,
        deep_frozen_ocean,
        deep_lukewarm_ocean
};

static const enum BiomeID DESERT_TEMPLE_ALLOW[] = {desert, desert_hills, desert_lakes};
static const enum BiomeID JUNGLE_TEMPLE_ALLOW[] = {jungle, jungle_hills};
static const enum BiomeID WITCH_HUT_TEMPLE_ALLOW[] = {swamp, swampland};
static const enum BiomeID IGLOO_TEMPLE_ALLOW[] = {icePlains, coldTaiga};

//NOT
static const enum BiomeID BURIED_TREASURE_ALLOW[] = {beach, coldBeach, stone_shore, mushroom_field_shore};

static const enum BiomeID WOODLAND_MANSION_ALLOW[] = {roofedForest};

static const enum BiomeID PILLAGER_OUTPOST_ALLOW[] = {
        plains, sunflower_plains, savanna, icePlains, taiga_hills, taiga, coldTaiga, coldTaigaHills, desert
};
#ifdef __cplusplus
extern "C" {
#endif

int overworld_structure_find(enum BEStructureType type, Layer *layer, uint32_t seed, struct ChunkPos pos);

int nether_structure_find(enum BEStructureType type, Layer *layer, uint32_t worldSeed, struct ChunkPos chunkPos);

struct ChunkPos *generate_stronghold_positions(uint32_t seed, struct Layer *layer);

int struct_position_valid(enum BEStructureType type, Layer *g, struct ChunkPos pos);

struct ChunkPos find_spawn_position(uint32_t seed);

int contain_biome_only(Layer *layer, int px, int pz, int r, const int *filter, size_t n);

uint32_t cal_structure_seed(uint32_t worldSeed, int salt, int x, int z);

#ifdef __cplusplus
}
#endif