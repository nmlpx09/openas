#pragma once

#include <cstdint>
#include <string>

//net
static const std::string IP = "0.0.0.0";
static const std::uint16_t PORT = 1234;
static const std::size_t DATASIZE = 3840;
static const std::size_t MAXQUEUESIZE = 20;

//alsa
static const std::string DEVICE = "iec958:CARD=Q20,DEV=0";
static const std::string FORMAT = "s32le";
static const std::size_t CHANNELS = 2;
static const std::size_t RATE = 48000;
