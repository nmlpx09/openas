#include "config.h"

#include <common/utils.h>
#include <read/socket.h>
#include <write/alsa.h>

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

void Write(NWrite::TWritePtr write, TContextPtr ctx) noexcept {
    bool dropMode = false;
    while(true) {
        std::unique_lock<std::mutex> ulock{ctx->mutex};
        ctx->cv.wait(ulock, [ctx] { return !ctx->queue.empty(); });

        if (dropMode) {
            if (ctx->queue.size() == 1) {
                dropMode = false;
            } else {
                ctx->queue.pop_front();
            }
        } else if (ctx->queue.size() > MAXQUEUESIZE) {
            dropMode = true;
        }

        auto value = ctx->queue.front();
        ctx->queue.pop_front();
        ulock.unlock();

        if (auto ec = write->Write(std::move(value)); ec) {
            std::cerr << ec.message() << std::endl;
        }
    }
}

void Read(NRead::TReadPtr read, TContextPtr ctx) noexcept {
    while(true) {
        if (auto result = read->Read(DATASIZE); result) {
            if (NUtils::isZero(result.value())) {
                continue;
            }
            std::unique_lock ulock{ctx->mutex};
            ctx->queue.emplace_back(std::move(result).value());
            ulock.unlock();
            ctx->cv.notify_one();
        } else {
            std::cerr << result.error().message() << std::endl;
        }
    }
}

int main() {
    NRead::TReadPtr read = std::make_unique<NRead::TSocket>(IP, PORT);
    NWrite::TWritePtr write = std::make_unique<NWrite::TAlsa>(DEVICE, FORMAT, CHANNELS, RATE, DATASIZE);
    
    if (auto ec = write->Init(); ec) {
        std::cerr << ec.message() << std::endl;
        return 1;
    }

    if (auto ec = read->Init(); ec) {
        std::cerr << ec.message() << std::endl;
        return 1;
    }

    auto ctx = std::make_shared<TContext>();

    std::thread tWrite(Write, std::move(write), ctx);
    std::thread tRead(Read, std::move(read), ctx);

    tRead.join();
    tWrite.join();

    return 0;
}
