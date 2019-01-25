#pragma once

class Impersonate
{
public:
	Impersonate();
	~Impersonate();

private:
	HANDLE _hToken = NULL;
};

class ImpersonateUser
{
public:
	ImpersonateUser(HANDLE hToken);
	~ImpersonateUser();

private:
	HANDLE _hToken = NULL;
};