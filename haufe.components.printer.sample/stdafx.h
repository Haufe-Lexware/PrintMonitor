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
