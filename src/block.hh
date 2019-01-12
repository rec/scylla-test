#pragma once

#include <stdlib.h>
#include <array>

#include "open_file.h"

namespace tom {

template <size_t SIZE>
struct block_type {
    using data_type = std::array<uint8_t, SIZE>;
    using vector_type = std::vector<data_type>;

    static vector_type read_file(const char* name, off_t begin, off_t end) {
        static const auto MAX_OFFSET = std::numeric_limits<off_t>::max() / SIZE;
        if (begin > MAX_OFFSET || end > MAX_OFFSET)
            throw std::runtime_error("Offset is greater than max file size");

        open_file file(name);
        off_t size = 0;

        if (!file.seek(begin * SIZE))
            size = end - begin;

        vector_type results(size);
        for (auto i = 0; i < results.size(); ++i) {
            if (file.read(results[i].front(), SIZE))
                results.resize(i);
        }
        return results;
    }
};
