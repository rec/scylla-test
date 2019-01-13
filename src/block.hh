#pragma once

#include <stdlib.h>
#include <array>

#include "open_file.h"

namespace tom {

template <size_t SIZE, typename file_type = open_file>
class blocks_file {
    using block_type = std::array<uint8_t, SIZE>;
    using blocks_type = std::vector<block_type>;

    blocks_file(const char* name, position_type available_memory) :
        _file(name),
        _blocks(get_memory_blocks(available_memory)) {
    }

    void sort_external() {
        sort_chunks();
        merge();
    }

  private:
    file_type _file;
    blocks_type _blocks;

    position_type get_memory_blocks(position_type available_memory) const {
        auto file_chunks = (_file.size() + SIZE - 1) / SIZE;
        auto memory_chunks = available_memory / SIZE;
        return (file_chunks + memory_chunks - 1) / memory_chunks;
    }

    size_t read_blocks() {
        for (size_t i = 0; i < _blocks.size(); ++i) {
            auto& block = _blocks[i];
            auto bytes_read = _file.read(&block.front(), SIZE);
            if (!bytes_read)
                return i;

            if (bytes_read < SIZE) {
                std::fill(block.begin() + bytes_read, block.end(), 0);
                return i + 1;
            }
        }
        return _blocks.size();
    }

    void write_blocks(size_t size) {
        for (size_t i = 0; i < size; ++i)
            _file.write(_blocks[i].front());
    }

    void sort_chunks(int frame_size) {
        _file.seek(0);

        while (true) {
            {
                auto r = _file.restore_position();
                if (auto count = read_blocks())
                    std::sort(_blocks.begin(), _blocks.begin() + count);
                else
                    return;
            }
            write_blocks();
        }
    }

    void merge() {
    }
};

}  // namespace tom


/*
    static blocks_type read_file(const char* name, position_type begin, position_type end) {
        static const auto MAX_OFFSET = std::numeric_limits<position_type>::max() / SIZE;
        if (begin > MAX_OFFSET || end > MAX_OFFSET)
            throw std::runtime_error("Offset is greater than max file size");

        file_type file(name);
        position_type size = 0;

        if (!file.seek(begin * SIZE))
            size = end - begin;

        blocks_type results(size);
        for (auto i = 0; i < results.size(); ++i) {
            if (file.read(results[i].front(), SIZE))
                results.resize(i);
        }
        return results;
    }

 */
