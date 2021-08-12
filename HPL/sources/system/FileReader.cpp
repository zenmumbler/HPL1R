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
}