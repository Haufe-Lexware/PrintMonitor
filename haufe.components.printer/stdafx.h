//	PrintMonitor for the Ghostscript Printer Driver
//	Copyright (C) 2019  Haufe-Lexware
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU Affero General Public License as
//	published by the Free Software Foundation, either version 3 of the
//	License, or (at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU Affero General Public License for more details.
//
//	You should have received a copy of the GNU Affero General Public License
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once
#include "targetver.h"

#define STRICT 1
#define ISOLATION_AWARE_ENABLED 1
#define WIN32_LEAN_AND_MEAN						// Exclude rarely-used stuff from Windows headers
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS		// some CString constructors will be explicit
#define _ATL_NO_PERF_SUPPORT
#define _ATL_NO_COM_SUPPORT

#include <atlbase.h>
#include <atlwin.h>
#include <atlsecurity.h>
#include <map>
#include <mutex>
#include <winspool.h>
#include <winsplp.h>
#include "..\deploy\PrintMonitor.h"

#include <shlobj.h>
#pragma comment(lib, "shell32.lib")
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")
#include <wtsapi32.h>
#pragma comment(lib, "wtsapi32.lib")

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " \
	"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma warning(push)
#pragma warning(disable: 4471)
#import "uiautomationcore.dll" exclude("tagRECT", "tagPOINT") rename("FindText", "_FindText")
#pragma warning(pop)

#ifdef _DEBUG
	#define RPTN(rptno, msg, ...) \
		_RPT_BASE(_CRT_##rptno, __FILE__ "\nFunction: " __FUNCTION__, __LINE__, NULL, msg "\n", __VA_ARGS__)
#else
	#define RPTN(rptno, msg, ...)
#endif