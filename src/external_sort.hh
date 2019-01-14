#pragma once

#include <stdlib.h>
#include <array>

#include "chunk_file.h"

namespace tom {

template <typename chunk_file_type>
class external_sort {
  public:
    external_sort(chunk_file_type& file) : _file(file) {}

    void sort() {
        sort_each_chunk();
        merge_chunks();
    }

  private:
    chunk_file_type& _file;

    void sort_each_chunk() {
        auto const begin = _file.chunk().begin();
        while (true) {
            auto const pos = _file.tell();
            auto const chunk_size = _file.chunk().size();
            if (auto const count = read_blocks(0, chunk_size))
                std::sort(begin, begin + count);
                _file.seek(pos);
                write_blocks(begin, begin + count);
            } else {
                return;
            }
        }
    }

    void merge_chunks() {
        struct buffer {
            size_t begin, end;     // in blocks
            size_t file_position;  // in bytes

            bool empty() const { return begin == end; }
        };

        auto const file_blocks = (_file.size() + SIZE - 1) / SIZE;
        auto const ratio = (file_blocks + _blocks.size() - 1) / _blocks.size();
        auto const in_size = _blocks.size() / (ratio + 1);

        std::vector<buffer> inputs;
        inputs.reserve(ratio);

        for (size_t i = 0; i < ratio; ++i) {
            auto const begin = i * in_size;
            auto const file = i * _blocks.size();
            inputs.push_back({begin, begin, file, false});
        }
        buffer output{in_size * ratio, in_size * ratio, 0, false};

        for (bool running = true; running;) {
            blocks_type* found = {};
            for (auto& i : ins) {
                if (i.begin == i.end) {
                    if (i.file_position >= _file.size())
                        continue;
                    _file.seek(i.file_position);

                    auto blocks_read = read_blocks(i.begin
            }
            for (auto& buf : input_buffers) {
                if (!buf.finished) {
                    if (buf.block >= buffer_size) {
                        buf.file += buffer_size;
                        buf.block = 0;
                        Buf.finished = _file.seek(buf.file * SIZE);
                        if (!buf.finished) {
                    }
                }
        }

        auto read = [] (buffer& buf) {
            if (buf.begin != buf.end)
                return true;

            if (buf.file.empty())
                return false;

            return true;
        };

        auto write = [] (buffer& buf) {
           if (buf.begin != buf.end) {
               _file.seek(file);

        };

            bool read() {
            }

            void write() {
                }
            }
        };

        auto const file_blocks = (_file.size() + SIZE - 1) / SIZE;
        auto const ratio = (file_blocks + _blocks.size() - 1) / _blocks.size();
        auto const input_size = _blocks.size() / (ratio + 1);

        auto const output_begin = input_size * ratio

        buffer output_buffer = {output_begin, output_begin, 0, false};
        std::vector<buffer> input_buffers;
        input_buffers.reserve(ratio);
        for (size_t i = 0; i < ratio; ++i) {
            auto const begin = i * input_size;
            auto const file = i * _blocks.size();
            inputs.push_back({begin, begin, file, false});
            read(inputs.back());
        }

        for (bool running = true; running; ) {
            block_type* found = {};
            for (auto& buf : input_buffers) {
                if (!buf.finished) {
                    if (buf.block >= buffer_size) {
                        buf.file += buffer_size;
                        buf.block = 0;
                        buf.finished = _file.seek(buf.file * SIZE);
                        if (!buf.finished) {

                    }
                }
            }

        }
    }
};

}  // namespace tom
