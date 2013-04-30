#include "precomp.h"
#include "ntstrsafe.h"
#include "../CommonIO.h"

typedef struct in_addr {
	union {
		struct { UCHAR s_b1,s_b2,s_b3,s_b4; } S_un_b;
		struct { USHORT s_w1,s_w2; } S_un_w;
		ULONG S_addr;
	} S_un;
} IN_ADDR, *PIN_ADDR, FAR *LPIN_ADDR;

extern LIST_ENTRY		   list_head;

BOOLEAN isValid(UCHAR protocol,IN_ADDR iaSrc, IN_ADDR iaDst);
// 输入参数：
//	Packet： 被分析的NDIS包描述符
//	bRecOrSend: 如果是接收包，为TRUE;如果为发送包，为FALSE。
// 返回值：
//	理想的情况下，调用者通过返回值以决定如何处理NDIS包：续传、失败、转发
FILTER_STATUS AnalysisPacket(PNDIS_PACKET Packet,  BOOLEAN bRecOrSend)
{
	FILTER_STATUS status = FILTER_STATUS_PASS; // 默认全部通过
	return status;
}