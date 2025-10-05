#include "config.h"

#include "read/pulse.h"
#include "write/socket.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <list>
#include <iostream>
#include <thread>

struct TContext {
    std::list<TData> queue;
    std::mutex mutex;
    std::condition_variable cv;
};

using TContextPtr = std::shared_ptr<TContext>;

void Read(NRead::TReadPtr read, TContextPtr ctx) noexcept {
    while (true) {
        if (auto result = read->Read(DATASIZE); !result) {
            std::cerr << "read error: " << result.error().message() << std::endl;
            continue;
        } else {
            std::unique_lock ulock{ctx->mutex};
            ctx->queue.emplace_back(std::move(result).value());
        }
        ctx->cv.notify_one();
    }
}

void Write(NWrite::TWritePtr write, TContextPtr ctx) noexcept {
    while (true) {
        std::unique_lock ulock{ctx->mutex};
        ctx->cv.wait(ulock, [ctx] { return !ctx->queue.empty(); });

        auto data = std::move(ctx->queue.front());
        ctx->queue.pop_front();
        ulock.unlock();

        if (auto ec = write->Write(std::move(data)); ec) {
            std::cerr << "write error: " << ec.message() << std::endl;
        }
    }
}

int main() {
    NRead::TReadPtr read = std::make_unique<NRead::TPulse>(DEVICE, FORMAT, CHANNELS, RATE);
    NWrite::TWritePtr write = std::make_unique<NWrite::TSocket>(IP, PORT);

    if (auto ec = read->Init(); ec) {
        std::cerr << "read init error: " << ec.message() << std::endl;
        return 1;
    }

    if (auto ec = write->Init(); ec) {
        std::cerr << "write init error: " << ec.message() << std::endl;
        return 1;
    }

    auto ctx = std::make_shared<TContext>();

    std::thread tWrite(Write, std::move(write), ctx);
    std::thread tRead(Read, std::move(read), ctx);

    tWrite.join();
    tRead.join();

    return 0;
}
