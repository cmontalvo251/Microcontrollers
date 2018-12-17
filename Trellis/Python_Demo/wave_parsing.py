import struct
import array

def parse_wav(filename, *, scale = 1.0):
    print("Reading file " + filename)
    f = open(filename, "rb")
    chunk_id = f.read(4)
    f.read(4) # skip chunksize
    format = f.read(4)
    subchunk1_id = f.read(4)
    if chunk_id != b'RIFF' or format != b'WAVE' or subchunk1_id != b'fmt ':
        raise RuntimeError("Invalid WAVE")
    subchunk1_size = struct.unpack("<I", f.read(4))[0]
    if subchunk1_size != 16:
        raise RuntimeError("Only PCM supported")
    audio_format, num_channels, sample_rate = struct.unpack("<HHI", f.read(8))
    if audio_format != 1:
        raise RuntimeError("Only uncompressed WAVs")
    f.read(6) # skip byterate and blockalign
    bits_per_sample = struct.unpack("<H", f.read(2))[0]
    subchunk2_id =  f.read(4)
    if subchunk2_id != b'data':
        raise RuntimeError("Invalid WAVE")
    data_size = struct.unpack("<I", f.read(4))[0]
    if bits_per_sample != 16:
        raise RuntimeError("Only 16 bit samples")
    num_samples = data_size * 8 // bits_per_sample
    f.close()
    return {'sample_rate': sample_rate,
            'channels': num_channels,
            'num_samples': num_samples,
            'data_size': data_size}
