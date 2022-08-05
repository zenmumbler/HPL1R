/*
 * 2022 by zenmumbler
 * This file is part of Rehatched
 */

#include "system/Log.h"
#include <cstdio>
#include <string>

// define this to have all log, warning and error messages go to stdout/stderr as well
#define STD_LOGGING

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// LOG WRITER
	//////////////////////////////////////////////////////////////////////////

	class LogWriter
		{
		public:
			LogWriter(const char* filePath);
			~LogWriter();

			void Write(const char* message);
			void Clear();

			void SetFileName(const char* filePath);

		private:
			void ReopenFile();

			FILE *mpFile;
			std::string msFileName;
	};

	//-----------------------------------------------------------------------

	static LogWriter gLogWriter("hpl.log");
	static LogWriter gUpdateLogWriter("hpl_update.log");

	//-----------------------------------------------------------------------

	LogWriter::LogWriter(const char* filePath) : mpFile(nullptr), msFileName(filePath)
	{
	}

	LogWriter::~LogWriter()
	{
		if(mpFile) fclose(mpFile);
	}

	void LogWriter::Write(const char* message)
	{
		if(!mpFile) ReopenFile();

		if(mpFile)
		{
			fprintf(mpFile, "%s", message);
			fflush(mpFile);
		}
	}

	void LogWriter::Clear()
	{
		ReopenFile();
		if(mpFile) fflush(mpFile);
	}

	//-----------------------------------------------------------------------


	void LogWriter::SetFileName(const char* filePath)
	{
		if(msFileName == filePath) return;

		msFileName = filePath;
		ReopenFile();
	}

	//-----------------------------------------------------------------------


	void LogWriter::ReopenFile()
	{
		if(mpFile) fclose(mpFile);

		#ifdef WIN32
			mpFile = _fopen(msFileName.c_str(), "w");
		#else
			mpFile = fopen(msFileName.c_str(), "w");
		#endif
	}

	//-----------------------------------------------------------------------

	void SetLogFile(const char* filePath)
	{
		gLogWriter.SetFileName(filePath);
	}

	//-----------------------------------------------------------------------

	void FatalError(const char* fmt,... )
	{
		char text[2048];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
			vsnprintf(text, 2048, fmt, ap);
		va_end(ap);

#ifdef STD_LOGGING
		fprintf(stderr, "FATAL ERROR: %s", text);
#endif

		std::string sMess = "FATAL ERROR: ";
		sMess += text;
		gLogWriter.Write(sMess.c_str());

#ifdef WIN32
//		MessageBox( NULL, cString::To16Char(text).c_str(), _W("FATAL ERROR"), MB_ICONERROR);
#elif __MACOSX__
//		MacOSAlertBox(eMsgBoxType_Error, sMess, "");
#endif

		exit(1);
	}

	void Error(const char* fmt, ...)
	{
		char text[2048];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
			vsnprintf(text, 2048, fmt, ap);
		va_end(ap);

#ifdef STD_LOGGING
		fprintf(stderr, "ERROR: %s", text);
#endif

		std::string sMess = "ERROR: ";
		sMess += text;
		gLogWriter.Write(sMess.c_str());
	}

	void Warning(const char* fmt, ...)
	{
		char text[2048];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
			vsnprintf(text, 2048, fmt, ap);
		va_end(ap);

#ifdef STD_LOGGING
		fprintf(stdout, "WARNING: %s", text);
#endif

		std::string sMess = "WARNING: ";
		sMess += text;
		gLogWriter.Write(sMess.c_str());
	}

	void Log(const char* fmt, ...)
	{
		char text[2048];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		vsnprintf(text, 2048, fmt, ap);
		va_end(ap);

#ifdef STD_LOGGING
		fprintf(stdout, "%s", text);
#endif

		std::string sMess = "";
		sMess += text;
		gLogWriter.Write(sMess.c_str());
	}

	//-----------------------------------------------------------------------

	static bool gbUpdateLogIsActive = false;

	void SetUpdateLogFile(const char* filePath)
	{
		gUpdateLogWriter.SetFileName(filePath);
	}

	void ClearUpdateLogFile()
	{
		if(!gbUpdateLogIsActive) return;

		gUpdateLogWriter.Clear();
	}

	void SetUpdateLogActive(bool abX)
	{
		gbUpdateLogIsActive =abX;
	}

	void LogUpdate(const char* fmt, ...)
	{
		if (!gbUpdateLogIsActive) return;

		char text[2048];
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		vsnprintf(text, 2048, fmt, ap);
		va_end(ap);

		gUpdateLogWriter.Write(text);
	}

}
