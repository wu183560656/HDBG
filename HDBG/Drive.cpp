#include "pch.h"
#include "Drive.h"

#include <fylib\fylib\fylib.hpp>
#include <fylib\fylib\user\symbol.hpp>

#include "tool.h"
#include "..\common\drive_type.h"

namespace Drive
{
	static bool SearchInstruction(PUCHAR Address, ULONG Size, ULONG Disp, SKIP_INFORMATION* pInformation)
	{
		bool result = false;
		FYLIB::INSTRUCTION::HDES hde;
		PUCHAR Pos = Address;
		do {
			memset(&hde, 0, sizeof(hde));
			unsigned int Len = FYLIB::INSTRUCTION::X64::Disasm(Pos, &hde);
			if (Len == 0)
				break;
			if (hde.disp.disp32 == Disp)
			{
				if (hde.opcode == 0x8B && hde.modrm_mod == 2)		//MOV reg,[reg+***]
				{
					pInformation->Address = (unsigned __int64)Pos;
					pInformation->Type = SKIP_INFORMATION::TYPE::MOV;
					pInformation->MOV.SourceRegister = (hde.rex_b << 3) | hde.modrm_rm;
					pInformation->MOV.TargetRegister = (hde.rex_r << 3) | hde.modrm_reg;
					pInformation->Size = Len;
					result = true;
					break;
				}
				else if (hde.opcode == 0x83 && hde.modrm_mod == 2 && hde.imm.imm32 == 0)	//CMP [reg+***],0
				{
					pInformation->Address = (unsigned __int64)Pos;
					pInformation->Type = SKIP_INFORMATION::TYPE::CMP;
					pInformation->CMP.SourceRegister = (hde.rex_b << 3) | hde.modrm_rm;
					pInformation->Size = Len;
					result = true;
					break;
				}
			}
			Pos += Len;
		} while (Pos < Address + Size);
		return result;
	}

