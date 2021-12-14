//
// Created by xhy on 2021/8/23.
//

#ifndef CUBIOMES_UTILS_H
#define CUBIOMES_UTILS_H

#include <chrono>
#include <cstring>

#define DEBUG
#ifdef WIN32
#define __FILENAME__ (__builtin_strrchr(__FILE__, '\\') ? __builtin_strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#define LOG(fmt, ...) log(__FILENAME__,__FUNCTION__,__LINE__,fmt,__VA_ARGS__)

typedef std::chrono::high_resolution_clock timer_clock;
#define TIMER_START auto start = timer_clock::now();
#define TIMER_END                                                      \
    auto elapsed = timer_clock::now() - start;                         \
    long long timeReslut =                                             \
        std::chrono::duration_cast<std::chrono::microseconds>(elapsed) \
            .count();

struct Vec2i {
    int x = 0;
    int z = 0;

    bool operator<(const Vec2i &rhs) const;

    bool operator==(const Vec2i &rhs) const;

    constexpr Vec2i(int xx, int zz) : x(xx), z(zz) {}


    Vec2i operator+(const Vec2i &rhs) const {
        return {x + rhs.x, z + rhs.z};
    }

    Vec2i operator-(const Vec2i &rhs) const {
        return {x - rhs.x, z - rhs.z};
    }


    Vec2i() = default;
};

struct Box {
    Vec2i min;
    Vec2i max;

    bool operator<(const Box &rhs) const;

    bool operator==(const Box &rhs) const;
};

typedef Vec2i ChunkPos;
typedef Vec2i Pos;

template<typename T>
inline bool between(const T &min, const T &max, const T &x) {
    return x >= min && x <= max;
}

inline Pos scala_down(const Vec2i &origin, int SCALA) {
    auto cx = origin.x < 0 ? origin.x + 1 - SCALA : origin.x;
    auto cz = origin.z < 0 ? origin.z + 1 - SCALA : origin.z;
    return {cx / SCALA, cz / SCALA};
}

inline Box scala_up(const Pos &origin, int SCALA) {
    auto x = origin.x * SCALA;
    auto z = origin.z * SCALA;
    return {
            {x,             z},
            {x + SCALA - 1, z + SCALA - 1}
    };
}

//get the fisrt number(>= begin) which === remain (mod module)
//返回区间[begin,+inf)中和remain同余的最小的数(模modulo)
inline int get_cong_with_module(int begin, int modulo, int remain) {
    int r = begin - (begin % modulo);
    if (begin > 0)
        r += modulo;
    auto c1 = r + remain;
    return between(begin, begin + modulo, c1) ? c1 : c1 - modulo;
}


void log(const char *file_name, const char *function_name, size_t line, const char *fmt, ...);

#endif //CUBIOMES_UTILS_H
