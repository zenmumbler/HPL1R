/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */

#include "system/FileReader.h"

namespace hpl {

	FileReader::FileReader(const tString& filePath) {
		f = fopen(filePath.c_str(), "rb");
		if (f) {
			fseek(f, 0, SEEK_END);
			sizeBytes = static_cast<uint32_t>(ftell(f));
			fseek(f, 0, SEEK_SET);
		}
	}
	
	FileReader::~FileReader() {
		if (f) {
			fclose(f);
		}
	}
	
	bool FileReader::readBytes(void *into, size_t byteLength) {
		return fread(into, byteLength, 1, f) == 1;
	}
	
	void FileReader::skip(uint32_t bytesToSkip) {
		fseek(f, bytesToSkip, SEEK_CUR);
	}


	char* LoadEntireFile(const tString& asFileName, int& alLength)
	{
		FILE *pFile = fopen(asFileName.c_str(), "rb");
		if (pFile == NULL) {
			return nullptr;
		}

		fseek(pFile, 0, SEEK_END);
		alLength = static_cast<uint32_t>(ftell(pFile));
		fseek(pFile, 0, SEEK_SET);

		char *pBuffer = hplNewArray(char, alLength);
		fread(pBuffer, alLength, 1, pFile);

		fclose(pFile);
		return pBuffer;
	}

}
