//
// Created by xhy on 2021/8/3.
//

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


static const struct BEStructureConfig BE_RANDOM_SCATTERED = {32, 24, 14357617}; //女巫房子，丛林，沙漠神殿
static const struct BEStructureConfig BE_BURIED_TREASURE = {4, 2, 16842397}; //海底宝藏
static const struct BEStructureConfig BE_PILLAGER_OUTPOST = {80, 56, 165745296}; //哨塔
static const struct BEStructureConfig BE_VILLAGE = {27, 17, 10387312}; //村庄
static const struct BEStructureConfig BE_WOODLAND_MANSION = {80, 60, 10387319}; //林地府邸
static const struct BEStructureConfig BE_END_CITY = {20, 9, 10387313}; //末地城市
static const struct BEStructureConfig BE_OCEAN_MONUMENT = {32, 27, 10387313}; //海底神殿


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

int check_position(enum BEStructureType tpye, Layer *layer, uint32_t seed, struct ChunkPos p);

