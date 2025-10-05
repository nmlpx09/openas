#include "errors.h"
#include "socket.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

namespace NRead {

TSocket::TSocket(
    std::string ip,
    std::uint16_t port
)
: Ip(std::move(ip))
, Port(port) { }

TSocket::~TSocket() {
    close(Sockfd);
}

std::error_code TSocket::Init() noexcept {
    if (Sockfd = socket(AF_INET, SOCK_DGRAM, 0); Sockfd < 0) {
        return make_error_code(EErrorCode::SocketOpen);
    }

    auto servaddr = sockaddr_in {
        .sin_family = AF_INET,
        .sin_port = htons(Port),
        .sin_addr = {
            .s_addr = inet_addr(Ip.c_str())
        },
        .sin_zero = {0}
    };

    if (bind(Sockfd, reinterpret_cast<const sockaddr*>(&servaddr), sizeof(servaddr)) < 0) {
        return make_error_code(EErrorCode::SocketBind);
    }

    return {};
}

std::expected<TData, std::error_code> TSocket::Read(std::size_t dataSize) const noexcept {
    if (Sockfd < 0) {
        return std::unexpected(EErrorCode::SocketInit);
    }

    if (dataSize < 1) {
        return TData{};
    }

    TData buffer(dataSize);

    auto result = recv(Sockfd, buffer.data(), dataSize, MSG_WAITALL);

    if (result < 0) {
        return std::unexpected(EErrorCode::Read);
    }

    if (result < static_cast<ssize_t>(dataSize)) {
        return TData{};
    }

    return buffer;
}

}
