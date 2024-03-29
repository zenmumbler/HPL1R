/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _UNICODE

#ifdef WIN32
	#pragma comment(lib, "angelscript.lib")
	#define UNICODE
	#define WIN32_MEAN_AND_LEAN
	#include <windows.h>
	#include <shlobj.h>
#else
	#include <clocale>
	#include <langinfo.h>
	#include <unistd.h>
#endif

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <sys/stat.h>
#include <time.h>

#include "system/System.h"
#include "system/String.h"


namespace hpl {
	// macOS version defined in LowLevelSystemMac.mm
	tString GetDataDir();

#ifdef __linux__
	tString GetDataDir()
	{
		tString temp;
		BrInitError error;
		if (!br_init (&error)) {
			// Log non-fatal error
			printf("*** BinReloc failed to initialize. Error: %d\n", error);
		} else {
			char *exedir;
			exedir = br_find_exe_dir(NULL);
			if (exedir) {
				temp = exedir;
				free(exedir);
			}
		}
		return temp;
	}
#endif
} // ns hpl

extern int hplMain(const hpl::tString &asCommandLine);

#ifdef WIN32
int WINAPI WinMain(	HINSTANCE hInstance,  HINSTANCE hPrevInstance,LPSTR	lpCmdLine, int nCmdShow)
{
	return hplMain(lpCmdLine);
}
#else
int main(int argc, char *argv[])
{
#ifdef __linux__
	if(!std::setlocale(LC_CTYPE, "")) {
		fprintf(stderr, "Can't set the specified locale! Check LANG, LC_CTYPE, LC_ALL.\n");
		return 1;
	}
	char *charset = nl_langinfo(CODESET);
	bool utf8_mode = (strcasecmp(charset, "UTF-8") == 0);
	if (!utf8_mode) {
		fprintf(stderr, "UTF-8 Charset %s available.\nCurrent LANG is %s\nCharset: %s\n",
			utf8_mode ? "is" : "not",
			getenv("LANG"), charset);
	}
#endif

	bool cwd = false;
	hpl::tString cmdline = "";
	for (int i=1; i < argc; i++) {
		if (strcmp(argv[i], "-cwd") == 0) {
			cwd = true;
		} else if (strncmp(argv[i], "-psn", 4) == 0) {
			// skip "finder" process number
		} else {
			if (cmdline.length()>0) {
				cmdline.append(" ").append(argv[i]);
			} else {
				cmdline.append(argv[i]);
			}
		}
	}

	if (!cwd) {
		hpl::tString dataDir = hpl::GetDataDir();

		chdir(dataDir.c_str());
	}

	return hplMain(cmdline);
}
#endif


namespace hpl {

	//-----------------------------------------------------------------------

	void CopyTextToClipboard(const tWString &asText)
	{
#ifdef WIN32
		OpenClipboard(NULL);
		EmptyClipboard();

		HGLOBAL clipbuffer;
		wchar_t * pBuffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, (asText.size()+1) *sizeof(wchar_t));
		pBuffer = (wchar_t*)GlobalLock(clipbuffer);
		wcscpy(pBuffer, asText.c_str());
		//GlobalUnlock(clipbuffer);

		SetClipboardData(CF_UNICODETEXT, clipbuffer);

		GlobalUnlock(clipbuffer);

		CloseClipboard();
#else
	#pragma unused(asText)
	// TODO: tbi
#endif
	}

	tWString LoadTextFromClipboard()
	{
#ifdef WIN32
		tWString sText=_W("");
		OpenClipboard(NULL);

		HGLOBAL clipbuffer = GetClipboardData(CF_UNICODETEXT);

		wchar_t *pBuffer = (wchar_t*)GlobalLock(clipbuffer);

		if(pBuffer != NULL) sText = pBuffer;

		GlobalUnlock(clipbuffer);

		CloseClipboard();

		return sText;
#else
		// TODO: tbi
		return _W("");
#endif
	}

	//-----------------------------------------------------------------------

	void CreateMessageBoxW (eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, va_list ap)
	{
		wchar_t text[2048];

		if (fmt == NULL)
			return;
		vswprintf(text, 2047, fmt, ap);

		tWString sMess = _W("");

		#ifdef WIN32
		sMess += text;

		UINT lType = MB_OK;

		switch (eType)
		{
		case eMsgBoxType_Info:
			lType += MB_ICONINFORMATION;
			break;
		case eMsgBoxType_Error:
			lType += MB_ICONERROR;
			break;
		case eMsgBoxType_Warning:
			lType += MB_ICONWARNING;
			break;
		default:
			break;
		}

		MessageBox( NULL, sMess.c_str(), asCaption, lType );
		#else
		sMess += asCaption;
		sMess +=_W("\n\n");
		sMess += text;
		// TODO: show system alert
		#pragma unused(eType)

		#endif
	}

	void CreateMessageBoxW ( eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, ...)
	{
		va_list ap;

		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		CreateMessageBoxW (eType, asCaption, fmt, ap);
		va_end(ap);
	}

	void CreateMessageBoxW ( const wchar_t* asCaption, const wchar_t *fmt, ...)
	{
		va_list ap;
		if (fmt == NULL)
			return;
		va_start(ap, fmt);
		CreateMessageBoxW( eMsgBoxType_Default, asCaption, fmt, ap );
		va_end(ap);
	}

	//-----------------------------------------------------------------------

	void OpenBrowserWindow ( const tWString& asURL )
	{
		#ifdef WIN32
		ShellExecute ( NULL, _W("open"), asURL.c_str(), NULL, NULL, SW_SHOWNORMAL );
		#elif defined(__linux__)
		tString asTemp = "./openurl.sh "+cString::To8Char(asURL);
		system(asTemp.c_str());
		#elif defined(__APPLE__)
		tString asTemp = "open "+cString::To8Char(asURL);
		system(asTemp.c_str());
		#endif
	}

	//-----------------------------------------------------------------------

	static uint64_t baseTimeMS = 0;

	void InitAppTime()
	{
		baseTimeMS = 0UL; // GetAppTimeMS uses baseTimeNS itself
		baseTimeMS = GetAppTimeMS();
	}

	//-----------------------------------------------------------------------

	uint64_t GetAppTimeMS()
	{
		// TODO: use regular clock_gettime for other unixen and QPC stuff for Win
		return (clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW) / 1000000UL) - baseTimeMS;
	}

	//-----------------------------------------------------------------------

	float GetAppTimeFloat() {
		auto timeMS = GetAppTimeMS();
		return static_cast<float>(timeMS) / 1000.f;
	}

	//-----------------------------------------------------------------------

}
