// This file will only define:
// FIFO path string
// Audio sample format parameters
// Frame size (samples per read)

constexpr const char* AUDIO_IN_FIFO_PATH =  "/home/k_sxnchz/RTOS_Audio_Project/runtime/audio_in.fifo"; 

// audio sample format parameter (f_hz, y(amplitude), x(time))
constexpr const int AUDIO_SAMPLE_RATE_HZ = 160000; 
constexpr const int AUDIO_BYTES_PER_SAMPLE = 2; 
constexpr const int AUDIO_CHANNELS = 1; 

// frame size 
constexpr int AUDIO_FRAME_SAMPLES = 1024; 