# Real-Time Embedded Audio Anomaly Detection Device
QEMU + C++ + Python

A real-time embedded audio monitoring system that streams WAV audio from a Linux host into a simulated ARM device via virtual UART, performs FFT-based signal processing, and classifies sounds using RTOS-style task scheduling


- Python = controlled audio injection  
- FIFO = simulated UART interface  
- QEMU ARM = simulated embedded processor  
- C++ firmware = real-time DSP + classification

## What the C++ Firmware Is Doing

The embedded firmware running inside QEMU performs the following:

- Continuously listens for incoming PCM audio  
- Processes audio in real time  
- Performs FFT for frequency analysis  
- Extracts RMS, variance, and burst features  
- Classifies each frame as:
  - VOICE  
  - DOG  
  - DOORBELL  
  - GLASS  
  - UNKNOWN  
- Prints classifications directly to the console  

This simulates an **always-on embedded security listening device**.

---

## What the Python Scripts Do

### send_audio.py

This script:

- Loads a WAV file  
- Converts it to raw PCM  
- Streams the audio to the embedded device using a FIFO  
- Paces the stream in real time  

It simulates a **microphone feeding live audio into an embedded processor**.

---

## Project Structure 

RTOS-EMBEDDED-AUDIO/
├── audios/ ← must be created locally
│ └── your_audio_files.wav
├── docs/
│ └── ipc_protocol.md
├── runtime/ ← must be created locally
│ └── audio_in.fifo
├── scripts/
│ ├── send_audio.py
│ └── setup_ipc.sh
├── src/
│ ├── device_firmware/
│ │ ├── audio_input.cpp
│ │ ├── audio_input.h
│ │ ├── classifier.cpp
│ │ ├── classifier.h
│ │ ├── dsp_engine.cpp
│ │ ├── dsp_engine.h
│ │ ├── ipc_config.h
│ │ ├── rtos_tasks.cpp
│ │ └── rtos_tasks.h
│ └── main.cpp
├── main ← compiled ARM binary
├── .gitignore
├── LICENSE
└── README.md

---
## Required Local Setup (Important)

The `audios/` and `runtime/` directories are **not pushed to GitHub** and must be created locally.

---

### 1. Create the Audio Directory
mkdir audios
Place WAV files inside this folder
Install FFmpeg (Required for Audio Conversion)
  sudo apt update
  sudo apt install ffmpeg
Run this command in audios directory to convert to compatible format: 
  ffmpeg -i audios/<ORIGINAL_FILE>.wav -t 15 -ac 1 -ar 16000 -sample_fmt s16 audios/<MODIFIED_FILE>.wav


### 2. Create the Runtime FIFO Directory
This is a virtual UART used by the embedded device: 
  mkdir runtime
  mkfifo runtime/audio_in.fifo
  chmod 666 runtime/audio_in.fifo

Compile the firmware for ARM: 
  arm-linux-gnueabihf-g++ src/main.cpp src/device_firmware/*.cpp -o main

---

## Running the full system: 
Terminal 1: Boot the device: 
  qemu-arm -L /usr/arm-linux-gnueabihf ./main

Terminal 2: Inject audio using Python Script:
  python3 scripts/send_audio.py audios/<YOUR_AUDIO_FILE>.wav


