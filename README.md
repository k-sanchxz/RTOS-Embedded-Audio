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

The runtime directory and FIFO are not tracked by Git and they have to be added locally before running the system 

---
Create the Runtime FIFO Directory:
This is a virtual UART used by the embedded device: 
  mkdir runtime
  mkfifo runtime/audio_in.fifo
  chmod 666 runtime/audio_in.fifo
  
Update the FIFO Path in the firmware: 
```text
  open src/device_firmware/ipc_config.h
  update this line: 
    constexpr const char* AUDIO_IN_FIFO_PATH =
    "/home/YOUR_USERNAME/RTOS-Embedded-Audio/runtime/audio_in.fifo";
```
  If this path is incorrec then the firmware will fail to open the FIFO which will lead to an error since no audio will be        received. 
    
Compile the firmware for ARM: 
  arm-linux-gnueabihf-g++ src/main.cpp src/device_firmware/*.cpp -o main

---

## Running the full system: 
Terminal 1: Boot the device: 
  qemu-arm -L /usr/arm-linux-gnueabihf ./main

Terminal 2: Inject audio using Python Script:
  python3 scripts/send_audio.py audios/<YOUR_AUDIO_FILE>.wav


## Note:
The real time side of the system is fully working. The Python script continously send the audio into the FIFO, the C++ firmware running in QEMU reads and processes each frame and moves them throught the pipeline correctly. The part that still needs refinement is the signal processing code. Some FFT threshold and frequency boundaries in the classicfication logic need adjustmments before all sounds are detected consistely, but the core embedded project which is the real time audio reading and processing is functioning as intended. 
