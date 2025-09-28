#pragma once

struct DBGINIT_INFORMATION
{
	unsigned int DbgkCreateThread_Offset;
	unsigned int DbgkCreateThread_Size;
	unsigned int DbgkExitThread_Offset;
	unsigned int DbgkExitThread_Size;
	unsigned int DbgkMapViewOfSection_Offset;
	unsigned int DbgkMapViewOfSection_Size;
	unsigned int DbgkUnMapViewOfSection_Offset;
	unsigned int DbgkUnMapViewOfSection_Size;
	unsigned int DbgkExitProcess_Offset;
	unsigned int DbgkExitProcess_Size;
	unsigned int DbgkForwardException_Offset;
	unsigned int DbgkForwardException_Size;
	unsigned int DbgkpQueueMessage_Offset;
	unsigned int DbgkpQueueMessage_Size;
};