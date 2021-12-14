//
// Created by xhy on 2021/8/23.
//

#include "utils.h"
#include <cstdarg>
#include <cstdio>

bool Vec2i::operator<(const Vec2i &rhs) const {
    if (x < rhs.x)
        return true;
    if (rhs.x < x)
        return false;
    return z < rhs.z;
}

bool Vec2i::operator==(const Vec2i &rhs) const {
    return x == rhs.x &&
           z == rhs.z;
}

bool Box::operator<(const Box &rhs) const {
    if (min < rhs.min)
        return true;
    if (rhs.min < min)
        return false;
    return max < rhs.max;
}

bool Box::operator==(const Box &rhs) const {
    return min == rhs.min &&
           max == rhs.max;
}

void log(const char *file_name, const char *function_name, size_t line, const char *fmt, ...) {
#ifdef  DEBUG
    va_list args;
    va_start(args, fmt);
    printf("<%s> [%s: %d]  ", file_name, function_name, line);
    vprintf(fmt, args);
    printf("\n");
#endif
}
