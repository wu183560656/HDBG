#include "pch.h"
#include "tool.h"

namespace tool
{
	std::wstring GetCurrentPath()
	{
		static std::wstring path;
		if (path.empty())
		{
			WCHAR ModuleName[MAX_PATH] = { 0 };
			DWORD Len = GetModuleFileNameW(NULL, ModuleName, sizeof(ModuleName) / sizeof(ModuleName[0]));
			for (int i = Len - 1; i >= 0; i--)
			{
				if (ModuleName[i] == L'\\')
					break;
				ModuleName[i] = 0;
			}
			path = ModuleName;
		}
		return path;
	}
};