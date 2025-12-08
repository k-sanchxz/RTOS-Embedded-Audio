import sys
import wave
import time

FIFO_PATH = "/home/k_sxnchz/RTOS-Embedded-Audio/runtime/audio_in.fifo"
AUDIO_SAMPLE_RATE_HZ = 16000  # 16 kHz
AUDIO_BYTES_PER_SAMPLE = 2    # 16-bit samples
AUDIO_CHANNELS = 1            # mono
AUDIO_FRAME_SAMPLES = 1024    # frame size

def load_and_verify_wav(wav_path):
	# open wav verify format and read data
	try:
		with wave.open(wav_path, 'rb') as wf:
			channels = wf.getnchannels()
			sampwidth = wf.getsampwidth()
			framerate = wf.getframerate()
			nframes = wf.getnframes()

			# verify format before sending anything to the device
			if channels != AUDIO_CHANNELS:
				print(f"ERROR: expected mono, got {channels} channels.")
				sys.exit(1)
			if sampwidth != AUDIO_BYTES_PER_SAMPLE:
				print(f"ERROR: expected 16 bit, got {sampwidth} bytes/ sample.")
				sys.exit(1)
			if framerate != AUDIO_SAMPLE_RATE_HZ:
				print(f"ERROR: expected {AUDIO_SAMPLE_RATE_HZ} Hz, got {framerate} Hz.")
				sys.exit(1)

			pcm_bytes = wf.readframes(nframes)
			total_samples = nframes * channels

			print(f"WAV ok: {total_samples} samples, {len(pcm_bytes)} bytes.")
			return pcm_bytes, total_samples

	except wave.Error as e:
		print(f"ERROR: couldn't read wav: {e}")
		sys.exit(1)

	except FileNotFoundError:
		print(f"ERROR: file not found: {wav_path}")
		sys.exit(1)


def open_fifo_for_write():
	# block until the device side opens the fifo
	try:
		fifo = open(FIFO_PATH, 'wb')
		print(f"fifo ready: {FIFO_PATH}")
		return fifo

	except Exception as e:
		print(f"ERROR: fifo open failed: {e}")
		sys.exit(1)


def stream_pcm_to_fifo(pcm_bytes, fifo):
	# break audio into frames 
	frame_size_bytes = AUDIO_FRAME_SAMPLES * AUDIO_BYTES_PER_SAMPLE
	# only send full frames
	total_frames = len(pcm_bytes) // frame_size_bytes
	total_samples = total_frames * AUDIO_FRAME_SAMPLES
	# simulate realtime streaming
	sleep_time = AUDIO_FRAME_SAMPLES / AUDIO_SAMPLE_RATE_HZ

	print(f"streaming {total_frames} frames...")

	for i in range(total_frames):
		chunk = pcm_bytes[i*frame_size_bytes : (i+1)*frame_size_bytes]
		fifo.write(chunk)
		
		# experienced issues with python buffer
		# so flush after each write
		fifo.flush()
		# simulate real time 
		time.sleep(sleep_time)

	fifo.close()

	print("done streaming.")
	print(f"frames sent: {total_frames}")
	print(f"samples sent: {total_samples}")


if __name__ == "__main__":
	# arg check
	if len(sys.argv) != 2:
		print("usage: python send_audio.py <input.wav>")
		sys.exit(1)

	wav_path = sys.argv[1]
	pcm_bytes, total_samples = load_and_verify_wav(wav_path)
	
	# write to fifo from python side 
	fifo = open_fifo_for_write()
	stream_pcm_to_fifo(pcm_bytes, fifo)
