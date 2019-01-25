// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently
#pragma once
#define VC_EXTRALEAN			// Exclude rarely-used stuff from Windows headers

#include "targetver.h"
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#define _AFX_ALL_WARNINGS		// turns off MFC's hiding of some common and often safely ignored warning messages
#include <afxwin.h>				// MFC core and standard components
#include "afxdialogex.h"
#include <afxrich.h>			// MFC rich edit classes
//nclude <atlpath.h>
#include <winspool.h>

#include "..\haufe.components.printer\log.h"													// INTERNALLY, DON'T USE THIS IN YOUR APPLICATION!
#include "..\..\..\..\build\Inc\Components\PrintMonitor\PrintMonitor.h"							// OK

#if 0																							// AMYUNI TEST, DON'T USE THIS IN YOUR APPLICATION!
#include "..\..\..\..\ThirdParty\Amyuni_PDF_Converter\Version_5.5\pdfdrv550\PDF Converter\SDK\cdintf.h"
  #ifdef _WIN64
	#pragma comment(lib, "..\\..\\..\\..\\ThirdParty\\Amyuni_PDF_Converter\\Version_5.5\\pdfdrv550\\PDF Converter\\SDK\\x64\\cdintf.lib")
  #else
	#pragma comment(lib, "..\\..\\..\\..\\ThirdParty\\Amyuni_PDF_Converter\\Version_5.5\\pdfdrv550\\PDF Converter\\SDK\\Win32\\cdintf.lib")
  #endif
#endif

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
