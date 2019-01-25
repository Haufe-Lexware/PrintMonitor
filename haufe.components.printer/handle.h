#pragma once

class CPrinterHandle
{
public:
	CPrinterHandle(PWSTR szPrinterName, PPRINTER_DEFAULTS pDefaults = NULL);
	CPrinterHandle(PWSTR szPrinterName, ACCESS_MASK dwDesiredAccess);
	virtual ~CPrinterHandle();

	operator HANDLE() const { return _hHandle; }
	operator bool() const { return !!_hHandle; }

private:
	HANDLE _hHandle;
};