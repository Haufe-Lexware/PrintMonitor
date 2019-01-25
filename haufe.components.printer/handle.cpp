#include "stdafx.h"
#include "handle.h"
#include "log.h"

CPrinterHandle::CPrinterHandle(PWSTR szPrinterName, ACCESS_MASK dwDesiredAccess)
{
	PRINTER_DEFAULTS pd = { 0 };
	pd.pDatatype = NULL;
	pd.pDevMode = NULL;
	pd.DesiredAccess = dwDesiredAccess;

	if (!::OpenPrinter(szPrinterName, &_hHandle, &pd))
	{
		Log.Error(__FUNCTION__ " (OpenPrinter %d)", ::GetLastError());
		_hHandle = NULL;
	}
}

CPrinterHandle::CPrinterHandle(PWSTR szPrinterName, PPRINTER_DEFAULTS pDefaults)
{
	if (!::OpenPrinter(szPrinterName, &_hHandle, pDefaults))
	{
		Log.Error(__FUNCTION__ " (OpenPrinter (%d)", ::GetLastError());
		_hHandle = NULL;
	}
}

CPrinterHandle::~CPrinterHandle()
{
	if (_hHandle)
	{
		DWORD dwLastError = ::GetLastError();
		::ClosePrinter(_hHandle);
		::SetLastError(dwLastError);
	}
}
