#include "config.h"

#include <read/flac.h>

#ifdef ALSA
#include <write/alsa.h>
#else
#include <write/pulse.h>
#endif

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <optional>
#include <memory>
#include <mutex>
#include <list>
#include <thread>
#include <vector>

struct TContext {
    std::list<std::tuple<std::chrono::time_point<std::chrono::steady_clock>, TFormat, std::vector<std::uint8_t>>> queue;
    std::mutex mutex;
    std::condition_variable writeCv;
    std::condition_variable readCv;

    bool end = false;
};

using TContextPtr = std::shared_ptr<TContext>;

void Write(TContextPtr ctx, std::string device) noexcept {
    NWrite::TWritePtr write = std::make_unique<NWrite::TWrite>(device);

    auto popQueue = [=] () noexcept -> std::optional<std::pair<TFormat, TData>> {
        std::unique_lock<std::mutex> ulock{ctx->mutex};
        ctx->writeCv.wait(ulock, [ctx] { return !ctx->queue.empty() || ctx->end; });

        if (ctx->end) {
            return std::nullopt;
        }

        auto [date, format, buffer] = ctx->queue.front();
        ctx->queue.pop_front();

        ulock.unlock();
        ctx->readCv.notify_one();

        std::this_thread::sleep_until(date);

        return std::make_optional(std::make_pair(std::move(format), std::move(buffer)));
    };

    if (auto ec = write->Write(popQueue); ec) {
        std::cerr << "write error: " << ec.message() << std::endl;
    }
}

void Read(TContextPtr ctx, std::vector<std::filesystem::path> files) noexcept {
    const auto delta = std::chrono::seconds(1);

    for (const auto& file: files) {
        std::cerr << file.filename().string() << "; ";

        auto time = std::chrono::steady_clock::now() + delta;

        NRead::TReadPtr read = std::make_unique<NRead::TFlac>();

        TFormat format;
        if (auto result = read->Init(file.string()); !result) {
            std::cerr << "read init error: " << result.error().message() << std::endl;
            return;
        } else {
            format = result.value();
        }

        std::cerr << "format: " << format.SampleRate << "Hz " << format.BitsPerSample << "bps " << format.NumChannels << "ch" << std::endl;

        auto pushQueue = [=, &time] (TData data) noexcept {
            std::unique_lock<std::mutex> ulock{ctx->mutex};
            ctx->readCv.wait(ulock, [ctx] { return ctx->queue.empty(); });

            ctx->queue.emplace_back(std::make_tuple(time, format, std::move(data)));
            time += delta;

            ulock.unlock();
            ctx->writeCv.notify_one();
        };

        if (auto ec = read->Read(pushQueue); ec) {
            std::cerr << "read error: " << ec.message() << std::endl;
        }
    }

    ctx->end = true;
    ctx->writeCv.notify_one();
} 

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "run: play dir" << std::endl;
        return 1;
    }

    std::filesystem::path path = std::string{argv[1]};
    std::vector<std::filesystem::path> files;

    if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (TFormatPermited::Format.contains(entry.path().extension())) {
                files.push_back(entry.path());
            }
        }
    } else {
        if (TFormatPermited::Format.contains(path.extension())) {
            files.push_back(path);
        }
    }

    std::string device;
    if (argc == 3) {
        device = std::string{argv[2]};
    } else {
        device = DEVICE;
    }

    std::sort(files.begin(), files.end());

    auto ctx = std::make_shared<TContext>();
    
    std::thread tWrite(Write, ctx, std::move(device));
    std::thread tRead(Read, ctx, std::move(files));

    tRead.join();
    tWrite.join();

    return 0;
}
