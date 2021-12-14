//
// Created by xhy on 2021/8/3.
//

#pragma once
#ifndef BSSTRUCTURRE_STRUCTURE_H
#define BSSTRUCTURRE_STRUCTURE_H

#endif //BSSTRUCTURRE_STRUCTURE_H

#include "cubiomes/layers.h"
#include "cubiomes/generator.h"
#include "utils.h"

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

int check_nether_fortress_1_14(uint32_t worldseed, ChunkPos pos);


struct BEStructureConfig {
    int spacing;
    int spawnRange;
    int salt;
    int num;
};

#define DEF_STRUCTURE(Name, spacing, spawnRange, salt) constexpr BEStructureConfig Name = {spacing, spawnRange, salt};


constexpr BEStructureConfig BE_RANDOM_SCATTERED = {32, 24, 14357617, 2}; //女巫房子，丛林，沙漠神殿
constexpr BEStructureConfig BE_BURIED_TREASURE = {4, 2, 16842397, 4}; //海底宝藏
constexpr BEStructureConfig BE_PILLAGER_OUTPOST = {80, 56, 165745296, 4}; //哨塔
constexpr BEStructureConfig BE_VILLAGE = {27, 17, 10387312, 4}; //村庄
constexpr BEStructureConfig BE_WOODLAND_MANSION = {80, 60, 10387319, 4}; //林地府邸
constexpr BEStructureConfig BE_END_CITY = {20, 9, 10387313, 4}; //末地城市
constexpr BEStructureConfig BE_OCEAN_MONUMENT = {32, 27, 10387313, 4}; //海底神殿
constexpr BEStructureConfig BE_NETHER_STRUCTURE = {30, 26, 430084232, 4};
constexpr BEStructureConfig BE_SHIP_WREAK = {10, 5, 1, 4};
constexpr BEStructureConfig BE_OCEAN_RUIN = {12, 5, 14357621, 4};
constexpr BEStructureConfig BE_RUIN_PORTAL_NETHER = {25, 15, 40552231, 4};
constexpr BEStructureConfig BE_RUIN_PORTAL_OVERWORLD = {40, 25, 40552231, 4};
constexpr BEStructureConfig BE_INVALID = {1, 1, 0, 1};

void biome_test(uint32_t seed, int l, const char *path, int width);

constexpr uint16_t BSZ = sizeof(enum BiomeID);

constexpr enum BiomeID WORLD_SPAWN_FILTER[] = {
        forest,
        plains,
        taiga_hills,
        forestHills,
        jungle_hills,
        jungle
};

constexpr enum BiomeID VILLAGE_FILTER[] = {
        plains,
        savanna,
        icePlains,
        taiga,
        taiga_hills,
        coldTaiga,
        coldTaigaHills,
        desert
};

constexpr enum BiomeID OCEAN_MONUMENT_FILTER[] = {
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

constexpr enum BiomeID OCEAN_MONUMENT_SPAWN_FILTER[] = {
        deep_ocean,
        deep_cold_ocean,
        deep_warm_ocean,
        deep_frozen_ocean,
        deep_lukewarm_ocean
};

constexpr enum BiomeID DESERT_TEMPLE_FILTER[] = {
        desert,
        desert_hills,
        desert_lakes
};

constexpr enum BiomeID JUNGLE_TEMPLE_FILTER[] = {
        jungle,
        jungle_hills
};
constexpr enum BiomeID WITCH_HUT_FILTER[] = {
        swamp,
        swampland
};
constexpr enum BiomeID IGLOO_FILTER[] = {
        icePlains,
        coldTaiga
};
//NOT
constexpr enum BiomeID BURIED_TREASURE_FILTER[] = {
        beach,
        coldBeach,
        stone_shore,
        mushroom_field_shore
};

constexpr enum BiomeID WOODLAND_MANSION_FILTER[] = {
        roofedForest
};

constexpr enum BiomeID PILLAGER_OUTPOST_FILTER[] = {
        plains,
        sunflower_plains,
        savanna,
        icePlains,
        taiga_hills,
        taiga,
        coldTaiga,
        coldTaigaHills,
        desert
};


//check a chunk will spawn a structure or not
int is_feature_chunk_overworld(enum BEStructureType type, Layer *layer, uint32_t seed, ChunkPos pos);

int is_feature_chunk_nether(enum BEStructureType type, Layer *layer, uint32_t worldSeed, ChunkPos chunkPos);

//get the first 3 position of stronghold
ChunkPos *generate_stronghold_positions(uint32_t seed, struct Layer *layer);

//extra check for structure
int struct_position_valid(enum BEStructureType type, Layer *g, ChunkPos pos);

ChunkPos get_world_spawn_position(uint32_t seed);

//calculate the structure area seed
uint32_t cal_structure_seed(uint32_t worldSeed, int salt, int x, int z);


BiomeID get_biome_at_pos(Layer *layer, int x, int z);

int area_contain_biome_only(Layer *layer, int px, int pz, int r, const BiomeID *filter, size_t n);
