#pragma once

#include <common/types.h>

#include <cstdint>
#include <expected>
#include <memory>
#include <system_error>

namespace NRead {

struct TInterface {
public:
    virtual std::error_code Init() noexcept = 0;
    virtual std::expected<TData, std::error_code> Read(std::size_t dataSize) const noexcept = 0;
    virtual ~TInterface() {};
};

using TReadPtr = std::unique_ptr<TInterface>;

}
