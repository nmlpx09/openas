#pragma once

#include <system_error>

namespace NRead {

enum EErrorCode {
    Ok,
    DeviceInit,
    Channels,
    Format,
    Rate,
    Read
};

}

namespace std {

template<> struct is_error_code_enum<NRead::EErrorCode> : std::true_type{};

}

namespace NRead {

class TErrorCategory final : public std::error_category {
public:
    const char* name() const noexcept override {
        return "read error";
    }

    std::string message(int value) const override {
        switch (value) {
            case Ok:
                return "ok";
            case Channels:
                return "channels not supported error";
            case Format:
                return "format not supported error";
            case Rate:
                return "rate not supported error";
            case DeviceInit:
                return "device init error";
            case Read:
                return "read ended with error";
        }

        return "unknown error code: " + std::to_string(value);
    }

    static const TErrorCategory& instance() {
        static const TErrorCategory errorCategory;
        return errorCategory;
    }
};

inline std::error_code make_error_code(EErrorCode errorCode) {
    return {errorCode, TErrorCategory::instance()};
}

}
