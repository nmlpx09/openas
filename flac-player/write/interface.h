#pragma once

#include <common/types.h>

#include <functional>
#include <memory>
#include <optional>
#include <system_error>

namespace NWrite {

using TCallback = std::function<std::optional<std::pair<TFormat, TData>>()>;

struct TInterface {
public:
    virtual std::error_code Write(const TCallback&) noexcept = 0;
    virtual ~TInterface() {};
protected:
    virtual std::error_code Init(TFormat) noexcept = 0;
};

using TWritePtr = std::unique_ptr<TInterface>;

}
