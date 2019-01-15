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
    std::string s = "none-empty";
    REQUIRE_THROWS(tom::sort_string(s, 0));
}
