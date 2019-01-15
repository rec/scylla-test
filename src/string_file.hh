#pragma once

#include <stddef.h>
#include <string>

namespace tom {

/** Like open_file but backed by std::string, for testing */
class string_file {
  public:
    string_file(std::string&& s) : _contents(std::move(s)) {}

    size_t size() const { return _contents.size(); }
    off_t tell() const { return _position; }

    int read(uint8_t* data, size_t length) {
        auto const to_copy = std::min(length, size() - _position);
        auto const cdata = reinterpret_cast<char*>(data);
        auto const begin = _contents.begin() + _position;

        std::copy(begin, begin + to_copy, data);
        return static_cast<int>(to_copy);
    }

    int write(uint8_t const* data, size_t length) {
        auto const to_copy = std::min(length, size() - _position);
        auto cdata = reinterpret_cast<const char*>(data);
        std::copy(data, data + to_copy, _contents.begin() + _position);
        return static_cast<int>(to_copy);
    }

    int seek(off_t offset, int whence = SEEK_SET) {
        if (whence == SEEK_END)
            offset += _contents.size();
        else if (whence == SEEK_CUR)
            offset += _position;

        offset = std::max(off_t(0), offset);
        _position = static_cast<size_t>(offset);
        _position = std::min(_contents.size(), _position);

        return 0;
    }

  private:
    std::string _contents;
    size_t _position;
};


}  // namespace tom
