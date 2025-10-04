#pragma once

#include <system_error>

namespace NWrite {

enum EErrorCode {
    Ok,
    DeviceOpen,
    DeviceInit,
    InitializeParameter,
    Resampling,
    SetAccess,
    Channels,
    SetChannels,
    Format,
    SetFormat,
    Rate,
    SetRate,
    SetBuffer,
    SetHwParams,
    Prepare
};

}

namespace std {

template<> struct is_error_code_enum<NWrite::EErrorCode> : std::true_type{};

}

namespace NWrite {

class TErrorCategory final : public std::error_category {
public:
    const char* name() const noexcept override {
        return "write error";
    }

    std::string message(int value) const override {
        switch (value) {
            case Ok:
                return "ok";
            case DeviceOpen:
                return "device open error";
            case DeviceInit:
                return "device init error";
            case InitializeParameter:
                return "initialize hardware parameter structure error";
            case Resampling:
                return "resampling setup error";
            case SetAccess:
                return "set access error";
            case Channels:
                return "channels not supported error";
            case SetChannels:
                return "set channel error";
            case Format:
                return "format not supported error";
            case SetFormat:
                return "set format error";
            case Rate:
                return "rate not supported error";
            case SetRate:
                return "set rate error";
            case SetBuffer:
                return "set buffer error";
            case SetHwParams:
                return "set params error";
            case Prepare:
                return "prepare device error";
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
