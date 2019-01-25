#include "stdafx.h"
#include "automation.h"

// static
void Automation::RegEdit(PCWSTR szPath)
{
	HWND hWndRegEdit = ::FindWindow(L"RegEdit_RegEdit", NULL);
	if (!hWndRegEdit)
	{
		ATL::CRegKey key;
		if (ERROR_SUCCESS == key.Open(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Regedit", KEY_READ | KEY_WRITE))
		{
			key.SetStringValue(L"LastKey", ATL::CA2W(""));																										// szPath
		}

		SHELLEXECUTEINFO se = { 0 };
		se.cbSize = sizeof(se);
		se.fMask = SEE_MASK_DEFAULT;
		se.lpVerb = L"runas";
		se.lpFile = L"regedit.exe";
		::ShellExecuteEx(&se);
	}

	int i = 30;
	while (!hWndRegEdit && i--)
	{
		::Sleep(100);
		hWndRegEdit = ::FindWindow(L"RegEdit_RegEdit", NULL);
	}

	if (hWndRegEdit)
	{
		RegEdit(hWndRegEdit, szPath);
	}
}

// static
void Automation::RegEdit(HWND hWndRegEdit, PCWSTR szPath)
{
	using namespace UIAutomationClient;
	using IAccessiblePtr = UIAutomationClient::IAccessiblePtr;

	try
	{
		IUIAutomationPtr spUIAutomation;
		::CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&spUIAutomation);
		
		IUIAutomationElementPtr spRegEdit = spUIAutomation->ElementFromHandle(hWndRegEdit);
		spRegEdit->SetFocus();

		IUIAutomationConditionPtr spUIAutomationCondition = spUIAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, UIA_TreeControlTypeId);
		IUIAutomationElementPtr spUIAutomationElement = spRegEdit->FindFirst(TreeScope_Children, spUIAutomationCondition);										// tree

		IUIAutomationConditionPtr spUIAutomationCondition1 = spUIAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, UIA_TreeItemControlTypeId);
		IUIAutomationConditionPtr spUIAutomationCondition2 = spUIAutomation->CreatePropertyCondition(UIA_NamePropertyId, L"Computer");
		spUIAutomationCondition = spUIAutomation->CreateAndCondition(spUIAutomationCondition1, spUIAutomationCondition2);
		spUIAutomationElement = spUIAutomationElement->FindFirst(TreeScope_Children, spUIAutomationCondition);													// item

		struct {
			PCWSTR szShort;
			PCWSTR szLong;
		} map[] = {
			{ L"HKCR", L"HKEY_CLASSES_ROOT" },
			{ L"HKCU", L"HKEY_CURRENT_USER" },
			{ L"HKLM", L"HKEY_LOCAL_MACHINE" },
			{ L"HKU",  L"HKEY_USERS" },
			{ L"HKCC", L"HKEY_CURRENT_CONFIG" },
		};

		bool bDone = false;
		WCHAR szPATH[0x200];
		::wcscpy_s(szPATH, szPath);

		PWSTR szContext = NULL;
		while (PCWSTR szPart = ::wcstok_s(!bDone ? szPATH : NULL, L"/\\", &szContext))
		{
			if (!bDone)
			{
				bDone = true;
				for (int i = 0; i < _countof(map); i++)
				{
					if (!::wcscmp(szPart, map[i].szShort))
					{
						szPart = map[i].szLong;
						break;
					}
				}
			}

			spUIAutomationCondition = spUIAutomation->CreatePropertyConditionEx(UIA_NamePropertyId, szPart, PropertyConditionFlags_IgnoreCase);
			spUIAutomationElement = spUIAutomationElement->FindFirst(TreeScope_Children, spUIAutomationCondition);

			if (spUIAutomationElement)
			{
				IUIAutomationExpandCollapsePatternPtr spUIAutomationExpandCollapsePattern = spUIAutomationElement->GetCurrentPattern(UIA_ExpandCollapsePatternId);
				spUIAutomationExpandCollapsePattern->raw_Expand();
			}
			else if (!::wcstok_s(NULL, L"/\\", &szContext))																											// last part
			{
				spUIAutomationCondition = spUIAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, UIA_ListControlTypeId);
				spUIAutomationElement = spRegEdit->FindFirst(TreeScope_Descendants, spUIAutomationCondition);														// list

				spUIAutomationCondition = spUIAutomation->CreatePropertyConditionEx(UIA_NamePropertyId, szPart, PropertyConditionFlags_IgnoreCase);
				spUIAutomationElement = spRegEdit->FindFirst(TreeScope_Descendants, spUIAutomationCondition);

				if (spUIAutomationElement)
				{
					IUIAutomationSelectionItemPatternPtr spIUIAutomationSelectionItemPattern = spUIAutomationElement->GetCurrentPattern(UIA_SelectionItemPatternId);
					spIUIAutomationSelectionItemPattern->Select();
				}
			}
			else
			{
				break;
			}
		}
	}
	catch (_com_error &e)
	{
		e;
		RPTN(ASSERT, "0x%08x: %S", e.Error(), e.ErrorMessage());
	}
}
