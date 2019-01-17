#include <iostream>

#include "open_file.hh"
#include "string_file.hh"
#include "sorter.hh"
#include "sort.hh"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE( "Sort an empty string", "[empty]" ) {
    std::string s;
    tom::sort_string(s, 0);
}

TEST_CASE( "Throws when zero memory", "[throws_zero]" ) {
    std::string s = "non-empty";
    REQUIRE_THROWS(tom::sort_string(s, 0));
}

TEST_CASE( "Throws when memory can't fit two blocks", "[throws_two]" ) {
    std::string s = "non-empty";
    REQUIRE_THROWS(tom::sort_string<1>(s, 1));
}

TEST_CASE( "single element, 2 blocks", "[one_two]" ) {
    std::string s = "1";
    tom::sort_string<1>(s, 2);
    REQUIRE(s == "1");
}

TEST_CASE( "4 elements, 4 blocks", "[four_four]" ) {
    std::string s = "4321";
    tom::sort_string<1>(s, 4);
    REQUIRE(s == "1234");

    tom::sort_string<1>(s, 4);
    REQUIRE(s == "1234");
}

TEST_CASE( "2 blocks, four elements", "[two_four]" ) {
    // Not enough space to do that!
    std::string s = "4321";
    REQUIRE_THROWS(tom::sort_string<1>(s, 2));
}

TEST_CASE( "8 blocks, 16 elements", "[eight_sixteen]" ) {
    std::string s = "PONMLKJIHGFEDCBA";
    tom::sort_string<1>(s, 8);
    REQUIRE(s == "ABCDEFGHIJKLMNOP");

    s = "BOLAKGJPHEMICFND";
    tom::sort_string<1>(s, 8);
    REQUIRE(s == "ABCDEFGHIJKLMNOP");
}

TEST_CASE( "repeats", "[repeats]" ) {
    std::string s = "AAAAAAAAAAAAAAAA";
    tom::sort_string<1>(s, 8);
    REQUIRE(s == "AAAAAAAAAAAAAAAA");

    s = "ABABBABAAABBBBAA";
    tom::sort_string<1>(s, 8);
    REQUIRE(s == "AAAAAAAABBBBBBBB");
}

static const
auto S = "CchVCrbzeJqvwaMaQZiLXuaDjxfsalokIpOAWmCdYUGRENBzHynSKgiPaaTtF";

static const
auto S_SORTED = "ABCCCDEFGHIJKLMNOPQRSTUVWXYZaaaaaabcdefghiijklmnopqrstuvwxyzz";

static const
int MEMORY_SIZES[] = {24, 55};

static const
int BLOCK_SIZES[] = {1, 2, 5, 6, 7, 8, 9};

TEST_CASE("memory", "[memory]" ) {
    for (auto memory : MEMORY_SIZES) {
        auto fmt = "memory: %d, block: %s\n";
        {
            printf(fmt, memory, 1);
            std::string s(S);
            tom::sort_string<1>(s, memory);
            REQUIRE(s == S_SORTED);
        }
        {
            printf(fmt, memory, 2);
            std::string s(S);
            tom::sort_string<2>(s, memory);
            REQUIRE(s == S_SORTED);
        }
        if (memory >= 10) {
            printf(fmt, memory, 5);
            std::string s(S);
            tom::sort_string<5>(s, memory);
            REQUIRE(s == S_SORTED);
        }
    }
}
