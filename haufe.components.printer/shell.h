#pragma once

class Shell
{
private:
	Shell();								// not createable

public:
	template <DWORD dwPath>
	static bool BrowseForFile(HANDLE hUserToken, WCHAR(&szPath)[dwPath])
	{
		return BrowseForFile(hUserToken, szPath, dwPath);
	}

	static void QueryOutputPath(PWSTR szGuid);

protected:
	enum : DWORD { Size = 0x400 };

	static bool BrowseForFile(HANDLE hUserToken, PWSTR szPath, DWORD dwPath);
	static bool BrowseForFile(PWSTR szPath, DWORD dwPath);
};
