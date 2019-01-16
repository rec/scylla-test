#pragma once

#include <stdlib.h>
#include <algorithm>
#include <array>
#include <vector>

namespace tom {

template <size_t BLOCK_SIZE, typename file_type>
class sorter {
  public:
    sorter(file_type& file, size_t memory) :
            _file(file),
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
    chunk_type _chunk;

    void sort_chunks() {
        while (true) {
            auto const pos = _file.tell();
            auto const count = read_blocks(0, _chunk.size());
            if (!count)
                break;
            _file.seek(pos);

            std::sort(_chunk.begin(), _chunk.begin() + count);
            write_blocks(0, count);
        }
    }

    void merge_chunks() {
        // A K-way merge: file size N, memory size M, and K buffers of size B
        // https://en.wikipedia.org/wiki/External_sorting#External_merge_sort
        auto const M = _chunk.size();
        auto const N = (_file.size() + BLOCK_SIZE - 1) / BLOCK_SIZE;
        auto const K = (N + M - 1) / M;
        auto const B = M / (K + 1);
        auto const OB = M - B * K;

        if (K <= 1)
            return;

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

        auto get_block = [&] (const buffer& buf) {
            return _chunk[buf.block.begin];
        };

        buffer out{{K * B, K * B}, {0, 0}};
        std::vector<buffer> ins;

        ins.reserve(K);
        for (size_t i = 0; i < K; ++i)
            ins.push_back({{i * B, i * B}, {i * M, (i + 1) * M}});

        _file.seek(0);
        while (true) {
            buffer* min = {};
            int min_i = -1;
            for (auto& buf : ins) {
                auto const i = &buf - &ins.front();
                auto& [b, f] = buf;

                if (b.empty() && !f.empty()) {
                    if (true) std::cout << "   seek "
                                        << f.begin * BLOCK_SIZE << '\n';
                    _file.seek(f.begin * BLOCK_SIZE);
                    auto const count = read_blocks(i * B, (i + 1) * B);
                    b = {i * B, i * B + count};
                    f.begin += count;
                }
                if (min)
                    if (false) std::cout << "blocks: "
                          << char(get_block(*min)[0]) << ": "
                          << char(get_block(buf)[0]) << " "
                          << min_i
                          << '\n';
                if (!b.empty() && (!min || get_block(*min) > get_block(buf))) {
                    min = &buf;
                    min_i = i;
                }
            }
            if (min) {
                if (not false) std::cout
                        << "-> min "
                        << (min - &ins[0])
                        << " entry: "
                        << (char) get_block(*min)[0]
                        << '\n';
                _chunk[out.block.end++] = get_block(*min);
                min->block.begin++;
            }

            if (!min || out.block.end - out.block.begin >= M) {
                if (false) std::cout << "writing " << out.block.begin
                           << ", " << out.block.end << '\n';
                _file.seek(out.file.begin * BLOCK_SIZE);
                out.file.begin += write_blocks(out.block.begin, out.block.end);
                out.block.end = K * B;
            }
            if (!min)
                break;
        }
    }

    size_t read_blocks(size_t begin, size_t end) {
        auto t = _file.tell();
        auto i = begin;
        for (; i < end; ++i) {
            auto& block = _chunk[i];
            auto bytes_read = _file.read(&block.front(), BLOCK_SIZE);
            if (bytes_read < BLOCK_SIZE) {
                // Clear a partial block or omit an empty block
                if (bytes_read) {
                    std::fill(block.begin() + bytes_read, block.end(), 0);
                    i++;
                }
                break;
            }
        }
        if (true) std::cout << "   read_blocks  " << begin << ", "
                            << end << " :" << t << '\n';
        return i - begin;
    }

    size_t write_blocks(size_t begin, size_t end) {
        auto t = _file.tell();
        for (auto i = begin; i < end; ++i)
            _file.write(&_chunk[i].front(), BLOCK_SIZE);
        if (true) std::cout << "   write_blocks " << begin << ", " << end << ": "
                            << t << ':' << _file.contents() << '\n';
        return end - begin;
    }
};

}  // namespace tom
