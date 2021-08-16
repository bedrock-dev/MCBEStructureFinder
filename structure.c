//
// Created by xhy on 2021/8/3.
//

#include "structure.h"

#include "cubiomes/layers.h"
#include "cubiomes/generator.h"
#include "cubiomes/util.h"
#include "bedrng.h"
#include <stdio.h>

static int find_in_array(int target, const int *biomes, size_t n) {
    for (int i = 0; i < n; i++) {
        if (target == biomes[i]) {
            return 1;
        }
    }
    return 0;
}

static uint32_t cal_structure_seed(uint32_t worldSeed, int salt, int x, int z) {
    return salt + worldSeed - 245998635 * z - 1724254968 * x;
}

static int contain_biome_only(Layer *layer, int px, int pz, int r, const int *filter, size_t n) {
    if (!layer)
        return 0;
    /**
     * Original: use a 4*4 resolution to check
//    int x0 = (px - r) >> 2;
//    int z0 = (pz - r) >> 2;
//    int w = ((r + px) >> 2) - x0 + 1;
//    int h = ((r + pz) >> 2) - z0 + 1;
//    int *biomeIds = allocCache(layer, w, h);
//    genArea(layer, biomeIds, x0, z0, w, h);
//    const int area = w * h;
**/
    int x0 = px - r;
    int z0 = pz - r;
    int w = 2 * r + 1;
    int *biomeIds = allocCache(layer, w, w);
    genArea(layer, biomeIds, x0, z0, w, w);
    const int area = w * w;
    for (int i = 0; i < area; i++) {
        if (!find_in_array(biomeIds[i], filter, n)) {
            return 0;
        }
    }
    return 1;
}


int structure_check_1(const struct BEStructureConfig cfg, uint32_t worldSeed, struct ChunkPos chunkPos) {
    if (chunkPos.x < 0)
        chunkPos.x -= cfg.spacing - 1;
    if (chunkPos.z < 0)
        chunkPos.z -= cfg.spacing - 1;
    uint32_t seed = cal_structure_seed(worldSeed, cfg.salt, chunkPos.x / cfg.spacing, chunkPos.z / cfg.spacing);
    uint32_t *mt = mt_n_get(seed, 4);
    uint32_t r1 = mt[0] % cfg.spawnRange;
    uint32_t r2 = mt[1] % cfg.spawnRange;
    uint32_t r3 = mt[2] % cfg.spawnRange;
    uint32_t r4 = mt[3] % cfg.spawnRange;
    int x_offset = chunkPos.x % cfg.spacing;
    int z_offset = chunkPos.z % cfg.spacing;
    if (x_offset < 0)
        x_offset += cfg.spacing - 1;
    if (z_offset < 0)
        z_offset += cfg.spacing - 1;
    free(mt);
    return (r1 + r2) / 2 == x_offset && (r3 + r4) / 2 == z_offset;
}

int structure_check_2(const struct BEStructureConfig cfg,
                      uint32_t worldSeed, struct ChunkPos chunkPos) {
    if (chunkPos.x < 0)
        chunkPos.x -= cfg.spacing - 1;
    if (chunkPos.z < 0)
        chunkPos.z -= cfg.spacing - 1;
    uint32_t seed = cal_structure_seed(worldSeed, cfg.salt, chunkPos.x / cfg.spacing, chunkPos.z / cfg.spacing);

    uint32_t *mt = mt_n_get(seed, 4);
    uint32_t r1 = mt[0] % cfg.spawnRange;
    uint32_t r2 = mt[1] % cfg.spawnRange;
    int xOff = chunkPos.x % cfg.spacing;
    int zOff = chunkPos.z % cfg.spacing;
    if (xOff < 0)
        xOff += cfg.spacing - 1;
    if (zOff < 0)
        zOff += cfg.spacing - 1;
    free(mt);
    return r1 == xOff && r2 ==
                         zOff;

}

static int nether_fortress_check_1_14(uint32_t worldseed, struct ChunkPos pos) {
    int cx = pos.x >> 4;
    int cz = pos.z >> 4;
    uint32_t seed = worldseed ^(cz << 4) ^cx;
    uint32_t *mt = mt_n_get(seed, 4);
    if (mt[1] % 3 != 0) {
        return 0;
    }
    return pos.x == mt[2] % 8 + 16 * cx + 4 && pos.z == mt[3] % 8 + 16 + cz + 4;

}


