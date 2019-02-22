###This will test any audio you like using parse_wav.py

import os
import sys
import struct

def parse_wav(filename,scale = 1.0):
	print("Reading file " + filename)
	f = open(filename, "rb")
	chunk_id = f.read(4)
	print("chunk_id = ",chunk_id)
	f.read(4) # skip chunksize
	format = f.read(4)
	print("format = ",format)
	subchunk1_id = f.read(4)
	print("subchunk1_id = ",subchunk1_id)
	if chunk_id != b'RIFF' or format != b'WAVE' or subchunk1_id != b'fmt ':
		raise RuntimeError("Invalid WAVE - Check CHUNK,Format and Subchunk")
	subchunk1_size = struct.unpack("<I", f.read(4))[0]
	print("subchunk1 size = ",subchunk1_size)
	if subchunk1_size != 16:
		raise RuntimeError("Only PCM supported")
	audio_format, num_channels, sample_rate = struct.unpack("<HHI", f.read(8))
	print("audio_format = ",audio_format)
	print("num_channels = ",num_channels)
	print("sample_rate = ",sample_rate)
	if audio_format != 1:
		raise RuntimeError("Only uncompressed WAVs")
	f.read(6) # skip byterate and blockalign
	bits_per_sample = struct.unpack("<H", f.read(2))[0]
	subchunk2_id =  f.read(4)
	print("bits_per_sample = ",bits_per_sample)
	print("subchunk2_id = ",subchunk2_id)
	if subchunk2_id != b'data':
		raise RuntimeError("Invalid WAVE - Check the Subchunk 2")
	data_size = struct.unpack("<I", f.read(4))[0]
	if bits_per_sample != 16:
		raise RuntimeError("Only 16 bit samples")
	num_samples = data_size * 8 // bits_per_sample
	f.close()
	return {'sample_rate': sample_rate,
			'channels': num_channels,
			'num_samples': num_samples,
			'data_size': data_size}

###Remember the type is the row
TYPES = ["Hey","Square","Deeper","Days"]
##And the column is dictated by Instrument
INSTRUMENTS = ["Drums","Tops","Bass","Chords","Leads","Voice"]

wave_format = parse_wav("voice01.wav")
print(wave_format)
wave_format = parse_wav('Future_Beat/FFMPEG/cleaned/' + INSTRUMENTS[0] + ' ' + TYPES[0] + '.wav')
print(wave_format)
wave_format = parse_wav('Future_Beat/FFMPEG/cleaned/Split/000/cleaned/Drums Hey000.wav')
print(wave_format)