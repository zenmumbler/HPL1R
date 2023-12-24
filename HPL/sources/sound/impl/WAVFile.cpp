/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */

#include "sound/impl/WAVFile.h"
#include "system/Files.h"
#include "system/Log.h"

#include <cstdio>
#include <AL/al.h>

struct WAVHeader {
	uint32_t wavID; // "RIFF"
	uint32_t dataSize; // full file size - 8
	uint32_t wavSig; // "WAVE"
	
	uint32_t formatID; // "fmt "
	uint32_t formatSize; // must be 16
	uint16_t encoding; // 1 is PCM, others unsupported
	uint16_t channels;
	uint16_t rate; // sample rate in Hz
	uint32_t bytesPerSecond; // rate * channels * sampleBits/8
	uint16_t bytesPerSample; // channels * sampleBits/8
	uint16_t sampleBits; // 8 or 16
};

struct ChunkHeader {
	uint32_t chunkID;
	uint32_t chunkSize;
};

namespace hpl {

	uint32_t LoadWAVFile(const tString& filePath, int *channels, ALint *format, int *rate, short **samples) {
		FileReader f {filePath};
		if (f.valid() == false) return 0;
		if (f.sizeBytes < sizeof(WAVHeader) + sizeof(ChunkHeader)) return 0;

		WAVHeader header {};
		if (f.read(&header) == false) return 0;

		// check header
		if (header.wavID != 'FFIR') return 0;
		if (header.dataSize > f.sizeBytes - 8) return 0;
		if (header.dataSize < sizeof(WAVHeader) - 8 + sizeof(ChunkHeader)) return 0;
		if (header.wavSig != 'EVAW') return 0;
		if (header.formatID != ' tmf') return 0;
		if (header.formatSize != 16) return 0;
		if (header.encoding != 1) return 0;
		if (header.channels == 0 || header.channels > 2) return 0;
		if (header.rate < 1 || header.rate > 48000) return 0;
		if (header.sampleBits != 8 && header.sampleBits != 16) return 0;
		uint32_t sampleBytesCheck = header.sampleBits >> 3;
		if (header.bytesPerSecond != header.rate * header.channels * sampleBytesCheck) return 0;
		if (header.bytesPerSample != header.channels * sampleBytesCheck) return 0;

		// bail on 8-bit sample data
		if (header.bytesPerSample == 1) {
			Error("*** 8-bit WAVs not supported: %s\n", filePath.c_str());
			return 0;
		}

		// warn when detecting very low-quality sounds
		if (header.rate < 22000) {
			Warning("Low quality WAV file (%d Hz): %s\n", header.rate, filePath.c_str());
		}

		// scan chunks for the "data" chunk (usually the first)
		ChunkHeader chunkHeader {};
		do {
			if (f.read(&chunkHeader) == false) return 0;
			if (chunkHeader.chunkID != 'atad') {
				f.skip(chunkHeader.chunkSize);
			}
		} while (chunkHeader.chunkID != 'atad');

		if (chunkHeader.chunkSize < header.bytesPerSample) return 0;
		if (chunkHeader.chunkSize > f.sizeBytes - sizeof(WAVHeader) - sizeof(ChunkHeader)) return 0;

		// read sample data from file
		short *sampleData = static_cast<short *>(malloc(chunkHeader.chunkSize));
		if (f.read(sampleData, chunkHeader.chunkSize) == false) {
			Error("*** Could not read WAV sample data?\n");
			free(sampleData);
			return 0;
		}

		// set output values
		*channels = header.channels;
		*format = header.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		*rate = header.rate;
		*samples = sampleData;

		return chunkHeader.chunkSize / header.bytesPerSample;
	}

}
