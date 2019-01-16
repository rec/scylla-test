#pragma once

#include <stddef.h>

namespace tom {

template <typename file_type, typename Iterator>
size_t read_blocks(file_type& file, Iterator begin, Iterator end) {
    for (auto i = begin; i < end; ++i) {
        auto bytes_read = file.read(&i->front(), i->size());
        if (bytes_read < i->size()) {
            // Clear a partial block or omit an empty block
            if (bytes_read) {
                std::cout << "read\n";
                std::fill(i->begin() + bytes_read, i->end(), 0);
                ++i;
            } else {
                std::cout << "NOT read\n";
            }

            std::cout << "no data " << (i - begin) << "\n";
            return i - begin;
        }
    }
    std::cout << "DATA exit " << (end - begin) << "\n";
    return end - begin;
}

template <typename file_type, typename Iterator>
size_t write_blocks(file_type& file, Iterator begin, Iterator end) {
    for (auto i = begin; i < end; ++i)
        file.write(&i->front(), i->size());
    return end - begin;
}

}  // namespace tom
