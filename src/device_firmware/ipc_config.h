#ifndef IPC_CONFIG_H
#define IPC_CONFIG_H

// update path below:
// "/home/YOUR_USERNAME/RTOS-Embedded-Audio/runtime/audio_in.fifo"
constexpr const char* AUDIO_IN_FIFO_PATH =
    "/home/k_sxnchz/RTOS-Embedded-Audio/runtime/audio_in.fifo";

constexpr const int AUDIO_SAMPLE_RATE_HZ = 16000;
constexpr const int AUDIO_BYTES_PER_SAMPLE = 2;
constexpr const int AUDIO_CHANNELS = 1;
constexpr int AUDIO_FRAME_SAMPLES = 1024;

#endif // IPC_CONFIG_H
