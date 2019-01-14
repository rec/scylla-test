#pragma once

#include <stdlib.h>
#include <array>

#include "open_file.h"

namespace tom {

template <typename file_type = open_file, size_t BLOCK_SIZE = 0x1000>
class chunk_file {
  public:
    static const auto SIZE = BLOCK_SIZE;
    using block_type = std::array<uint8_t, SIZE>;
    using chunk_type = std::vector<block_type>;

    chunk_file(const char* name, size_t memory) :
            _chunk(memory / SIZE),
            _file(file_type(name)) {
    }

    size_t read_chunk(size_t begin, size_t end) {
        for (auto i = begin; i != end; ++i) {
            auto& block = _chunk[i];
            if (auto bytes_read = _file.read(&block.front(), SIZE)) {
                if (bytes_read < SIZE) {
                    // Clear the remainder of this partial block
                    std::fill(block.begin() + bytes_read, block.end(), 0);
                    return i - begin;
                }
            } else {
                // Empty block
                return i - begin - 1;
            }
        }
        return end - begin;
    }

    void write_chunk(size_t begin, size_t end) {
        for (auto i = begin; i < end; ++i)
            _file.write(_chunk[i].front());
    }

    int seek(off_t offset) {
        return _file.seek(offset);
    }

    chunk_type& chunk() { return _chunk; }

  private:
    chunk_type _chunk;
    file_type _file;
};

}  // namespace tom
