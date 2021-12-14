//
// Created by xhy on 2021/8/23.
//
#include <cassert>
#include "utils.h"
#include "test_base.h"
#include <iostream>

void test_between() {
    EXPECT_TRUE(between(0, 12, 3));
    EXPECT_TRUE(between(0, 1, 0));
    EXPECT_TRUE(between(0, 1, 1));
    EXPECT_FALSE(between(0, 4, -1));
}

void test_scala_down() {
    auto p = scala_down({15, 15}, 16);
    auto p_1 = Vec2i{0, 0};
    EXPECT_EQ(p, p_1);
    auto p2 = scala_down({16, 15}, 16);
    auto p2_1 = Vec2i{1, 0};
    EXPECT_EQ(p2, p2_1);
    auto p3 = scala_down({-16, -16}, 16);
    auto p3_1 = Vec2i{-1, -1};
    EXPECT_EQ(p3, p3_1);
    auto p4 = scala_down({-17, -17}, 16);
    auto p4_1 = Vec2i{-2, -2};
    EXPECT_EQ(p4, p4_1);
}

void test_scala_up() {
    auto p = Vec2i{0, 0};
    auto box = scala_up(p, 16);
    auto min = Vec2i{0, 0}, max = Vec2i{15, 15};
    EXPECT_EQ(box.min, min);
    EXPECT_EQ(box.max, max);
    p = Vec2i{-1, -1};
    box = scala_up(p, 32);
    min = {-32, -32};
    max = {-1, -1};
    EXPECT_EQ(box.min, min);
    EXPECT_EQ(box.max, max);
}


void test_get_cong_in_range() {
    EXPECT_EQ(get_cong_with_module(0, 12, 1), 1);
    EXPECT_EQ(get_cong_with_module(2, 2, 3), 3);
    EXPECT_EQ(get_cong_with_module(-12, 7, 3), -4);
    EXPECT_EQ(get_cong_with_module(-1, 1, 1), 0);
}

void test_log() {
    int a = 12;
    LOG("the value of a is %d", a);
}


int main() {
    test_between();
    test_scala_down();
    test_scala_up();
    test_log();

    return 0;
}