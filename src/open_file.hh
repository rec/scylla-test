#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

namespace tom {

/**
   A file with a size whose bytes are written and read as uint8_t.

   We use reinterpret_cast to convert back and forth between uint8_t* and char*:
   see e.g. https://stackoverflow.com/a/26202784/43839

 */
class open_file {
  public:
    open_file(const char* name) : _fp(open(name)), _size(get_size()) {}
    ~open_file() { fclose(_fp); }

    size_t size() const { return _size; }
    off_t tell() const { return ftello(_fp); }

    int read(uint8_t* data, size_t length) {
        auto cdata = reinterpret_cast<char*>(data);
        return fread(cdata, 1, length, _fp);
    }

    int write(uint8_t const* data, size_t length) {
        auto cdata = reinterpret_cast<const char*>(data);
        return fwrite(cdata, 1, length, _fp);
    }

    int seek(off_t offset, int whence = SEEK_SET) {
        return fseeko(_fp, offset, whence);
    }

  private:
    FILE* const _fp;
    size_t const _size;

    FILE* open(const char* name) {
        if (auto fp = fopen(name, "r+"))
            return fp;
        throw std::runtime_error(name);
    }

    size_t get_size() {
        seek(0, SEEK_END);
        auto result = tell();
        seek(0);
        return result;
    }
};

}  // namespace tom
