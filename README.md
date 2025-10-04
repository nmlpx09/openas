# about

open audio system c++23

pulse-client captured audio stream from pulseaudio and received it to alsa-server

# requirement

1. clang-20
2. make
3. libasound2-dev
4. libpulse0

# build

make

# install

make install

# configure pulse client

in file default.pa `/etc/pulse/default.pa` add:

`
load-module module-null-sink sink_name=openas sink_properties=device.description=Openas
`

and set in audio mixer default output on Openas
