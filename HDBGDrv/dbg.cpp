#include "dbg.h"
#include <ntifs.h>

namespace dbg
{
	//所有DebugPoint列表
	static PVOID DebugPointList[0x10000 / 4] = { NULL };

	bool Initialize(DBGINIT_INFORMATION* pInformation)
	{
		if (
			!pInformation->DbgkCreateThread_Offset || !pInformation->DbgkCreateThread_Size ||
			!pInformation->DbgkExitThread_Offset || !pInformation->DbgkExitThread_Size ||
			!pInformation->DbgkMapViewOfSection_Offset || !pInformation->DbgkMapViewOfSection_Size ||
			!pInformation->DbgkUnMapViewOfSection_Offset || !pInformation->DbgkUnMapViewOfSection_Size ||
			!pInformation->DbgkForwardException_Offset || !pInformation->DbgkForwardException_Size ||
			!pInformation->DbgkpQueueMessage_Offset || !pInformation->DbgkpQueueMessage_Size
			)
		{
			return false;
		}

		return false;
	}
	void Uninitialize()
	{

	}
};
