#pragma once

#include <stdlib.h>
#include <array>

namespace tom {

class open_file {
  public:
    explicit
    open_file(const char* name) {
        _fp = fopen(name, "r");
        if (!_fp)
            throw std::runtime_error(name);
    }

    ~open_file() { fclose(_fp); }

    int seek(off_t offset) {
        return fseeko(_fp, offset, SEEK_SET);
    }

    int read(char* data, size_t length) {
        return fread(data, 1, length, _fp);
    }

    int read(uint8_t* data, size_t length) {
        return read(reinterpret_cast<char*>(data), length);
        // See e.g. https://stackoverflow.com/a/26202784/43839
    }

  private:
    FILE* _fp;
};

}  // namespace tom
