#include <iostream>
#include <unistd.h>
#include "device_firmware/audio_input.h"
#include "device_firmware/ipc_config.h"
#include "device_firmware/dsp_engine.h"
#include "device_firmware/classifier.h"
using std::cout, std::cin, std::endl;

int main() {
    cout << "[DEVICE] Boot sequence complete." << endl;
    cout << "[DEVICE] Ready. Waiting for audio stream..." << endl;

    if (!open_audio_input()) {
        cout << "Error to open audio input in main" << endl;
        return -1;
    }
    cout << "Audio input opened successfully in main" << endl;

    std::vector<std::vector<int16_t>> all_frames;
    while (true) {
        std::vector<int16_t> frame;
        AudioReadStatus read_status = read_audio_frame(frame);
        if (read_status == AudioReadStatus::EndOfFile) {
            cout << "[DEVICE] End of audio stream (EOF). Storing data and proceeding..." << endl;
            break;
        } else if (read_status == AudioReadStatus::Error) {
            cout << "[DEVICE] Error reading audio frame. Exiting..." << endl;
            break;
        }
        if (!frame.empty()) {
            all_frames.push_back(frame);
        }
    }

    close_audio_input();
    cout << "[DEVICE] Audio input FIFO closed." << endl;
    cout << "[DEVICE] Total frames received: " << all_frames.size() << endl;

    // Process all frames and classify
    int voice_count = 0, dog_count = 0, doorbell_count = 0, alert_count = 0, unknown_count = 0;
    dsp_engine::init_window(AUDIO_FRAME_SAMPLES);
    for (const auto& frame : all_frames) {
        AudioFeatureVector features = dsp_engine::extract_features(frame, AUDIO_SAMPLE_RATE_HZ);
        SoundLabel label = classify(features);
        switch (label) {
            case VOICE: voice_count++; break;
            case DOG: dog_count++; break;
            case DOORBELL: doorbell_count++; break;
            case ALERT: alert_count++; break;
            default: unknown_count++; break;
        }
    }
    cout << "[RESULTS] VOICE: " << voice_count << " DOG: " << dog_count << " DOORBELL: " << doorbell_count << " ALERT: " << alert_count << " UNKNOWN: " << unknown_count << endl;
    return 0;
}
