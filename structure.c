//
// Created by xhy on 2021/8/3.
//

#include "structure.h"

#include "cubiomes/layers.h"
#include "cubiomes/generator.h"
#include "cubiomes/util.h"
#include "bedrng.h"
#include <stdio.h>
#include <math.h>

static int max(int x, int z) {
    return x < z ? z : x;
}

static int get_biome_at_pos(Layer *layer, int x, int z) {
    int *ids = allocCache(layer, 1, 1);
    genArea(layer, ids, x, z, 1, 1);
    int biomeID = ids[0];
    free(ids);
    return biomeID;
    return 0;
}

static int find_in_array(int target, const int biomes[], size_t n) {
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
    //  Original: use a 4*4 resolution to check
    int x0 = (px - r) >> 2;
    int z0 = (pz - r) >> 2;
    int w = ((r + px) >> 2) - x0 + 1;
    int h = ((r + pz) >> 2) - z0 + 1;
    int *biomeIds = allocCache(layer, w, h);
    genArea(layer, biomeIds, x0, z0, w, h);
    const int area = w * h;
    for (int i = 0; i < area; i++) {
        if (!find_in_array(biomeIds[i], filter, n)) {
            return 0;
        }
    }
    return 1;
}


int is_valid_spawn_biome(enum BiomeID biome_id) {
    return find_in_array(biome_id, WORLD_SPAWN_ALLOW_BIOME, sizeof(WORLD_SPAWN_ALLOW_BIOME) / BSZ);
}

