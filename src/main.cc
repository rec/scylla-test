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

TEST_CASE( "Throws when memory can't fit two blocks", "[throws_zero]" ) {
    std::string s = "non-empty";
    REQUIRE_THROWS(tom::sort_string<1>(s, 1));
}

TEST_CASE( "Blocks of size 1, single element", "[one_one]" ) {
    std::string s = "1";
    tom::sort_string<1>(s, 2);
    REQUIRE(s == "1");
}

TEST_CASE( "2 blocks of size 1, four elements", "[one_two]" ) {
    std::string s = "4321";
    tom::sort_string<1>(s, 2);
    REQUIRE(s == "1234");

    tom::sort_string<1>(s, 2);
    REQUIRE(s == "1234");
}

TEST_CASE( "4 blocks of size 1, four elements", "[one_four]" ) {
    std::string s = "4321";
    tom::sort_string<1>(s, 4);
    REQUIRE(s == "1234");

    tom::sort_string<1>(s, 4);
    REQUIRE(s == "1234");
}
