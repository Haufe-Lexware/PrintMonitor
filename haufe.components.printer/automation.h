#pragma once

class Automation
{
private:
	Automation();							// not createable

public:
	static void RegEdit(PCWSTR szPath);

protected:
	static void RegEdit(HWND hWndRegEdit, PCWSTR szPath);
};
