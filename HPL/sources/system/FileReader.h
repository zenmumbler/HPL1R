/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_FILE_READER_H
#define HPL_FILE_READER_H

#include "system/String.h"

namespace hpl {

	struct FileReader {
		FILE *f;
		uint32_t sizeBytes = 0;

		FileReader(const tString& filePath);
		~FileReader();
		
		bool valid() {
			return f != nullptr;
		}
		
		template <typename T>
		bool read(T *data, uint32_t byteLength = sizeof(T)) {
			return fread(data, byteLength, 1, f) == 1;
		}
		
		bool readBytes(void *into, size_t byteLength);
		
		void skip(uint32_t bytesToSkip);
	};

	char* LoadEntireFile(const tString& asFileName, int& alLength);
}

#endif // HPL_FILE_READER_H
