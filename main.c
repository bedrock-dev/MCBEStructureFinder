//
// Created by xhy on 2021/8/19.
//
#include "structure.h"
#include "stdio.h"
int main() {
    uint32_t seed = 1;
    LayerStack g;
    setupOverworldGenerator(&g, MC_1_17);
    // Extract the desired layer.
    Layer *layer = &g.layers[L_NUM - 2];
    setLayerSeed(layer, seed);


    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            struct ChunkPos pos = {i, j};
            if (nether_structure_find(BERuindPortal, layer, seed, pos)) {
                printf("%d %d\n", i * 16 + 8, j * 16 + 8);
            }
        }
    }

    return 0;
}
