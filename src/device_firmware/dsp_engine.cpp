#include "dsp_engine.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iostream>

// shared hann window, kept simple and global for now
namespace {
    std::vector<float> window;
    bool window_ready = false;
}

// build hann window once at startup
void dsp_engine::init_window(size_t frame_size) {
    window.resize(frame_size);

    for (size_t i = 0; i < frame_size; ++i) {
        // standard hann window formula
        window[i] = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (frame_size - 1)));
    }

    window_ready = true;
    std::cout << "[DSP] window initialized (size: " << frame_size << ")\n";
}

// convert raw pcm samples into normalized float range [-1, 1]
static void pcm_to_float(const std::vector<int16_t>& pcm,
                         std::vector<float>& out)
{
    out.resize(pcm.size());

    for (size_t i = 0; i < pcm.size(); ++i) {
        out[i] = static_cast<float>(pcm[i]) / 32768.0f;
    }
}

// apply the precomputed window to the samples
static void apply_window(std::vector<float>& samples) {
    if (!window_ready || samples.size() != window.size()) {
        std::cout << "[DSP] window not ready or size mismatch\n";
        return;
    }

    for (size_t i = 0; i < samples.size(); ++i) {
        samples[i] *= window[i];
    }
}

// simple brute-force fft magnitude (slow but deterministic)
static void simple_fft_magnitude(const std::vector<float>& in,
                                 std::vector<float>& mag)
{
    size_t N = in.size();
    mag.assign(N / 2, 0.0f);

    for (size_t k = 0; k < N / 2; ++k) {
        float re = 0.0f;
        float im = 0.0f;

        for (size_t n = 0; n < N; ++n) {
            float angle = 2.0f * M_PI * k * n / N;
            re += in[n] * std::cos(angle);
            im -= in[n] * std::sin(angle);
        }

        mag[k] = std::sqrt(re * re + im * im);
    }
}

// root-mean-square energy of a frame
static float compute_rms(const std::vector<float>& samples) {
    if (samples.empty()) return 0.0f;

    float sum_sq = 0.0f;
    for (float s : samples)
        sum_sq += s * s;

    return std::sqrt(sum_sq / samples.size());
}

// variance of the frame (how spread out the signal is)
static float compute_variance(const std::vector<float>& samples) {
    if (samples.empty()) return 0.0f;

    float mean =
        std::accumulate(samples.begin(), samples.end(), 0.0f) / samples.size();

    float var = 0.0f;
    for (float s : samples)
        var += (s - mean) * (s - mean);

    return var / samples.size();
}

// burst detector based on rms change inside the frame
static bool detect_burst(const std::vector<float>& samples) {
    if (samples.size() < 2)
        return false;

    size_t mid = samples.size() / 2;

    float rms_first =
        compute_rms(std::vector<float>(samples.begin(),
                                       samples.begin() + mid));

    float rms_second =
        compute_rms(std::vector<float>(samples.begin() + mid,
                                       samples.end()));

    // threshold is tuned later, this is just a starter value
    return std::abs(rms_second - rms_first) > 0.2f;
}

// full dsp pipeline: raw pcm -> frequency + energy features
AudioFeatureVector dsp_engine::extract_features(
    const std::vector<int16_t>& pcm_frame,
    int sample_rate)
{
    AudioFeatureVector features{};
    auto t0 = std::chrono::high_resolution_clock::now();

    // basic safety checks
    if (!window_ready || pcm_frame.empty()) {
        features.error = "window not ready or empty frame";
        std::cout << "[DSP] error: " << features.error << "\n";
        return features;
    }

    // step 1: convert pcm to float
    std::vector<float> samples;
    pcm_to_float(pcm_frame, samples);

    // step 2: apply window before fft
    apply_window(samples);
    auto t1 = std::chrono::high_resolution_clock::now();

    // step 3: frequency domain
    std::vector<float> mag;
    simple_fft_magnitude(samples, mag);
    auto t2 = std::chrono::high_resolution_clock::now();

    // step 4: dominant frequency
    auto max_it = std::max_element(mag.begin(), mag.end());
    size_t dom_bin = std::distance(mag.begin(), max_it);

    features.dominant_freq_hz =
        static_cast<float>(dom_bin) * sample_rate / samples.size();

    // step 5: time-domain energy stats
    features.rms      = compute_rms(samples);
    features.variance = compute_variance(samples);

    // step 6: transient / burst detection
    features.burst = detect_burst(samples);

    // timing breakdown for profiling
    features.fft_time =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);

    features.feature_time =
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - t2);

    features.total_time =
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - t0);

    // numeric sanity check
    if (std::isnan(features.dominant_freq_hz) ||
        std::isnan(features.rms) ||
        std::isnan(features.variance))
    {
        features.error = "nan detected in features";
        std::cout << "[DSP] error: " << features.error << "\n";
    }

    return features;
}
