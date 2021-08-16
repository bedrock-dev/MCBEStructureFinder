#include "generator.h"
#include "layers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Layer *setupLayer(LayerStack *g, int layerId, mapfunc_t *map, int mc,
    int8_t zoom, int8_t edge, uint64_t saltbase, Layer *p, Layer *p2)
{
    Layer *l = g->layers + layerId;
    l->getMap = map;
    l->mc = mc;
    l->zoom = zoom;
    l->edge = edge;
    l->scale = 0;
    if (saltbase == 0)
        l->layerSalt = saltbase;
    else
        l->layerSalt = getLayerSalt(saltbase);
    l->startSalt = 0;
    l->startSeed = 0;
    l->data = NULL;
    l->p = p;
    l->p2 = p2;
    return l;
}

static void setupScale(Layer *l, int scale)
{
    l->scale = scale;
    if (l->p)
        setupScale(l->p, scale * l->zoom);
    if (l->p2)
        setupScale(l->p2, scale * l->zoom);
}

void setupOverworldGenerator(LayerStack *g, int mc)
{
    memset(g, 0, sizeof(LayerStack));
    Layer *p;

    // G: generator layer stack
    // L: layer ID
    // M: mapping function
    // V: minecraft version
    // Z: zoom
    // E: edge
    // S: salt base
    // P1: parent 1
    // P2: parent 2

    //             G, L,                    M               V   Z  E  S     P1 P2
    p = setupLayer(g, L_CONTINENT_4096,     mapContinent,   mc, 1, 0, 1,    0, 0);
    p = setupLayer(g, L_ZOOM_2048,          mapZoomFuzzy,   mc, 2, 3, 2000, p, 0);
    p = setupLayer(g, L_LAND_2048,          mapLand,        mc, 1, 2, 1,    p, 0);
    p = setupLayer(g, L_ZOOM_1024,          mapZoom,        mc, 2, 3, 2001, p, 0);
    p = setupLayer(g, L_LAND_1024_A,        mapLand,        mc, 1, 2, 2,    p, 0);

    //             G  L                 M               V   Z  E  S     P1 P2
    p = setupLayer(g, L_LAND_1024_B,    mapLand,        mc, 1, 2, 50,   p, 0);
    p = setupLayer(g, L_LAND_1024_C,    mapLand,        mc, 1, 2, 70,   p, 0);
    p = setupLayer(g, L_ISLAND_1024,    mapIsland,      mc, 1, 2, 2,    p, 0);
    p = setupLayer(g, L_SNOW_1024,      mapSnow,        mc, 1, 2, 2,    p, 0);
    p = setupLayer(g, L_LAND_1024_D,    mapLand,        mc, 1, 2, 3,    p, 0);
    p = setupLayer(g, L_COOL_1024,      mapCool,        mc, 1, 2, 2,    p, 0);
    p = setupLayer(g, L_HEAT_1024,      mapHeat,        mc, 1, 2, 2,    p, 0);
    p = setupLayer(g, L_SPECIAL_1024,   mapSpecial,     mc, 1, 2, 3,    p, 0);
    p = setupLayer(g, L_ZOOM_512,       mapZoom,        mc, 2, 3, 2002, p, 0);
    p = setupLayer(g, L_ZOOM_256,       mapZoom,        mc, 2, 3, 2003, p, 0);
    p = setupLayer(g, L_LAND_256,       mapLand,        mc, 1, 2, 4,    p, 0);

    p = setupLayer(g, L_BIOME_256,      mapBiome,       mc, 1, 0, 200,  p, 0);
    p = setupLayer(g, L_MUSHROOM_256,   mapMushroom,    mc, 1, 2, 5,    p, 0);
    p = setupLayer(g, L_DEEP_OCEAN_256, mapDeepOcean,   mc, 1, 2, 4,    p, 0);
    
    p = setupLayer(g, L_BAMBOO_256,     mapBamboo,      mc, 1, 0, 1001, p, 0);
    p = setupLayer(g, L_ZOOM_128,       mapZoom,        mc, 2, 3, 1001, p, 0);
    p = setupLayer(g, L_ZOOM_64,        mapZoom,        mc, 2, 3, 1001, p, 0);
    p = setupLayer(g, L_BIOME_EDGE_64,  mapBiomeEdge,   mc, 1, 2, 1000, p, 0);

    // river noise layer chain, also used to determine where hills generate
    p = setupLayer(g, L_RIVER_INIT_256, mapNoise,       mc, 1, 0, 100,
            g->layers+L_DEEP_OCEAN_256, 0);
    p = setupLayer(g, L_ZOOM_128_HILLS, mapZoom,        mc, 2, 3, 0, p, 0);
    p = setupLayer(g, L_ZOOM_64_HILLS,  mapZoom,        mc, 2, 3, 0, p, 0);

    p = setupLayer(g, L_HILLS_64,       mapHills,       mc, 1, 2, 1000,
            g->layers+L_BIOME_EDGE_64, g->layers+L_ZOOM_64_HILLS);

    p = setupLayer(g, L_SUNFLOWER_64,   mapSunflower,   mc, 1, 0, 1001, p, 0);
    p = setupLayer(g, L_ZOOM_32,        mapZoom,        mc, 2, 3, 1000, p, 0);
    p = setupLayer(g, L_LAND_32,        mapLand,        mc, 1, 2, 3,    p, 0);
    p = setupLayer(g, L_ZOOM_16,        mapZoom,        mc, 2, 3, 1001, p, 0);
    p = setupLayer(g, L_SHORE_16,       mapShore,       mc, 1, 2, 1000, p, 0);
    p = setupLayer(g, L_ZOOM_8,         mapZoom,        mc, 2, 3, 1002, p, 0);
    p = setupLayer(g, L_ZOOM_4,         mapZoom,        mc, 2, 3, 1003, p, 0);

    p = setupLayer(g, L_SMOOTH_4,       mapSmooth,      mc, 1, 2, 1000, p, 0);

    // river layer chain
    p = setupLayer(g, L_ZOOM_128_RIVER, mapZoom,        mc, 2, 3, 1001,
            g->layers+L_RIVER_INIT_256, 0);
    p = setupLayer(g, L_ZOOM_64_RIVER,  mapZoom,        mc, 2, 3, 1001, p, 0);
    p = setupLayer(g, L_ZOOM_32_RIVER,  mapZoom,        mc, 2, 3, 1001, p, 0);
    p = setupLayer(g, L_ZOOM_16_RIVER,  mapZoom,        mc, 2, 3, 1001, p, 0);
    p = setupLayer(g, L_ZOOM_8_RIVER,   mapZoom,        mc, 2, 3, 1001, p, 0);
    p = setupLayer(g, L_ZOOM_4_RIVER,   mapZoom,        mc, 2, 3, 1001, p, 0);

    p = setupLayer(g, L_RIVER_4,        mapRiver,       mc, 1, 2, 1,    p, 0);
    p = setupLayer(g, L_SMOOTH_4_RIVER, mapSmooth,      mc, 1, 2, 1000, p, 0);

    p = setupLayer(g, L_RIVER_MIX_4, mapRiverMix, mc, 1, 0, 100,
            g->layers+L_SMOOTH_4, g->layers+L_SMOOTH_4_RIVER);

    // ocean variants
    p = setupLayer(g, L_OCEAN_TEMP_256, mapOceanTemp,   mc, 1, 0, 2,    0, 0);
    p = setupLayer(g, L_OCEAN_EDGE_256, mapOceanEdge,   mc, 1, 2, 2,    p, 0);
    p = setupLayer(g, L_ZOOM_128_OCEAN, mapZoom,        mc, 2, 3, 2002, p, 0);
    p = setupLayer(g, L_ZOOM_64_OCEAN,  mapZoom,        mc, 2, 3, 2002, p, 0);
    p = setupLayer(g, L_ZOOM_32_OCEAN,  mapZoom,        mc, 2, 3, 2002, p, 0);
    p = setupLayer(g, L_ZOOM_16_OCEAN,  mapZoom,        mc, 2, 3, 2002, p, 0);
    p = setupLayer(g, L_ZOOM_8_OCEAN,   mapZoom,        mc, 2, 3, 2002, p, 0);
    p = setupLayer(g, L_ZOOM_4_OCEAN,   mapZoom,        mc, 2, 3, 2002, p, 0);
    p = setupLayer(g, L_OCEAN_MIX_4,    mapOceanMix,    mc, 1, 17, 100,
            g->layers+L_RIVER_MIX_4, g->layers+L_ZOOM_4_OCEAN);

    p = setupLayer(g, L_VORONOI_1, mapVoronoi, mc, 4, 7, 10, p, 0);

    g->entry_1 = p;
    g->entry_4 = g->layers + L_OCEAN_MIX_4;
    g->entry_16 = g->layers + L_SHORE_16;
    g->entry_64 = g->layers + L_SUNFLOWER_64;
    g->entry_256 = g->layers + L_BAMBOO_256;

    setupScale(g->entry_1, 1);
}