//thanks for ddf
//There is an error within tens of meters
struct ChunkPos find_spawn_position(uint32_t seed) {
    LayerStack g;
    setupOverworldGenerator(&g, MC_1_17);
    Layer *layer = &g.layers[L_NUM - 2];
    setLayerSeed(layer, seed);
    struct ChunkPos pos = {-1, -1};
    int step = 0;
    while (1) {
        int *biomeIds = allocCache(layer, 10, 10);
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


int structure_location_check_1(const struct BEStructureConfig cfg, uint32_t worldSeed, struct ChunkPos chunkPos) {
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

int structure_location_check_2(const struct BEStructureConfig cfg,
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

static int check_nether_fortress__1_14(uint32_t worldseed, struct ChunkPos pos) {
    int cx = pos.x >> 4;
    int cz = pos.z >> 4;
    uint32_t seed = worldseed ^(cz << 4) ^cx;
    uint32_t *mt = mt_n_get(seed, 4);
    if (mt[1] % 3 != 0) {
        return 0;
    }
    return pos.x == mt[2] % 8 + 16 * cx + 4 && pos.z == mt[3] % 8 + 16 + cz + 4;

}

static int check_ship_wreak() {
    return 1;
}

static int check_ocean_ruin() {
    return 1;
}


int check_random_scattered(enum BEStructureType type, struct Layer *layer, uint32_t seed, struct ChunkPos pos) {
    if (!structure_location_check_2(BE_RANDOM_SCATTERED, seed, pos)) {
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


int check_ruined_portal() {
    return 1;
}

//error
int check_minshaft(uint32_t seed, struct ChunkPos pos) {
    uint32_t *mt = mt_n_get(seed, 2);
    uint32_t chunk_seed = seed ^(mt[1] * pos.z) ^(mt[0] * pos.x);
    uint32_t *mt2 = mt_n_get(chunk_seed, 3);
    double rf = int_2_float(mt2[1]);
    if (rf >= 0.004) return 0;
    return mt2[2] % 80 < max(abs(pos.x), abs(pos.z));
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

int nether_structure_find(enum BEStructureType type, Layer *layer, uint32_t worldSeed, struct ChunkPos chunkPos) {
    if (type == BERuindPortal) {
        return structure_location_check_2(BE_RUIN_PORTAL_NETHER, worldSeed, chunkPos);
    }
    if (type != BEBastion && type != BENetherFortress) {
        return 0;
    }
    struct BEStructureConfig cfg = BE_NETHER_STRUCTURE;
    if (chunkPos.x < 0)
        chunkPos.x -= cfg.spacing - 1;
    if (chunkPos.z < 0)
        chunkPos.z -= cfg.spacing - 1;
    uint32_t seed = cal_structure_seed(worldSeed, cfg.salt, chunkPos.x / cfg.spacing, chunkPos.z / cfg.spacing);

    uint32_t *mt = mt_n_get(seed, 3);
    uint32_t r1 = mt[0] % cfg.spawnRange;
    uint32_t r2 = mt[1] % cfg.spawnRange;
    int xOff = chunkPos.x % cfg.spacing;
    int zOff = chunkPos.z % cfg.spacing;
    if (xOff < 0)
        xOff += cfg.spacing - 1;
    if (zOff < 0)
        zOff += cfg.spacing - 1;
    free(mt);
    if (r1 == xOff && r2 == zOff) {
        if (mt[2] % 6 >= 2) {
            return BEBastion;
        } else {
            return BENetherFortress;
        }
    } else {
        return 0;
    }

}

int overworld_structure_find(enum BEStructureType type, struct Layer *layer, uint32_t seed, struct ChunkPos pos) {
    if (type == BEJungleTemple || type == BEDesertTemple || type == BEWitchHut || type == BEIgloo) {
        return check_random_scattered(type, layer, seed, pos);
    }
    switch (type) {
        case BEVillage:
            if (structure_location_check_1(BE_VILLAGE, seed, pos)) {
                return struct_position_valid(BEVillage, layer, pos);
            } else {
                return 0;
            }
        case BEStronghold:
            return 0;
        case BEMineshaft:
            return check_minshaft(seed, pos);
        case BEOceanMonument:
            if (structure_location_check_1(BE_OCEAN_MONUMENT, seed, pos)) {
                return struct_position_valid(BEOceanMonument, layer, pos);
            } else {
                return 0;
            }
        case BEOceanRuin:
            if (structure_location_check_1(BE_OCEAN_RUIN, seed, pos)) {
                return struct_position_valid(BEOceanRuin, layer, pos);
            } else {
                return 0;
            }
        case BEWoodlandMansion:
            if (structure_location_check_1(BE_WOODLAND_MANSION, seed, pos)) {
                return struct_position_valid(BEWoodlandMansion, layer, pos);
            } else {
                return 0;
            }
        case BEShipwreck:
            if (structure_location_check_1(BE_SHIP_WREAK, seed, pos)) {
                return struct_position_valid(BEShipwreck, layer, pos);
            } else {
                return 0;
            }
        case BERuindPortal:
            return structure_location_check_2(BE_RUIN_PORTAL_OVERWORLD, seed, pos);
        case BEBuriedTreasure:
            if (structure_location_check_2(BE_BURIED_TREASURE, seed, pos)) {
                return struct_position_valid(BEBuriedTreasure, layer, pos);
            }
            break;
        case BEPillagerOutpost:
            if (structure_location_check_1(BE_PILLAGER_OUTPOST, seed, pos)) {
                return struct_position_valid(BEPillagerOutpost, layer, pos);
            } else {
                return 0;
            }
        case BEEndcity:
            return 0;
        default:
            return 0;
    }
    return 0;
}


struct ChunkPos *generate_stronghold_positions(uint32_t seed, struct Layer *layer) {
    struct ChunkPos *positions = malloc(sizeof(struct ChunkPos) * 3);
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
                //village near
                struct ChunkPos pos = {chunkX, chunkZ};
                if (overworld_structure_find(BEVillage, layer, seed, pos)) {
                    found = 1;
                    positions[count].x = chunkX * 16;
                    positions[count].z = chunkZ * 16;
                    ++count;
                    if (count == 3) {
                        return positions;
                    }
                    break;
                }
            }
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