	bool Initialize()
	{
		bool result = false;
		DBGINIT_INFORMATION Information;
		PUCHAR NtoskrnlBase = NULL;
		PUCHAR NtoskrnlMapperBase = NULL;

		//通过符号表定位需要的点
		do
		{
			NtoskrnlBase = (PUCHAR)FYLIB::GetSystemModuleBase("ntoskrnl.exe", NULL);
			if (!NtoskrnlBase)
				break;
			if (!SYMBOL::Initialize((tool::GetCurrentPath() + L"Symbol").c_str()))
				break;
			//加载符号
			auto mNtoskrnl = SYMBOL::GetModule(L"C:\\Windows\\System32\\ntoskrnl.exe");
			//映射Ntoskrnl到本地
			SIZE_T NtoskrnlSize = 0;
			FYLIB::IMAGE::MapForFile(L"C:\\Windows\\System32\\ntoskrnl.exe", NULL, &NtoskrnlSize, true);
			NtoskrnlMapperBase = (PUCHAR)VirtualAlloc(NULL, NtoskrnlSize, MEM_COMMIT, PAGE_READWRITE);
			if (!FYLIB::IMAGE::MapForFile(L"C:\\Windows\\System32\\ntoskrnl.exe", NtoskrnlMapperBase, &NtoskrnlSize, true))
				break;
			//获取EPROCESS.DebugPort偏移
			ULONG EPROCESS$DebugPort_Offset = mNtoskrnl.GetStructByName(L"_EPROCESS").GetField(L"DebugPort").GetOffset();
			ULONG ETHREAD$HideFromDebugger_Offset = mNtoskrnl.GetStructByName(L"_ETHREAD").GetField(L"HideFromDebugger").GetOffset();
			//解析DbgkCreateThread
			ULONG RVA = mNtoskrnl.GetRVAByName(L"DbgkCreateThread");
			ULONG Size = mNtoskrnl.GetSizeByName(L"DbgkCreateThread");
			if (!SearchInstruction((PUCHAR)NtoskrnlMapperBase + RVA, Size, EPROCESS$DebugPort_Offset, &Information.DbgkCreateThread_DebugPoint))
				break;
			Information.DbgkCreateThread_DebugPoint.Address = Information.DbgkCreateThread_DebugPoint.Address + (NtoskrnlBase - NtoskrnlMapperBase);
			//解析DbgkExitThread
			RVA = mNtoskrnl.GetRVAByName(L"DbgkExitThread");
			Size = mNtoskrnl.GetSizeByName(L"DbgkExitThread");
			if (!SearchInstruction((PUCHAR)NtoskrnlMapperBase + RVA, Size, EPROCESS$DebugPort_Offset, &Information.DbgkExitThread_DebugPoint))
				break;
			Information.DbgkExitThread_DebugPoint.Address = Information.DbgkExitThread_DebugPoint.Address + (NtoskrnlBase - NtoskrnlMapperBase);
			//解析DbgkMapViewOfSection
			RVA = mNtoskrnl.GetRVAByName(L"DbgkMapViewOfSection");
			Size = mNtoskrnl.GetSizeByName(L"DbgkMapViewOfSection");
			if (!SearchInstruction((PUCHAR)NtoskrnlMapperBase + RVA, Size, EPROCESS$DebugPort_Offset, &Information.DbgkMapViewOfSection_DebugPoint))
				break;
			Information.DbgkMapViewOfSection_DebugPoint.Address = Information.DbgkMapViewOfSection_DebugPoint.Address + (NtoskrnlBase - NtoskrnlMapperBase);
			//解析DbgkUnMapViewOfSection
			RVA = mNtoskrnl.GetRVAByName(L"DbgkUnMapViewOfSection");
			Size = mNtoskrnl.GetSizeByName(L"DbgkUnMapViewOfSection");
			if (!SearchInstruction((PUCHAR)NtoskrnlMapperBase + RVA, Size, EPROCESS$DebugPort_Offset, &Information.DbgkUnMapViewOfSection_DebugPoint))
				break;
			Information.DbgkUnMapViewOfSection_DebugPoint.Address = Information.DbgkUnMapViewOfSection_DebugPoint.Address + (NtoskrnlBase - NtoskrnlMapperBase);
			//解析DbgkExitProcess
			RVA = mNtoskrnl.GetRVAByName(L"DbgkExitProcess");
			Size = mNtoskrnl.GetSizeByName(L"DbgkExitProcess");
			if (!SearchInstruction((PUCHAR)NtoskrnlMapperBase + RVA, Size, EPROCESS$DebugPort_Offset, &Information.DbgkExitProcess_DebugPoint))
				break;
			Information.DbgkExitProcess_DebugPoint.Address = Information.DbgkExitProcess_DebugPoint.Address + (NtoskrnlBase - NtoskrnlMapperBase);
			//解析DbgkpQueueMessage
			RVA = mNtoskrnl.GetRVAByName(L"DbgkpQueueMessage");
			Size = mNtoskrnl.GetSizeByName(L"DbgkpQueueMessage");
			if (!SearchInstruction((PUCHAR)NtoskrnlMapperBase + RVA, Size, EPROCESS$DebugPort_Offset, &Information.DbgkpQueueMessage_DebugPoint))
				break;
			Information.DbgkpQueueMessage_DebugPoint.Address = Information.DbgkpQueueMessage_DebugPoint.Address + (NtoskrnlBase - NtoskrnlMapperBase);
			//解析DbgkForwardException
			RVA = mNtoskrnl.GetRVAByName(L"DbgkForwardException");
			Size = mNtoskrnl.GetSizeByName(L"DbgkForwardException");
			if (!SearchInstruction((PUCHAR)NtoskrnlMapperBase + RVA, Size, EPROCESS$DebugPort_Offset, &Information.DbgkForwardException_DebugPoint))
				break;
			Information.DbgkForwardException_DebugPoint.Address = Information.DbgkForwardException_DebugPoint.Address + (NtoskrnlBase - NtoskrnlMapperBase);
			if (!SearchInstruction((PUCHAR)NtoskrnlMapperBase + RVA, Size, ETHREAD$HideFromDebugger_Offset, &Information.DbgkForwardException_HideFromDebugger))
				break;
			Information.DbgkForwardException_HideFromDebugger.Address = Information.DbgkForwardException_HideFromDebugger.Address + (NtoskrnlBase - NtoskrnlMapperBase);

			result = true;
		} while (false);
		if (NtoskrnlMapperBase)
		{
			VirtualFree(NtoskrnlMapperBase, 0, MEM_FREE);
		}
		if (result)
		{
			//加载驱动

			//初始化调试器


		}
		return result;
	}

	void Uninitialize()
	{

	}
}