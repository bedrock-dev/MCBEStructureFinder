#include "finder.h"
#include "utils.h"
#include "new_finder.h"

int main(int argc, const char *argv[]) {
//    if (argc != 3) {
//        fprintf(stderr, "use ./find [minseed] [maxseed]");
//        return -1;
//    }
//    uint32_t min = std::strtoul(argv[1], nullptr, 10);
//    uint32_t max = std::strtoul(argv[2], nullptr, 10);
//    if (min >= max) {
//
//        fprintf(stderr, "use ./find [minseed] [maxseed]");
//        return -1;
//    }
//    printf("search range is[%u -- %u]", min, max);
//    init_cached_structure_seed();
//    TIMER_START
//    multi_finder(min, max);
//    TIMER_END
//    printf("%.3lfs used\n", static_cast<double >(timeReslut) / 1000000.0);

//    init_cached_structure_seed();
//    LayerStack g;
//    setupOverworldGenerator(&g, MC_1_17);
//    std::ifstream fin("p3.txt");
//    std::string s;
//    while (!fin.eof()) {
//        getline(fin, s);
//        uint32_t seed = std::strtoul(s.c_str(), nullptr, 10);
//        check_seed<32, 26>(seed, &g, SEARCH_RADIUS, 12, nullptr, 0);
//    }
    StructureFinder finder({0, 0}, 2000, BE_OCEAN_MONUMENT);
    return 0;
}