static int struct_position_valid(enum BEStructureType type, Layer *g, struct ChunkPos pos) {
    int x = pos.x * 16 + 8;
    int z = pos.z * 16 + 8;
    const size_t SZ = sizeof(enum BiomeID);
    switch (type) {
        case BEVillage:
            return contain_biome_only(g, x, z, 2, VILLAGE_ALLOW_BIOME, sizeof(VILLAGE_ALLOW_BIOME) / SZ);
        case BEOceanMonument:
            if (!contain_biome_only(g, x, z, 16, OCEAN_MONUMENT_ALLOW_SPAWN_BIOME,
                                    sizeof(OCEAN_MONUMENT_ALLOW_SPAWN_BIOME) / SZ)) {
                return 0;
            } else {
                return contain_biome_only(g, x, z, 29, OCEAN_MONUMENT_ALLOW_BIOME,
                                          sizeof(OCEAN_MONUMENT_ALLOW_BIOME) / SZ);
            }
        case BEBuriedTreasure:
            return contain_biome_only(g, x, z, 3, BURIED_TREASURE_ALLOW, sizeof(BURIED_TREASURE_ALLOW) / SZ);
        case BEWoodlandMansion:
            return contain_biome_only(g, x, z, 32, WOODLAND_MANSION_ALLOW, sizeof(WOODLAND_MANSION_ALLOW) / SZ);
        case BEPillagerOutpost:
            return contain_biome_only(g, x, z, 0, PILLAGER_OUTPOST_ALLOW, sizeof(PILLAGER_OUTPOST_ALLOW) / SZ);
        default:
            break;
    }
    return 1;
}

int check_random_scattered(enum BEStructureType type, struct Layer *layer, uint32_t seed, struct ChunkPos pos) {
    if (!structure_check_2(BE_RANDOM_SCATTERED, seed, pos)) {
        return 0;
    }
    int x = pos.x * 16 + 8;
    int z = pos.z * 16 + 8;
    switch (type) {
        case BEDesertTemple:
            return contain_biome_only(layer, x, z, 0, DESERT_TEMPLE_ALLOW, 2);
        case BEJungleTemple:
            return contain_biome_only(layer, x, z, 0, JUNGLE_TEMPLE_ALLOW, 2);
        case BEWitchHut:
            return contain_biome_only(layer, x, z, 0, WITCH_HUT_TEMPLE_ALLOW, 2);
        case BEIgloo:
            return contain_biome_only(layer, x, z, 0, IGLOO_TEMPLE_ALLOW, 2);
        default:
            return 0;
    }

}

int check_position(enum BEStructureType type, struct Layer *layer, uint32_t seed, struct ChunkPos pos) {
    if (type == BEJungleTemple || type == BEDesertTemple || type == BEWitchHut || type == BEIgloo) {
        return check_random_scattered(type, layer, seed, pos);
    }
    switch (type) {
        case BEVillage:
            if (structure_check_1(BE_VILLAGE, seed, pos)) {
                return struct_position_valid(BEVillage, layer, pos);
            } else {
                return 0;
            }
        case BEStronghold:
            return 0;
        case BEMineshaft:
            return 0;
        case BEOceanMonument:
            if (structure_check_1(BE_OCEAN_MONUMENT, seed, pos)) {
                return struct_position_valid(BEOceanMonument, layer, pos);
            } else {
                return 0;
            }
            break;
        case BEOceanRuin:
            break;
        case BEWoodlandMansion:
            if (structure_check_1(BE_WOODLAND_MANSION, seed, pos)) {
                return struct_position_valid(BEWoodlandMansion, layer, pos);
            } else {
                return 0;
            }
            break;
        case BEShipwreck:
            break;
        case BERuindPortal:
            break;
        case BEBuriedTreasure:
            if (structure_check_2(BE_BURIED_TREASURE, seed, pos)) {
                return struct_position_valid(BEBuriedTreasure, layer, pos);
            }
            break;
        case BEPillagerOutpost:
            if (structure_check_1(BE_PILLAGER_OUTPOST, seed, pos)) {
                return struct_position_valid(BEPillagerOutpost, layer, pos);
            }
            break;
        case BENetherFortress:
            return nether_fortress_check_1_14(seed, pos);
        case BEBastion:
            break;
        case BEEndcity:
            break;
        default:
            break;
    }
    return 0;
}


int main() {

//    for (int i = 0; i < L_NUM; i++) {
//        char path[128];
//        sprintf(path, "img/%d.ppm", i);
//        biome_test(100, i, path);
//    }

    // biome_test(100, 53, "a.ppm");
    // range_test(0, 0, 2);
    uint32_t seed = 100;
    LayerStack g;
    setupOverworldGenerator(&g, MC_1_17);
    // Extract the desired layer.
    Layer *layer = &g.layers[L_NUM - 1];
    setLayerSeed(layer, seed);
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 200; j++) {
            struct ChunkPos pos = {i, j};
            if (check_position(BENetherFortress, layer, seed, pos)) {
                printf("%d %d\n", i * 16 + 8, j * 16 + 8);
            }
        }
    }
    return 0;
}


