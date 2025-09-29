#pragma once
struct SKIP_INFORMATION
{
	enum class TYPE
	{
		MOV,
		CMP
	};
	unsigned __int64 Address;
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
	unsigned __int64 ObpReferenceObjectByHandleWithTag_Address;
	unsigned int EPROCESS$DebugPort_Offset;
	unsigned int ETHREAD$HideFromDebugger_Offset;
	SKIP_INFORMATION DbgkCreateThread_DebugPoint;
	SKIP_INFORMATION DbgkExitThread_DebugPoint;
	SKIP_INFORMATION DbgkMapViewOfSection_DebugPoint;
	SKIP_INFORMATION DbgkUnMapViewOfSection_DebugPoint;
	SKIP_INFORMATION DbgkExitProcess_DebugPoint;
	SKIP_INFORMATION DbgkpQueueMessage_DebugPoint;
	SKIP_INFORMATION DbgkForwardException_DebugPoint;
	SKIP_INFORMATION DbgkForwardException_HideFromDebugger;
};