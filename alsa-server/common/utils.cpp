#include "utils.h"

#include <cstring>

namespace NUtils {

bool isZero(const TData& data) {
    if (data.size() == 0) {
        return true;
    }
    const auto* pointer = reinterpret_cast<const std::uint8_t*>(data.data());
    const auto size = data.size() * sizeof(TData::value_type);
    return data.front() == 0 && !std::memcmp(pointer, pointer + 1, size - 1);
}

}
