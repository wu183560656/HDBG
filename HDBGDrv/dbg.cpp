#include "dbg.h"
#include <ntifs.h>

namespace dbg
{
	//所有DebugPoint列表
	static PVOID DebugPointList[0x10000 / 4] = { NULL };

	bool Initialize(DBGINIT_INFORMATION* pInformation)
	{
		if (
			!pInformation->DbgkCreateThread_DebugPoint.Address || !pInformation->DbgkExitThread_DebugPoint.Address ||
			!pInformation->DbgkMapViewOfSection_DebugPoint.Address || !pInformation->DbgkUnMapViewOfSection_DebugPoint.Address ||
			!pInformation->DbgkExitProcess_DebugPoint.Address || !pInformation->DbgkpQueueMessage_DebugPoint.Address ||
			!pInformation->DbgkForwardException_DebugPoint.Address || !pInformation->DbgkForwardException_HideFromDebugger.Address
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
