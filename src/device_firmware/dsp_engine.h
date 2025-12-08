#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <chrono>

// Feature vector struct for classification
struct AudioFeatureVector {
    float dominant_freq_hz;
    float rms;
    float variance;
    bool burst;
    std::chrono::microseconds fft_time;
    std::chrono::microseconds feature_time;
    std::chrono::microseconds total_time;
    std::string error;
};

// DSP API
namespace dsp_engine {
    // Precompute window at startup
    void init_window(size_t frame_size);

    // Main feature extraction pipeline
    AudioFeatureVector extract_features(const std::vector<int16_t>& pcm_frame, int sample_rate);
}
