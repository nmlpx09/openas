#pragma once

#include "interface.h"

#include <alsa/asoundlib.h>

#include <common/types.h>

#include <cstdint>
#include <string>

namespace NWrite {

struct TAlsa: TInterface {
public:
    TAlsa(
        std::string device,
        std::string format,
        std::size_t channels,
        std::size_t rate,
        std::size_t bufferSize
    );
    TAlsa(const TAlsa&) = delete;
    TAlsa(TAlsa&&) noexcept = default;
    ~TAlsa();
    TAlsa& operator=(const TAlsa&) = delete;
    TAlsa& operator=(TAlsa&&) = delete;

    std::error_code Init() noexcept override;
    std::error_code Write(TData&& data) noexcept override;

private:
    std::string Device;
    std::string Format;
    std::size_t Channels = 0;
    std::size_t Rate = 0;
    std::size_t BufferSize = 0;
    
    snd_pcm_t* SoundDevice = nullptr;
    snd_pcm_hw_params_t* HwParams = nullptr;
    std::size_t FrameSize = 1;
};

}
