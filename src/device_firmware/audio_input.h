
#pragma once
#include <vector>
#include <cstdint>
#include "ipc_config.h"

// Opens the audio input FIFO for reading
bool open_audio_input();

// Status for reading audio frames
enum class AudioReadStatus { Success, EndOfFile, Error };

// Reads a single audio frame into the provided vector
AudioReadStatus read_audio_frame(std::vector<int16_t>& frame);

// Closes the audio input FIFO
void close_audio_input();