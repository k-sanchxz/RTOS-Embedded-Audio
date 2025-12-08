#pragma once
#include "dsp_engine.h"

// Classification output
enum SoundLabel {
    UNKNOWN,
    VOICE,
    DOG,
    DOORBELL,
    ALERT
};

// Classifier API
SoundLabel classify(const AudioFeatureVector& features);