/* Recursively calculates the minimum buffer size required to generate an area
 * of the specified size from the current layer onwards.
 */
static void getMaxArea(const Layer *layer, int areaX, int areaZ, int *maxX, int *maxZ)
{
    if (layer == NULL)
        return;

    if (layer->zoom == 2)
    {
        areaX >>= 1;
        areaZ >>= 1;
    }
    else if (layer->zoom == 4)
    {
        areaX >>= 2;
        areaZ >>= 2;
    }

    areaX += layer->edge;
    areaZ += layer->edge;

    if (areaX > *maxX) *maxX = areaX;
    if (areaZ > *maxZ) *maxZ = areaZ;

    getMaxArea(layer->p, areaX, areaZ, maxX, maxZ);
    getMaxArea(layer->p2, areaX, areaZ, maxX, maxZ);
}

int calcRequiredBuf(const Layer *layer, int areaX, int areaZ)
{
    int maxX = areaX, maxZ = areaZ;
    getMaxArea(layer, areaX, areaZ, &maxX, &maxZ);

    return maxX * maxZ;
}

int *allocCache(const Layer *layer, int sizeX, int sizeZ)
{
    int size = calcRequiredBuf(layer, sizeX, sizeZ);

    int *ret = (int *) malloc(sizeof(*ret)*size);
    memset(ret, 0, sizeof(*ret)*size);

    return ret;
}


