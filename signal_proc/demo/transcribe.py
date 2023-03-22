import pvleopard as pv
import sounddevice as sd
import pdb

T_recording = 5
F_sample = 16000

def get_audio_data():
  print("RECORDING...")
  waveform = sd.rec(T_recording*F_sample, samplerate=F_sample, channels=1, dtype='int64')
  sd.wait()
  print("DONE")
  return waveform

leopard = pv.create(access_key="4aaEbdDymSW3u6gRGfDwdsH6WYmaa7GcONy0AfQibvpJTMyfRlVV4A==")

transcript, words = leopard.process_file("harvard.wav")
print(transcript)
for word in words:
  print("{word=\"%s\" start_sec=%.2f end_sec=%.2f confidence=%.2f}" % (word.word, word.start_sec, word.end_sec, word.confidence))


