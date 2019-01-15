#pragma once

#include <stdlib.h>
#include <algorithm>
#include <array>
#include <vector>

namespace tom {

template <typename file_type, size_t BLOCK_SIZE = 0x1000>
class sorter {
  public:
    sorter(file_type& file, size_t memory) :
            _file(file),
            _chunk(memory / BLOCK_SIZE) {
    }

    void sort() {
        sort_chunks();
        merge_chunks();
    }

  private:
    using block_type = std::array<uint8_t, BLOCK_SIZE>;
    using chunk_type = std::vector<block_type>;

    // A range of blocks.
    struct range {
        size_t begin, end;
        bool empty() const { return begin >= end; }
    };

    struct buffer {
        range block;  // Range of blocks within _chunk
        range file;   // Range within a file (in Blocks)
    };

    file_type& _file;
    chunk_type _chunk;

    buffer _output;
    std::vector<buffer> _inputs;

    void sort_chunks() {
        while (true) {
            auto const pos = _file.tell();
            auto const count = read_blocks(0, _chunk.size());
            if (!count)
                break;
            _file.seek(pos);

            std::sort(_chunk.begin(), _chunk.begin() + count);
            write_blocks(_chunk.begin(), _chunk.begin() + count);
        }
    }

#if 1
    // void sort_chunks() {}
    void merge_chunks() {}
#else
    void merge_chunks() {
        // A K-way merge: file size N, memory size M, and K buffers of size B
        // https://en.wikipedia.org/wiki/External_sorting#External_merge_sort
        auto const M = _chunk.size();
        auto const N = (_file.size() + BLOCK_SIZE - 1) / BLOCK_SIZE;
        auto const K = (N + M - 1) / M;
        auto const B = M / (K + 1);

        _inputs.reserve(K);
        for (size_t i = 0; i < K; ++i)
            _inputs.push_back({{i * B, i * B}, {i * M, (i + 1) * M}});

        _output = {{K * B, M}, {0, 0}};

        while (true) {
            buffer* min = {};
            for (auto& [b, f] : _inputs) {
                if (b.empty() && !f.empty()) {
                    _file.seek(f.begin * BLOCK_SIZE);
                    auto read = read_blocks(b);
                    f.begin += read;
                    b.end += read;
                }
                if (!b.empty() && (!min || get_block(*min) > get_block(i)))
                    min = &i;
            }
            if (min) {
                get_block(_output) = get_block(*min);
                min->block.begin++;
                _output.block.begin++;
            }
            if (!min || _output.block.empty()) {
                _file.seek(output.file.begin * BLOCK_SIZE);
                output.file.begin += write_blocks(output.chunk);
                output.chunk.begin = K * B;
            }
            if (!min)
                break;
        }
    }

    block_type& get_block(buffer const& buf) {
        return _chunk[buf.block.begin];
    }

    size_t read_blocks(range r) {
        auto i = r.begin;
        for (; i != r.end; ++i) {
            auto& block = _chunk[i];
            auto bytes_read = _file.read(&block.front(), BLOCK_SIZE);
            if (bytes_read < BLOCK_SIZE) {
                // Clear a partial block or omit an empty block
                if (bytes_read)
                    std::fill(block.begin() + bytes_read, block.end(), 0);
                else
                    i--;
                break;
            }
        }
        return i - r.begin;
    }

    size_t write_blocks(range r) {
        for (auto i = r.begin; i < r.end; ++i)
            _file.write(_chunk[i].front());
        return r.end - r.begin;
    }
#endif
};

}  // namespace tom
