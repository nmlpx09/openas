#include "pulse.h"
#include "errors.h"

namespace NRead {

TPulse::TPulse(
    std::string device,
    std::string sampleFormat,
    std::uint8_t numChannels,
    std::uint32_t sampleRate
)
: Device(std::move(device))
, SampleFormat(std::move(sampleFormat))
, NumChannels(numChannels)
, SampleRate(sampleRate) { }

TPulse::~TPulse() {
    if (Simple != nullptr) {
        pa_simple_drain(Simple, nullptr);
        pa_simple_free(Simple);
    }
}

std::error_code TPulse::Init() noexcept {
    pa_sample_format_t format;

    if (NumChannels != 2) {
        return EErrorCode::DeviceInit;
    }

    if (SampleFormat == "s32le") {
        format = PA_SAMPLE_S32LE;
    } else {
        return EErrorCode::DeviceInit;
    }

    if (SampleRate != 48000) {
        return EErrorCode::DeviceInit;
    }

    Spec = {
        .format = format,
        .rate = SampleRate,
        .channels = NumChannels
    };

    if (Simple = pa_simple_new(nullptr, "openas", PA_STREAM_RECORD, Device.c_str(), "pclient", &Spec, nullptr, nullptr, nullptr); !Simple) {
        return EErrorCode::DeviceInit;
    }

    return {};
}

std::expected<TData, std::error_code> TPulse::Read(std::size_t dataSize) const noexcept {
    TData buffer(dataSize, 0);

    if (auto result = pa_simple_read(Simple, buffer.data(), buffer.size(), nullptr); result < 0) {
        return std::unexpected(EErrorCode::Read);
    }

    return buffer;
}

}
