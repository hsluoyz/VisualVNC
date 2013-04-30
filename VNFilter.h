#include <vector>
#include <WinSock2.h>
using namespace std;

#include <setupapi.h>

#define PACKET_IPV4 0
#define PACKET_ARP 1
#define PACKET_IPV6 2
#define PACKET_ICMP 3
#define PACKET_IGMP 4
#define PACKET_TCP 5
#define PACKET_UDP 6
#define PACKET_UNKNOWN 7

#define GETPC1 0xE8
#define GETPC2 0xD9


class VNNIC
{
public:
	int iphlp_index;
	CString iphlp_name;
	CString kernel_name;
	CString friendly_name;
	CString iphlp_desc;
	CString mac;
	CString ip;
	CString mask;
	CString gate;
	CString broadcast;
	BOOL dhcp;
	int enabled;
	BOOL physical;
public:
	VNNIC(int no, CString name)
	{
		this->iphlp_index = no;
		this->iphlp_name = name;
		this->dhcp = FALSE;
		this->enabled = -1;
		this->physical = FALSE;
		this->mac = _T("00-00-00-00-00-00");
		this->ip = _T("0.0.0.0");
		this->mask = _T("0.0.0.0");
		this->gate = _T("0.0.0.0");
		this->broadcast = _T("0.0.0.0");
	}
};

class VNFilter
{
public:
	int m_iPacketNo;
	VNNIC *m_pPhysicalNIC;
	vector<VNNIC*> m_nPhysicalNICs;
	vector<VNNIC*> m_nNICs;

public:
	VNFilter();
	~VNFilter();
	void initPhysicalNIC();
	CString itos(int i);
	int stoi(CString s);
	void setReserveNICs(BOOL bReserveNICs);
	void expandString(char *s, int iDigits);
	void stop();
	
	BOOL isIdenticalNICName(CString strNICName1, CString strNICName2);
	BOOL executeCommand(CString strCommand, CString &strOutput);
	BOOL executeCommand(CString strCommand);

	void Simple_Output_Packets_List(CString strContent);
	void Simple_Output_Packets_List_Without_Increment(CString strContent);

	CString getKernelNameFromIPHlpName(CString strIPHlpName);
	void listNameInfos();
	BOOL listIPInfos();
	BOOL listGateInfos();
	BOOL listFriendlyNameInfos();
	BOOL listFriendlyNameInfos2();
	BOOL listNICs(BOOL bDisplay = TRUE);

	void getOldIPConfig(CString &strOldIP, CString &strOldMask, CString &strOldGate);
	void changeIPConfig(CString strNewIP, CString strNewMask, CString strNewGate);

	void setMTU(VNNIC *pNIC, int iMTU);
	BOOL restoreMTU();
	int checkMTU();
	BOOL changeMTUTo(int iMTU);

	static void enumDevices(HDEVINFO hDevInfo, BOOL bEnable);
	static void rebootNICs();

	BOOL checkIP(CString strIP);
	BOOL checkMask(CString strMask);
	CString getMacFromIP(CString strIP);
	CString getTargetMacOrGateMac(CString strTargetIP);

	static UINT IP_Str2Int_Host(CString strIP);
	static UINT IP_Str2Int_Net(CString strIP);
	static CString IP_Int2Str_Host(UINT iIP);
	static CString IP_Int2Str_Net(UINT iIP);
	static BOOL MAC_Str2Bytes(CString strMAC, PUCHAR pMac);
	static CString MAC_Bytes2Str(PUCHAR pMac);

	void makeWrapper(CString strSrcMac, CString strDstMac, CString strSrcIP, CString strDstIP);
	USHORT checkSum(USHORT *pBuf, UINT nBufLen);
};

/*
#define ETHER_ADDR_LEN 6
#define ETHER_HEADER_LENGTH 14
#define TCP_HEADER_LENGTH 20
#define UDP_HEADER_LENGTH 20

typedef struct _EthernetHeader
{
	UCHAR  dhost[ETHER_ADDR_LEN];
	UCHAR  shost[ETHER_ADDR_LEN];
	USHORT type;
} EthernetHeader, *PEthernetHeader;


typedef struct _ARPHeader
{
	u_short hardType;
	u_short protocolType;
	u_char hardLength;
	u_char proLength;
	u_short operation;
	u_char saddr[6];
	u_char sourceIP[4];
	u_char daddr[6];
	u_char destIP[4];
	unsigned char padding[18];     //填充数据空白数据
} ARPHeader, *PARPHeader;


typedef struct _IPHeader                    // 20字节的IP头
{
    UCHAR     iphVerLen;          // 4位首部长度+4位IP版本号            | Version (4 bits) + Internet header length (4 bits)
    UCHAR     ipTOS;                 // 8位服务类型                                    | TOS Type of service
    USHORT    ipLength;             // 16位封包总长度，即整个IP报的长度   | Total length
    USHORT    ipID;                    // 16位封包标识，惟一标识发送的每一个数据报      | Identification
    USHORT    ipFlags;              // 3位标志位+13报片偏移                 | Flags (3 bits) + Fragment offset (13 bits)
    UCHAR     ipTTL;                 // 8位生存时间，就是TTL                 | Time to live
    UCHAR     ipProtocol;            // 8位协议，TCP、UDP、ICMP等          | Protocol
    USHORT    ipChecksum;    // 16位IP首部校验和                   | Header checksum
    ULONG     ipSource;          // 32位源IP地址                       | Source address
    ULONG     ipDestination;            // 32位目标IP地址                     | Destination address
} IPHeader, *PIPHeader;

typedef struct _TCPHeader          // 20字节的TCP头
{
	USHORT sourcePort;           // 16位源端口号    | Source port
	USHORT destinationPort;      // 16位目的端口号 | Destination port
	ULONG sequenceNumber;       // 32位序列号      | Sequence Number
	ULONG acknowledgeNumber;    // 32位确认号      | Acknowledgement number
	UCHAR   dataoffset;                        // 高4位表示数据偏移，低6位保留字 | Header length
	UCHAR   flags;                  // 6位标志位       | packet flags
	USHORT windows;                    // 16位窗口大小    | Window size
	USHORT checksum;                  // 16位校验和      | Header Checksum
	USHORT urgentPointer;        // 16位紧急数据偏移量   | Urgent pointer...still don't know what this is...
} TCPHeader, *PTCPHeader;

typedef struct _UDPHeader
{
	USHORT               sourcePort;           // 16位源端口号   | Source port
	USHORT               destinationPort;      // 16位目的端口号 | Destination port     
	USHORT               len;                  // 16位封包长度   | Sequence Number
	USHORT               checksum;             // 16位校验和     | Acknowledgement number
} UDPHeader, *PUDPHeader;
*/



CString protocol2String(int iProtocolType);
CString protocol2String2(int iProtocolType);