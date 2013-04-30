/*++

Copyright (c) 2009-2020  nudt

Module Name:

    CommonIO.h

Abstract:

    定义程序I/O所需的常量
	1. CTL_CODE
	2. 成功失败码

Author:
   
    chengyong

Environment:

    windows xp

Revision History:

 
--*/


#ifndef __COMMONIO_H__
#define __COMMONIO_H__


#define CTL_VNCIO_BASE      FILE_DEVICE_NETWORK

#define _VNCIO_CTL_CODE(_Function, _Method, _Access)  \
            CTL_CODE(CTL_VNCIO_BASE, _Function, _Method, _Access)

//
// 简单读写
//
#define IOCTL_VNCIO_SIMPLE_READ				\
			_VNCIO_CTL_CODE(0x210, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VNCIO_SIMPLE_WRITE 			\
	        _VNCIO_CTL_CODE(0x211, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VNCIO_START_STOP 				\
	        _VNCIO_CTL_CODE(0x212, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VNCIO_CHANGE_MODE 			\
	        _VNCIO_CTL_CODE(0x213, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VNCIO_WRITE_CORE_RULE			\
	        _VNCIO_CTL_CODE(0x214, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VNCIO_WRITE_MAPPING_RULE		\
	        _VNCIO_CTL_CODE(0x215, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VNCIO_WRITE_DEBUG_RULE		\
	        _VNCIO_CTL_CODE(0x216, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VNCIO_CLEAR_RULES				\
	        _VNCIO_CTL_CODE(0x217, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define STATUS_STOP				0
#define STATUS_START			1
#define MODE_CENTER				0
#define MODE_MARGIN				1
#define MODE_UNKNOWN			2

#define MODE_FUNCTION_MARGIN	1
#define MODE_FUNCTION_CENTER	0

#define MODE_RULES_0			0
#define MODE_RULES_1			1
#define MODE_RULES_2			2
#define MODE_RULES_READY		MODE_RULES_2

#define MODE_SEND				0
#define MODE_RECEIVE			1

#define DEFAULT_MTU				1500

//
// 规则维护系列
//

#pragma pack(1)

typedef struct _myrule
{
	PCHAR protocol;
	ULONG ip_start;
	ULONG ip_end;
}MYRULE, *PMYRULE;

typedef struct _FILTER
{
	LIST_ENTRY list_entry;
	UCHAR protocol;
	ULONG ip_start;
	ULONG ip_end;
}FILTER, *PFILTER;


#define ETHER_ADDR_LEN 6

typedef struct _Eth_Header
{
	UCHAR  eth_DstAddr[ETHER_ADDR_LEN];
	UCHAR  eth_SrcAddr[ETHER_ADDR_LEN];
	USHORT eth_Type;
} Eth_Header, *PEth_Header;

#define ETH_HEADER_LEN sizeof(Eth_Header)

typedef struct _ARP_header //ARP头部
{
	USHORT arp_HardType;
	USHORT arp_ProtType;
	UCHAR arp_hardAddrLen;
	UCHAR arp_protAddrLen;
	USHORT arp_Opcode;
	UCHAR arp_srcHardAddr[6];
	ULONG arp_srcProtAddr;
	UCHAR arp_dstHardAddr[6];
	ULONG arp_dstProtAddr;
	UCHAR arp_padding[18];
} ARP_Header, *PARP_Header;

#define ARP_HEADER_LEN sizeof(ARP_Header)

typedef struct _IP_Header                    // 20字节的IP头
{
    UCHAR     ip_hVerLen;          // 4位首部长度+4位IP版本号            | Version (4 bits) + Internet header length (4 bits)
    UCHAR     ip_TOS;                 // 8位服务类型                                    | TOS Type of service
    USHORT    ip_Length;             // 16位封包总长度，即整个IP报的长度   | Total length
    USHORT    ip_ID;                    // 16位封包标识，惟一标识发送的每一个数据报      | Identification
    USHORT    ip_Flags;              // 3位标志位+13报片偏移                 | Flags (3 bits) + Fragment offset (13 bits)
    UCHAR     ip_TTL;                 // 8位生存时间，就是TTL                 | Time to live
    UCHAR     ip_Protocol;            // 8位协议，TCP、UDP、ICMP等          | Protocol
    USHORT    ip_Checksum;    // 16位IP首部校验和                   | Header checksum
    ULONG     ip_Src;          // 32位源IP地址                       | Source address
    ULONG     ip_Dst;            // 32位目标IP地址                     | Destination address
} IP_Header, *PIP_Header;

#define IP_HEADER_LEN sizeof(IP_Header)

#define IP_OFFSET                               0x0E

//IP Protocol Types
#define PROT_ICMP                               0x01 
#define PROT_TCP                                0x06 
#define PROT_UDP                                0x11 

typedef struct _ADAPT *PADAPT;

//////////////////////////////////内核层
typedef struct _NT_Core_Rule
{
	LIST_ENTRY	listEntry;
	UCHAR		cMAC[6]; //Center MAC Address
	UCHAR										reserved1[2]; //Reserved for alignment
	ULONG		cIP; //Center IP Address
	PADAPT		cAdapt; //Center Adapter
} NT_Core_Rule, *PNT_Core_Rule;

#define NT_CORE_RULE_LEN sizeof(NT_Core_Rule) //24

typedef struct _NT_Mapping_Rule
{
	LIST_ENTRY	listEntry;
	UCHAR		eMAC[6]; //External MAC Address
	UCHAR										reserved1[2]; //Reserved for alignment
	ULONG		vIP; //Virtual IP Address
	UCHAR		iMAC[6]; //Internal MAC Address
	UCHAR										reserved2[2]; //Reserved for alignment
	ULONG		pIP; //Physical IP Address
	PADAPT		iAdapt; //Internal Adapter
} NT_Mapping_Rule, *PNT_Mapping_Rule;

#define NT_MAPPING_RULE_LEN sizeof(NT_Mapping_Rule) //36

typedef struct _NT_Debug_Rule
{
	ULONG		eType; //TYPE_IP | TYPE_ARP_REQUEST | TYPE_ARP_REPLY
	ULONG		iType; //TYPE_IP | TYPE_ARP_REQUEST | TYPE_ARP_REPLY | TYPE_NONE
	UCHAR		eDstMAC[6];
	UCHAR		eSrcMAC[6];
	ULONG		eDstIP;
	ULONG		eSrcIP;
	UCHAR		iDstMAC[6];
	UCHAR		iSrcMAC[6];
	ULONG		iDstIP;
	ULONG		iSrcIP;
	UCHAR		content[30];
} NT_Debug_Rule, *PNT_Debug_Rule;

#define NT_DEBUG_RULE_LEN sizeof(NT_Debug_Rule) //68
#define CONTENT_LEN 30

//////////////////////////////////用户层
typedef struct _Core_Rule
{
	UCHAR		cMAC[6]; //Center MAC Address
	UCHAR										reserved1[2]; //Reserved for alignment
	ULONG		cIP; //Center IP Address
} Core_Rule, *PCore_Rule;

#define CORE_RULE_LEN sizeof(Core_Rule) //12

typedef struct _Mapping_Rule
{
	UCHAR		eMAC[6]; //External MAC Address
	UCHAR										reserved1[2]; //Reserved for alignment
	ULONG		vIP; //Virtual IP Address
	UCHAR		iMAC[6]; //Internal MAC Address
	UCHAR										reserved2[2]; //Reserved for alignment
	ULONG		pIP; //Physical IP Address
} Mapping_Rule, *PMapping_Rule;

#define MAPPING_RULE_LEN sizeof(Mapping_Rule) //24

#define TYPE_IP					0
#define TYPE_ARP_REQUEST		1
#define TYPE_ARP_REPLY			2
#define TYPE_NONE				3

//////////////////////////////////////////////////测试发包数据结构
typedef struct _Debug_Rule
{
	ULONG		eType; //TYPE_IP | TYPE_ARP_REQUEST | TYPE_ARP_REPLY
	ULONG		iType; //TYPE_IP | TYPE_ARP_REQUEST | TYPE_ARP_REPLY | TYPE_NONE
	UCHAR		eDstMAC[6];
	UCHAR		eSrcMAC[6];
	ULONG		eDstIP;
	ULONG		eSrcIP;
	UCHAR		iDstMAC[6];
	UCHAR		iSrcMAC[6];
	ULONG		iDstIP;
	ULONG		iSrcIP;
	UCHAR		content[30];
} Debug_Rule, *PDebug_Rule;

#define DEBUG_RULE_LEN sizeof(Debug_Rule) //68

#endif // __COMMONIO_H__