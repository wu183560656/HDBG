#pragma once
struct SKIP_INFORMATION
{
	enum class TYPE
	{
		MOV,
		CMP
	};
	void* Address;
	unsigned int Size;
	TYPE Type;
	struct
	{
		unsigned char SourceRegister;
		unsigned char TargetRegister;
	} MOV;
	struct
	{
		unsigned char SourceRegister;
	}CMP;
};

struct DBGINIT_INFORMATION
{
	unsigned __int64 NtUserFindWindow;

	SKIP_INFORMATION DbgkCreateThread_DebugPoint;
	SKIP_INFORMATION DbgkExitThread_DebugPoint;
	SKIP_INFORMATION DbgkMapViewOfSection_DebugPoint;
	SKIP_INFORMATION DbgkUnMapViewOfSection_DebugPoint;
	SKIP_INFORMATION DbgkExitProcess_DebugPoint;
	SKIP_INFORMATION DbgkpQueueMessage_DebugPoint;
	SKIP_INFORMATION DbgkForwardException_DebugPoint;
	SKIP_INFORMATION DbgkForwardException_HideFromDebugger;
};