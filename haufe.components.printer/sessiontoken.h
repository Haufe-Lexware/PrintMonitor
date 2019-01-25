#pragma once

class SessionToken
{
private:
	SessionToken();
	bool EnablePrivilege();

public:
	static bool GetUserToken(DWORD dwProcessID, HANDLE &hPrimaryToken);
	static bool GetUserToken(HANDLE &hUserToken);
};
