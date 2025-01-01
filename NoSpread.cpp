/*

	SUBJECT:	Silent NoSpread for SA:MP
	DESC:		ADDRESSES BY M0D_SA

*/



#include <Windows.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <vector>
#include <Tlhelp32.h>

size_t r_wcsstr(const wchar_t* str, const wchar_t* search)
{
    for (size_t i = wcslen(str) - wcslen(search); i > 0; --i)
    {
        if (wcsstr(str + i, search) != NULL)
            return i + 1;
    }

    return -1;
}

bool GetProcessID(const std::wstring& name, DWORD& pid)
{
    DWORD aProcesses[1024]{ 0 };
    DWORD cbNeeded{ 0 };
    DWORD cProcesses{ 0 };

    unsigned int i;

    if (EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded) == 0)
        return false;

    cProcesses = cbNeeded / sizeof(DWORD);

    for (i = 0; i < cProcesses; i++)
    {
        WCHAR module_name[MAX_PATH]{ 0 };
        HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);

        if (process == NULL ||
            GetProcessImageFileNameW(process, module_name, sizeof(module_name) / sizeof(WCHAR)) == 0)
            continue;

        size_t pos = r_wcsstr(module_name, name.c_str());

        if (pos != -1)
        {
            pid = aProcesses[i];
            return true;
        }
    }

    return false;
}

int main()
{

	
	DWORD ProcessID{ 0 };
	GetProcessID(L"gta_sa.exe", ProcessID);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, ProcessID);
	if (hProcess == NULL)
	{
		return 1;
	}

	// Vars
	DWORD SpreadAddr = 0x8D610F;
	bool HK = false;
	char EnableNoSpreadBytes[] = "\xBE\x00\x00\x00\x00";
	char DisableNoSpreadBytes[] = "\xBE\x00\x00\x40\x3F";

	// MAIN CODE
	std::cout << "Silent NoSpread for SA:MP started." << std::endl;
	std::cout << "Press F9 to activate the NoSpread.";
	while (true)
	{

		if (GetAsyncKeyState(VK_F9) & 1)
		{
			if (!HK)
			{
				WriteProcessMemory(hProcess, (LPVOID)SpreadAddr, &EnableNoSpreadBytes, 5, NULL);
				std::cout << "No Spread has been enabled." << std::endl;
				HK = true;
			}

			else
			{
				WriteProcessMemory(hProcess, (LPVOID)SpreadAddr, &DisableNoSpreadBytes, 5, NULL);
				std::cout << "No Spread has been disabled." << std::endl;
				HK = false;
			}
		}
		Sleep(95);
	}
}
