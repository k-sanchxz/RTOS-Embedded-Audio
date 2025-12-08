
#include <iostream>
#include <unistd.h>
#include "device_firmware/audio_input.h"
#include "device_firmware/ipc_config.h"
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
            // cout << "[DEVICE] Received audio frame of " << frame.size() << " samples. First sample: " << frame[0] << endl;
            all_frames.push_back(frame);
        } else {
            // cout << "Read audio frame, but frame is empty." << endl;
        }
    }

    close_audio_input();
    
    // int i = 0; 
    // for (const auto& frame : all_frames) {
    //     cout << "Frame " << i++ << " first 5 samples: ";
    //     for (int j = 0; j < 5 && j < frame.size(); ++j) {
    //         cout << frame[j] << " ";
    //     }
    //     cout << endl;
    // }

    cout << "[DEVICE] Audio input FIFO closed." << endl;
    cout << "[DEVICE] Total frames received: " << all_frames.size() << endl;
    // Proceed with next logic (e.g., processing/classification)
    // ...
    return 0;
}
