#pragma once

#include <stdlib.h>
#include <algorithm>
#include <array>
#include <vector>

namespace tom {

template <size_t BLOCK_SIZE, typename file_type>
class sorter {
  public:
    bool _debug = false;
    sorter(file_type& file, file_type& tmpfile, size_t memory) :
            _file(file),
            _tmpfile(tmpfile),
            _chunk(memory / BLOCK_SIZE) {
        if (_chunk.size() < 2 && _file.size())
            throw std::runtime_error("Not enough memory to sort file");
    }

    void sort() {
        if (!_chunk.empty()) {
            sort_chunks();
            merge_chunks();
        }
    }

  private:
    using block_type = std::array<uint8_t, BLOCK_SIZE>;
    using chunk_type = std::vector<block_type>;

    file_type& _file;
    file_type& _tmpfile;
    chunk_type _chunk;

    void sort_chunks() {
        // Sort from the input file to the tmpfile
        while (true) {
            auto const begin = _chunk.begin();
            auto const end = begin + read_blocks(_file, begin, _chunk.end());
            if (begin == end)
                break;

            std::sort(begin, end);
            write_blocks(_tmpfile, begin, end);
        }
        _file.seek(0);
        _tmpfile.seek(0);
        // std::cout << "one: '" << _file.contents() << "'\n";
        // std::cout << "two: '" << _tmpfile.contents() << "'\n";
    }

    void merge_chunks() {
        // A K-way merge: file size N, memory size M, and K buffers of size B
        // https://en.wikipedia.org/wiki/External_sorting#External_merge_sort
        auto const M = _chunk.size();
        auto const N = (_file.size() + BLOCK_SIZE - 1) / BLOCK_SIZE;
        auto const K = (N + M - 1) / M;
        auto const B = M / (K + 1);
        auto const OB = M - B * K;

        if (K <= 1) {
            write_blocks(_file, _chunk.begin(), _chunk.end());
            return;
        }

        if (B < 2)
            throw std::runtime_error("Not enough memory to sort file");

        std::cout
                << " M:" << M
                << " N:" << N
                << " K:" << K
                << " B:" << B
                << " OB:" << OB
                << '\n';

        // A range of blocks.
        struct range {
            size_t begin, end;
            bool empty() const { return begin >= end; }
        };

        struct buffer {
            range block;  // Range of blocks within _chunk
            range file;   // Range within a file (in Blocks)
        };

        auto get_block = [&] (const buffer& r) -> block_type& {
            return _chunk[r.block.begin];
        };

        std::vector<buffer> ins;
        ins.reserve(K);
        for (size_t i = 0; i < K; ++i)
            ins.push_back({{i * B, i * B}, {i * M, (i + 1) * M}});

        while (true) {
            buffer* min = {};
            int min_i = -1;
            for (auto& buf : ins) {
                auto const i = &buf - &ins.front();
                auto& [b, f] = buf;

                if (b.empty() && !f.empty()) {
                    if (true) std::cout << "   seek "
                                        << f.begin * BLOCK_SIZE << '\n';
                    _tmpfile.seek(f.begin * BLOCK_SIZE);
                    auto const begin = _chunk.begin() + i * B;
                    auto const end = begin + B;
                    auto const count = read_blocks(_tmpfile, begin, end);
                    if (true) std::cout << "   read_blocks  " << i * B << ", "
                                        << (i + 1) * B << " :"
                                        << f.begin * BLOCK_SIZE << '\n';
                    b = {i * B, i * B + count};
                    f.begin += count;
                }

                if (!b.empty() && (!min || get_block(*min) > get_block(buf))) {
                    min = &buf;
                    min_i = i;
                }
            }

            if (!min)
                break;
            auto min_block = &get_block(*min);
            write_blocks(_file, min_block, min_block + 1);
        }
    }
};

}  // namespace tom
