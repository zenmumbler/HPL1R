/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */

#ifndef HPL_SYSTEM_FILES_H
#define HPL_SYSTEM_FILES_H

#include "system/String.h"
#include "system/Date.h"

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

	//--------------------------------------------------------

	char* LoadEntireFile(const tString& asFileName, int& alLength);

	//--------------------------------------------------------

	enum eSystemPath
	{
		eSystemPath_Personal,
	};

	tWString GetSystemSpecialPath(eSystemPath aPathType);

	//--------------------------------------------------------

	bool FileExists(const tWString& asFileName);
	void RemoveFile(const tWString& asFileName);
	bool CloneFile(const tWString& asSrcFileName, const tWString& asDestFileName, bool abFailIfExists);

	bool CreateFolder(const tWString& asPath);
	bool FolderExists(const tWString& asPath);

	cDate FileModifiedDate(const tWString& asFilePath);
	cDate FileCreationDate(const tWString& asFilePath);

	//--------------------------------------------------------

	void FindFilesInDir(tWStringVec &fileNames, tWString asDir, tWString asMask);

}

#endif // HPL_SYSTEM_FILES_H
