#pragma once

#include <cstdint>
#include <string>

//net
static const std::string IP = "172.16.2.12";
static const std::uint16_t PORT = 1234;
static const std::size_t DATASIZE = 3840;

//pulse
static const std::string DEVICE = "opennap.monitor";
static const std::string FORMAT = "s32le";
static const std::int8_t CHANNELS = 2;
static const std::uint32_t RATE = 48000;
