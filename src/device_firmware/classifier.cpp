#include "classifier.h"
#include <cmath>
#include <string>

// Simple rule-based classifier
SoundLabel classify(const AudioFeatureVector& features) {
    if (!features.error.empty()) {
        return UNKNOWN;
    }
    // DOORBELL: high freq, burst, high variance
    if (features.dominant_freq_hz > 1800 && features.burst && features.variance > 0.02f) {
        return DOORBELL;
    }
    // DOG: mid freq, burst or high variance, moderate RMS
    if (features.dominant_freq_hz > 200 && features.dominant_freq_hz <= 1500 && (features.burst || features.variance > 0.015f) && features.rms > 0.15f) {
        return DOG;
    }
    // VOICE: low-mid freq, not burst, low variance, continuous RMS
    if (features.dominant_freq_hz > 80 && features.dominant_freq_hz <= 400 && !features.burst && features.variance < 0.01f && features.rms > 0.1f) {
        return VOICE;
    }
    // ALERT: abnormal, loud, or high variance (not matching above)
    if (features.rms > 0.4f || features.variance > 0.04f) {
        return ALERT;
    }
    return UNKNOWN;
}
