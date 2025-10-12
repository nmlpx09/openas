# about

open network audio player

1. pulse-client captured audio stream from pulseaudio and received it to network
2. alsa-server aptured audio stream from network and received it to alsa
3. flac audio player for 192000, 96000, 48000, 44100 Hz 16 or 24 bit stereo format supported alsa and pulseaudio

# requirement

1. clang-20
2. make
3. libasound2-dev
4. libpulse0
5. libflac++10

# build

make

# install

make install

# configure pulse client

in file default.pa `/etc/pulse/default.pa` add:

`
load-module module-null-sink sink_name=opennap sink_properties=device.description=Opennap
`

and set in audio mixer default output on Opennap
