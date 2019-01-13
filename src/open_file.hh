#pragma once

#include <stdlib.h>
#include <array>

namespace tom {

/**
   A potentially very large file whose bytes are written and read as uint8_t.
   We use reinterpret_cast to convert back and forth between uint8_t* and char*:
   see e.g. https://stackoverflow.com/a/26202784/43839

 */
class open_file {
  public:
    using position_type = off_t;

    explicit
    open_file(const char* name, const char* mode = "r+") {
        _fp = fopen(name, mode);
        if (!_fp)
            throw std::runtime_error(name);
    }

    ~open_file() {
        fclose(_fp);
    }

    int read(uint8_t* data, size_t length) {
        auto cdata = reinterpret_cast<char*>(data);
        return fread(cdata, 1, length, _fp)
    }

    int write(uint8_t const* data, size_t length) {
        auto cdata = reinterpret_cast<char*>(data);
        return fwrite(cdata, 1, length, _fp)
    }

    int seek(position_type offset, int whence = SEEK_SET) {
        return fseeko(_fp, offset, whence);
    }

    position_type size() {
        auto r = restore_position();
        seek(0, SEEK_END);
        return ftello(_fp);
    }

    /** @return an object that stores the current position in the file, and
        restores it on destruction. */
    auto restore_position() {
        return restorer_type(*this);
    }

    class restorer_type {
      public:
        restorer(open_file& f) : _file(f), _pos(f.tell()) {}
        ~restorer() { _file.seek(_pos); }

      private:
        open_file& _file;
        position_type _pos;
    };

  private:
    FILE* _fp;
};

}  // namespace tom
