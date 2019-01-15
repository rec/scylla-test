#pragma once

#include "open_file.hh"
#include "sorter.hh"

namespace tom {

template <size_t BLOCK_SIZE = 0x1000>
void sort(const char* name, size_t memory) {
    sorter(open_file(name), memory).sort();
}

}  // namespace tom
