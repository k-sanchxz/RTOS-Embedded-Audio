
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <cstdint>
#include "ipc_config.h"
#include "audio_input.h"

static int audio_fifo_fd = -1;

bool open_audio_input() {
    audio_fifo_fd = open(AUDIO_IN_FIFO_PATH, O_RDONLY);
    if (audio_fifo_fd < 0) {
        std::cerr << "[DEVICE] Failed to open audio input FIFO at " << AUDIO_IN_FIFO_PATH << std::endl;
        return false;
    }
    std::cout << "[DEVICE] Audio input FIFO opened at " << AUDIO_IN_FIFO_PATH << std::endl;
    return true;
}

AudioReadStatus read_audio_frame(std::vector<int16_t>& frame) {
    frame.resize(AUDIO_FRAME_SAMPLES);
    const size_t total_bytes = AUDIO_FRAME_SAMPLES * AUDIO_BYTES_PER_SAMPLE;
    // std::cout<<"JUST TEST: "<<frame.data()<<std::endl; 
    uint8_t* buffer = reinterpret_cast<uint8_t*>(frame.data());
    size_t bytes_read = 0;
    while (bytes_read < total_bytes) {
        // writing to frame.data()
        ssize_t n = read(audio_fifo_fd, buffer + bytes_read, total_bytes - bytes_read);
        if (n == 0) {
            // EOF
            return AudioReadStatus::EndOfFile;
        } else if (n < 0) {
            std::cerr << "[DEVICE] Error reading audio frame from FIFO" << std::endl;
            return AudioReadStatus::Error;
        }
        bytes_read += n;
    }
    return AudioReadStatus::Success;
}

void close_audio_input() {
    if (audio_fifo_fd >= 0) {
        close(audio_fifo_fd);
        audio_fifo_fd = -1;
        std::cout << "[DEVICE] Audio input FIFO closed." << std::endl;
    }
}
