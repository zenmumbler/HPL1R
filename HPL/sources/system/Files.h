/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */
#ifndef HPL_SYSTEM_FILES_H
#define HPL_SYSTEM_FILES_H

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
	bool RenameFile(const tWString& asFrom, const tWString& asTo);

	bool CreateFolder(const tWString& asPath);
	bool FolderExists(const tWString& asPath);

	bool IsFileLink(const tWString& asPath);
	bool LinkFile(const tWString& asPointsTo, const tWString& asLink);

	cDate FileModifiedDate(const tWString& asFilePath);
	cDate FileCreationDate(const tWString& asFilePath);

	//--------------------------------------------------------

	void FindFilesInDir(tWStringList &alstStrings, tWString asDir, tWString asMask);

}

#endif // HPL_SYSTEM_FILES_H
