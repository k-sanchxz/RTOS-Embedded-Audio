Channel path:
/home/k_sxnchz/RTOS_Audio_Project/runtime/audio_in.fifo

Direction:
Python → FIFO → C++ (device receives only; no TX back yet).

Format:

16-bit signed little-endian PCM

Mono

Sample rate 16 kHz

Framing:

Python sends raw PCM in blocks of AUDIO_FRAME_SAMPLES samples.

No headers. The device just reads AUDIO_FRAME_SAMPLES * AUDIO_BYTES_PER_SAMPLE bytes per frame.

End of stream:

When Python closes the FIFO, C++ sees EOF and treats that as “no more audio”.