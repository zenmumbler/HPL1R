/*
 * 2022 by zenmumbler
 * This file is part of Rehatched
 */

#ifndef HPL_SYSTEM_LOG_H
#define HPL_SYSTEM_LOG_H

namespace hpl {

	void SetLogFile(const char* filePath);
	void FatalError(const char* fmt,... );
	void Error(const char* fmt, ...);
	void Warning(const char* fmt, ...);
	void Log(const char* fmt, ...);

	void SetUpdateLogFile(const char* filePath);
	void ClearUpdateLogFile();
	void SetUpdateLogActive(bool abX);
	void LogUpdate(const char* fmt, ...);

}

#endif // HPL_SYSTEM_LOG_H
