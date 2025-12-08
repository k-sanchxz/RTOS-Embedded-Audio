# Real-Time Embedded Audio Anomaly Detection Device
QEMU + C++ + Python

- Python = sends WAV audio into the system (FIFO) in frames 
- FIFO = simulates UART linke between Linux and embedded target  
- QEMU ARM = simulated embedded processor  
- C++ firmware = runs the DSP pipeline and classification logic

## What the C++ Firmware Is Doing

The embedded firmware running inside QEMU is structued as a frame based processing pipeline:

- Accesses the FIFO and listens for incoming audio frames
- Processes audio in real time by converting raw int16 -> floating point for DSP
- Performs FFT for frequency analysis  
- Computes RMS and also variance 
- Classifies each frame as one of the follow:
  - VOICE  
  - DOG  
  - DOORBELL  
  - GLASS  
  - UNKNOWN  
- Then it prints classifications on the console  

This setup emulates an always on embedded audio detection device where every frame is processed

---

## What the Python Scripts Do

### send_audio.py

This script:

- Loads a WAV file  
- Converts it to raw PCM  
- Streams the audio to the embedded device using a FIFO  
- Paces the stream in real time  

It simulates a live audio coming into an embedded processor

---

## Project Structure 
```text
RTOS-EMBEDDED-AUDIO/
├── audios/ 
│ └── your_audio_files.wav
├── docs/
│ └── ipc_protocol.md
├── runtime/ ←--- this has be created locally ( read below for info)
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
├── main ← compiled ARM binary exec
├── .gitignore
├── LICENSE
└── README.md
```
---
## Required Local Setup (Important)

The runtime directory is not in the repo and has to be created locally

---
Create the Runtime FIFO Directory
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


