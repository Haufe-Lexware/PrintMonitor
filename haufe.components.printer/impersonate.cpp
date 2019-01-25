#include "stdafx.h"
#include "impersonate.h"
#include "log.h"

Impersonate::Impersonate()
{
	if (::OpenThreadToken(::GetCurrentThread(), TOKEN_IMPERSONATE, TRUE, &_hToken))
	{
		if (!::RevertToSelf())
		{
			Log.Error(__FUNCTION__ " (RevertToSelf %d)", ::GetLastError());
		}
	}
	else
	{
		Log.Error(__FUNCTION__ " (OpenThreadToken %d)", ::GetLastError());
	}
}

Impersonate::~Impersonate()
{
	if (_hToken)
	{
		if (!::SetThreadToken(NULL, _hToken))
		{
			Log.Error(__FUNCTION__ " (SetThreadToken %d)", ::GetLastError());
		}

		::CloseHandle(_hToken);
	}
}

ImpersonateUser::ImpersonateUser(HANDLE hToken) :
	_hToken(hToken)
{
	if (_hToken)
	{
		if (!::ImpersonateLoggedOnUser(_hToken))
		{
			Log.Error(__FUNCTION__ " (ImpersonateLoggedOnUser %d)", ::GetLastError());
		}
	}
}

ImpersonateUser::~ImpersonateUser()
{
	if (_hToken)
	{
		if (!::RevertToSelf())
		{
			Log.Error(__FUNCTION__ " (RevertToSelf %d)", ::GetLastError());
		}
	}
}