void applySeed(LayerStack *g, uint64_t seed)
{
    // the seed has to be applied recursively
    setLayerSeed(g->entry_1, seed);
}

int genArea(const Layer *layer, int *out, int areaX, int areaZ, int areaWidth, int areaHeight)
{
    memset(out, 0, areaWidth*areaHeight*sizeof(*out));
    return layer->getMap(layer, out, areaX, areaZ, areaWidth, areaHeight);
}

int genNetherScaled(int mc, uint64_t seed, int scale, int *out,
        int x, int z, int w, int h, int y0, int y1)
{
    if (scale != 1 && scale != 4 && scale != 16 && scale != 64)
        return 1; // unsupported scale

    NetherNoise nn;
    setNetherSeed(&nn, seed);

    if (scale == 1)
    {
        if (y0 != 0 || y1 != 0)
        {
            printf("getNetherScaled(): volume voronoi not implemented yet\n");
            return 1;
        }

        int vx = x - 2;
        int vz = z - 2;
        int pX = vx >> 2;
        int pZ = vz >> 2;
        int pW = ((vx + w) >> 2) - pX + 2;
        int pH = ((vz + h) >> 2) - pZ + 2;

        int err = mapNether2D(&nn, out, pX, pZ, pW, pH);
        if (err)
            return err;
        Layer lvoronoi;
        memset(&lvoronoi, 0, sizeof(Layer));
        lvoronoi.layerSalt = getLayerSalt(10);
        setLayerSeed(&lvoronoi, seed);
        return mapVoronoi(&lvoronoi, out, x, z, w, h);
    }
    else
    {
        return mapNether3D(&nn, out, x, z, w, h, y0, y1-y0+1, scale, 1.0);
    }
}


int genEndScaled(int mc, uint64_t seed, int scale, int *out,
        int x, int z, int w, int h)
{
    if (scale != 1)
        return 1; // unsupported scale

    int i, siz = w*h;
    for (i = 0; i < siz; i++)
        out[i] = the_end;
    return 0;
}





