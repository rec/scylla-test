#pragma once

#include "open_file.hh"
#include "sorter.hh"

namespace tom {

template <size_t BLOCK_SIZE, typename file_type>
void sort(file_type& file, size_t memory) {
    sorter<BLOCK_SIZE, file_type>(file, memory).sort();
}

template <size_t BLOCK_SIZE = 0x1000>
void sort_file(const char* name, size_t memory) {
    open_file file(name);
    sort<BLOCK_SIZE>(file, memory);
}

template <size_t BLOCK_SIZE = 0x1000>
void sort_string(std::string& s, size_t memory) {
    string_file file(s);
    sort<BLOCK_SIZE>(file, memory);
}

}  // namespace tom
