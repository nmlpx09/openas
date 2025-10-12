#pragma once

#include <cstdint>
#include <vector>
#include <set>
#include <string>

using TData = std::vector<std::uint8_t>;

struct TFormatPermited {
    inline static std::set<std::uint32_t> SampleRate = {44100, 48000, 96000, 192000};
    inline static std::set<std::uint32_t> BitsPerSample = {16, 24};
    inline static std::set<std::uint32_t> NumChannels = {2};
    inline static std::set<std::string> Format = {".flac"};
};

struct TFormat {
    std::uint32_t BitsPerSample = 0;
    std::uint32_t NumChannels = 0;
    std::uint32_t SampleRate = 0;

    bool operator ==(const TFormat& lhs) const = default;
};
