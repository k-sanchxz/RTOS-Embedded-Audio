#include <pthread.h>
#include <atomic>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "audio_input.h"
#include "ipc_config.h"
#include "rtos_tasks.h"
#include "dsp_engine.h"
#include "classifier.h"

// global rtos state
static std::atomic<bool> rtos_running{false};
static pthread_t audio_receive_thread;
static pthread_t signal_process_thread;
static pthread_t classification_thread;
static pthread_t output_thread;

// task entry points
void* audio_receive_task(void*);
void* signal_process_task(void*);
void* classification_task(void*);
void* output_task(void*);

// start rtos tasks
bool start_rtos_tasks() {
    rtos_running = true;
    int err = 0;
    // create threads with default scheduling for now
    err |= pthread_create(&audio_receive_thread, nullptr, audio_receive_task, nullptr);
    err |= pthread_create(&signal_process_thread, nullptr, signal_process_task, nullptr);
    err |= pthread_create(&classification_thread, nullptr, classification_task, nullptr);
    err |= pthread_create(&output_thread, nullptr, output_task, nullptr);
    if (err) {
        std::cerr << "[RTOS] Error starting tasks." << std::endl;
        rtos_running = false;
        return false;
    }
    std::cout << "[RTOS] All tasks started." << std::endl;
    return true;
}

// request shutdown
void request_rtos_shutdown() {
    rtos_running = false;
}

// join all tasks
void join_rtos_tasks() {
    pthread_join(audio_receive_thread, nullptr);
    pthread_join(signal_process_thread, nullptr);
    pthread_join(classification_thread, nullptr);
    pthread_join(output_thread, nullptr);
    std::cout << "[RTOS] All tasks joined." << std::endl;
}

// stub task implementations
void* audio_receive_task(void*) {
    int frame_idx = 0;
    while (rtos_running) {
        std::vector<int16_t> frame;
        AudioReadStatus status = read_audio_frame(frame);
        if (status != AudioReadStatus::Success) {
            std::cout << "[TASK] AudioReceiveTask: end of stream or error." << std::endl;
            break;
        }
        std::cout << "[TASK] AudioReceiveTask alive, frame " << frame_idx++ << std::endl;
        // TODO: push frame to queue
    }
    return nullptr;
}

void* signal_process_task(void*) {
    int proc_idx = 0;
    dsp_engine::init_window(AUDIO_FRAME_SAMPLES);
    while (rtos_running) {
        std::vector<int16_t> frame;
        AudioReadStatus status = read_audio_frame(frame);
        if (status != AudioReadStatus::Success) {
            std::cout << "[TASK] SignalProcessTask: end of stream or error." << std::endl;
            break;
        }
        // DSP feature extraction
        AudioFeatureVector features = dsp_engine::extract_features(frame, AUDIO_SAMPLE_RATE_HZ);
        // Classification
        SoundLabel label = classify(features);
        // Print label
        const char* label_str = "UNKNOWN";
        switch (label) {
            case VOICE: label_str = "VOICE"; break;
            case DOG: label_str = "DOG"; break;
            case DOORBELL: label_str = "DOORBELL"; break;
            case ALERT: label_str = "ALERT"; break;
            default: label_str = "UNKNOWN"; break;
        }
        std::cout << "[CLASS] " << label_str << std::endl;
        usleep(10 * 1000); // 10 ms
    }
    return nullptr;
}

void* classification_task(void*) {
    int class_idx = 0;
    while (rtos_running) {
        // TODO: wait for features
        std::cout << "[TASK] ClassificationTask alive, label UNKNOWN, idx " << class_idx++ << std::endl;
        usleep(30 * 1000); // 30 ms
    }
    return nullptr;
}

void* output_task(void*) {
    int out_idx = 0;
    while (rtos_running) {
        // TODO: wait for classification result
        std::cout << "[OUTPUT] System alive, last classification=UNKNOWN, idx " << out_idx++ << std::endl;
        usleep(200 * 1000); // 200 ms
    }
    return nullptr;
}
