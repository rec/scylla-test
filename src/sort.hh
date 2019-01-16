#pragma once

#include "open_file.hh"
#include "sorter.hh"

namespace tom {

template <size_t BLOCK_SIZE, typename file_type>
void sort(file_type& file, file_type& tmpfile, size_t memory) {
    sorter<BLOCK_SIZE, file_type>(file, tmpfile, memory).sort();
}

template <size_t BLOCK_SIZE = 0x1000>
void sort_file(const char* name, size_t memory) {
    auto tmp = std::string(name) + ".tmp";
    open_file file(name), tmp_file(tmp.c_str());

    sort<BLOCK_SIZE>(file, tmp_file, memory);
}

template <size_t BLOCK_SIZE = 0x1000>
void sort_string(std::string& s, size_t memory) {
    std::string tmp(s.size(), '*');
    string_file file(s), tmp_file(tmp);

    sort<BLOCK_SIZE>(file, tmp_file, memory);
}

}  // namespace tom
