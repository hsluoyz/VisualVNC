#include "StdAfx.h"

#include "VNFilter.h"
#include <Iphlpapi.h>
#include <stdio.h>
#include "Registry.h"
#include "CommonIO.h"

#pragma comment(lib, "setupapi.lib")

CString protocol2String(int iProtocolType)
{
	if (iProtocolType == PACKET_IPV4)
		return "IPv4";
	else if (iProtocolType == PACKET_ARP)
		return "ARP&RARP";
	else if (iProtocolType == PACKET_IPV6)
		return "IPv6";
	else if (iProtocolType == PACKET_ICMP)
		return "ICMP";
	else if (iProtocolType == PACKET_IGMP)
		return "IGMP";
	else if (iProtocolType == PACKET_TCP)
		return "TCP";
	else if (iProtocolType == PACKET_UDP)
		return "UDP";
	else if (iProtocolType == PACKET_UNKNOWN)
		return "Unknown";
	else
		return "Unknown";
}

CString protocol2String2(int iProtocolType)
{
	if (iProtocolType == PACKET_IPV4)
		return "IPv4     ";
	else if (iProtocolType == PACKET_ARP)
		return "ARP&RARP ";
	else if (iProtocolType == PACKET_IPV6)
		return "IPv6     ";
	else if (iProtocolType == PACKET_ICMP)
		return "ICMP     ";
	else if (iProtocolType == PACKET_IGMP)
		return "IGMP     ";
	else if (iProtocolType == PACKET_TCP)
		return "TCP      ";
	else if (iProtocolType == PACKET_UDP)
		return "UDP      ";
	else if (iProtocolType == PACKET_UNKNOWN)
		return "Unknown  ";
	else
		return "Unknown  ";
}

VNFilter::VNFilter()
{
	m_iPacketNo = 0;
	m_pPhysicalNIC = NULL;
}

VNFilter::~VNFilter()
{
	for (int i = 0; i < m_nNICs.size(); i ++)
	{
		if (m_nNICs[i])
		{
			delete m_nNICs[i];
		}
	}
	m_nNICs.clear();
	m_nPhysicalNICs.clear();
}

void VNFilter::initPhysicalNIC()
{
	/*
	if (!m_pPhysicalNIC)
	{
		if (m_nNICs.size() == 0)
		{
			listNICs(FALSE);
		}
		CString a, b, c;
		getOldIPConfig(a, b, c);
	}
	*/
	listNICs(FALSE);
	CString a, b, c;
	getOldIPConfig(a, b, c);
}

CString VNFilter::itos(int i)
{
	CString strTemp;
	strTemp.Format(_T("%d"), i);
	return strTemp;
}

int VNFilter::stoi(CString s)
{
	return atoi(s);
}

void VNFilter::setReserveNICs(BOOL bReserveNICs)
{
	//m_bReserveNICs = bReserveNICs;
}

void VNFilter::expandString(char *s, int iDigits)
{
	for (int i = 0; i < iDigits; i ++)
	{
		if (s[i] == '\0')
			s[i] = ' ';
	}
	s[16] = '\0';
}

void VNFilter::Simple_Output_Packets_List(CString strContent)
{
	Output_Console_List(itos(m_iPacketNo ++), strContent);
}

void VNFilter::Simple_Output_Packets_List_Without_Increment(CString strContent)
{
	Output_Console_List(itos(m_iPacketNo), strContent);
}

BOOL VNFilter::isIdenticalNICName(CString strNICName1, CString strNICName2)
{
	int iStart1, iEnd1, iStart2, iEnd2;
	iStart1 = strNICName1.Find("{", 0);
	iEnd1 = strNICName1.Find("}", iStart1);
	iStart2 = strNICName2.Find("{", 0);
	iEnd2 = strNICName2.Find("}", iStart2);

	if (iStart1 == -1 || iEnd1 == -1 || iStart2 == -1 || iEnd2 == -1)
	{
		//MyMessageBox_Error("isIdenticalNICName");
		return FALSE;
	}
	else
	{
		CString strNICID1 = strNICName1.Mid(iStart1, iEnd1 + 1);
		CString strNICID2 = strNICName2.Mid(iStart2, iEnd2 + 1);
		if (strNICID1 == strNICID2)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

BOOL VNFilter::executeCommand(CString strCommand, CString &strOutput)
{
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead,hWrite;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		MyMessageBox_Error(_T("executeCommand"));
		return FALSE;
	}
	char command[1024]; //长达1K的命令行，够用了吧 
	strcpy(command, "Cmd.exe /C ");
	strcat(command, strCommand);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si); 
	//si.hStdError = hWrite;            //把创建进程的标准错误输出重定向到管道输入 
	si.hStdOutput = hWrite;           //把创建进程的标准输出重定向到管道输入 
	si.wShowWindow = SW_HIDE; 
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//关键步骤，CreateProcess函数参数意义请查阅MSDN
	if (!CreateProcess(NULL, command, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		CloseHandle(hWrite);
		CloseHandle(hRead);
		MyMessageBox_Error(_T("executeCommand"));
		return FALSE;
	}
	CloseHandle(hWrite);
	char buffer[8192] = {0};          //用4K的空间来存储输出的内容，只要不是显示文件内容，一般情况下是够用了。 
	DWORD bytesRead;

	WaitForSingleObject(pi.hProcess, INFINITE);
	while (TRUE)
	{
		if (ReadFile(hRead, buffer, 8191, &bytesRead, NULL) == NULL)
			break;
		//buffer中就是执行的结果，可以保存到文本，也可以直接输出
		strOutput += buffer;
		//AfxMessageBox(buffer);   //这里是弹出对话框显示
	} 
	CloseHandle(hRead);
	return TRUE;
}

BOOL VNFilter::executeCommand(CString strCommand)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	char command[1024]; //长达1K的命令行，够用了吧 
	strcpy(command, "Cmd.exe /C ");
	strcat(command, strCommand);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si); 
	si.wShowWindow = SW_HIDE; 
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//关键步骤，CreateProcess函数参数意义请查阅MSDN
	if (!CreateProcess(NULL, command, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		MyMessageBox_Error(_T("executeCommand"));
		return FALSE;
	}
	
	WaitForSingleObject(pi.hProcess, INFINITE);

	return TRUE;
}

CString VNFilter::getKernelNameFromIPHlpName(CString strIPHlpName)
{
	int iStart = 0;
	int iEnd = 0;
	iStart = strIPHlpName.Find(_T("{"), iStart);
	iEnd = strIPHlpName.Find(_T("}"), iEnd);
	if (iStart == -1 || iEnd == -1)
	{
		MyMessageBox_Error(_T("getKernelNameFromIPHlpName"));
		return _T("");
	}
	else
	{
		return strIPHlpName.Mid(iStart, iEnd - iStart + 1);
	}
}

void VNFilter::listNameInfos() //get index, name
{
	char strTmp[1024];
    PIP_INTERFACE_INFO      pInterfaceInfo;
    PIP_ADAPTER_INDEX_MAP   pIndexMap;
	PBYTE   m_pBuffer = NULL;
	ULONG   m_ulSize;
	DWORD   m_dwResult;

    GetInterfaceInfo((PIP_INTERFACE_INFO) m_pBuffer, &m_ulSize);
	if (m_ulSize > 32768)
	{
		m_ulSize = 32768;
	}
    m_pBuffer = new BYTE[m_ulSize];
    if (NULL != m_pBuffer)
    {
        m_dwResult = GetInterfaceInfo((PIP_INTERFACE_INFO) m_pBuffer, &m_ulSize);
        if (m_dwResult == NO_ERROR)
        {
            pInterfaceInfo = (PIP_INTERFACE_INFO) m_pBuffer;
			
            for (int x = 0; x < pInterfaceInfo->NumAdapters; x++)
            {
				//sprintf(strTmp, "[Adapter index]: %lu  [Adapter name]: %S", pIndexMap->Index, pIndexMap->Name);
                //Simple_Output_Packets_List_Without_Increment(strTmp);

                pIndexMap = (PIP_ADAPTER_INDEX_MAP) &(pInterfaceInfo->Adapter[x]);
				VNNIC *pNIC = new VNNIC(pIndexMap->Index, pIndexMap->Name);
				m_nNICs.push_back(pNIC);
				pNIC->kernel_name = getKernelNameFromIPHlpName(pNIC->iphlp_name);
            }
        }
        else
        {
            sprintf(strTmp, "GetInterfaceInfo() failed.  Result = %lu", m_dwResult);
            Simple_Output_Packets_List_Without_Increment(strTmp);
        }
		
        delete [] m_pBuffer;
    }
}

BOOL VNFilter::listIPInfos() //get ip, mask, broadcast, enabled(before)
{
    PMIB_IPADDRTABLE    pAddrTable;
    PMIB_IPADDRROW      pAddrRow;
    in_addr             ia;
	PBYTE   m_pBuffer = NULL;
    ULONG   m_ulSize;
	DWORD   m_dwResult;
	CString strIP;
	CString strSubnet;
	CString strBroadcast;

	if (m_nNICs.size() == 0)
	{
		return TRUE;
	}

    GetIpAddrTable((PMIB_IPADDRTABLE) m_pBuffer, &m_ulSize, TRUE);
	if (m_ulSize > 32768)
	{
		m_ulSize = 32768;
	}
    m_pBuffer = new BYTE[m_ulSize];
    if (NULL != m_pBuffer)
    {
        m_dwResult = GetIpAddrTable((PMIB_IPADDRTABLE) m_pBuffer, &m_ulSize, TRUE);
		
        if (m_dwResult == NO_ERROR)
        {
            pAddrTable = (PMIB_IPADDRTABLE) m_pBuffer;
			
            for (int x = 0; x < pAddrTable->dwNumEntries; x++)
            {
                pAddrRow = (PMIB_IPADDRROW) &(pAddrTable->table[x]);
                /*
                ia.S_un.S_addr = pAddrRow->dwAddr;
                m_strText.Format("IP address: %s\r\n", inet_ntoa(ia));
                m_strText.Format("Interface index: %lu\r\n", pAddrRow->dwIndex);
                ia.S_un.S_addr = pAddrRow->dwMask;
                m_strText.Format("Subnet mask: %s\r\n", inet_ntoa(ia));
                ia.S_un.S_addr = pAddrRow->dwBCastAddr;
                m_strText.Format("Broadcast address: %s\r\n", inet_ntoa(ia));
				*/
				ia.S_un.S_addr = pAddrRow->dwAddr;
                strIP.Format("%s", inet_ntoa(ia));
                ia.S_un.S_addr = pAddrRow->dwMask;
                strSubnet.Format("%s", inet_ntoa(ia));
                ia.S_un.S_addr = pAddrRow->dwBCastAddr;
                strBroadcast.Format("%s", inet_ntoa(ia));

				for (int i = 0; i < m_nNICs.size(); i ++)
				{
					if (m_nNICs[i]->iphlp_index == pAddrRow->dwIndex)
					{
						m_nNICs[i]->ip = strIP;
						m_nNICs[i]->mask = strSubnet;
						m_nNICs[i]->broadcast = strBroadcast;
						break;
					}
				}

            }

			/*
			for (int i = 0; i < m_nNICs.size(); i ++)
			{
				if (m_nNICs[i]->ip == _T("0.0.0.0"))
				{
					m_nNICs[i]->enabled = FALSE;
					//MyMessageBox_Error("listIPInfos");
					//delete []m_pBuffer;
					//return FALSE;
				}
			}
			*/

        }
        else
        {
            MyMessageBox_Error("listIPInfos");
        }
		
        delete []m_pBuffer;
    }
	
    return TRUE;
}

BOOL VNFilter::listGateInfos() //get mac, iphlp_desc, gate, dhcp, physical
{
	ULONG cbBuf = 0;
    PIP_ADAPTER_INFO pAdapter = NULL;
    PIP_ADAPTER_INFO pMemory  = NULL;
    DWORD dwResult = 0;
	
    dwResult = GetAdaptersInfo(NULL, &cbBuf);
	if (cbBuf > 32768)
	{
		cbBuf = 32768;
	}
    pMemory = pAdapter = (PIP_ADAPTER_INFO) malloc(cbBuf);
    dwResult = GetAdaptersInfo(pAdapter, &cbBuf);
	if (dwResult != ERROR_SUCCESS)
	{
		if (dwResult == ERROR_NO_DATA)
		{
			//MyMessageBox_Error(_T("No network adapter available!"), _T("Run Info"));
		}
		else
		{
			MyMessageBox_Error(_T("GetAdaptersInfo"));
		}
		return FALSE;
	}
	
    while (pAdapter)
    {
		/*
        printf("Name: %s\nDescription: %s\nGateway: %s\nSubnet Mask: %s\n",
			pAdapter->AdapterName,
			pAdapter->Description,
			pAdapter->GatewayList.IpAddress.String,
			pAdapter->IpAddressList.IpMask.String);
		*/
		for (int i = 0; i < m_nNICs.size(); i ++)
		{

			if (isIdenticalNICName(m_nNICs[i]->iphlp_name, pAdapter->AdapterName))
			{
				CString strMac;
				strMac.Format( "%02x-%02x-%02x-%02x-%02x-%02x\r\n",
					pAdapter->Address[0],
					pAdapter->Address[1],
					pAdapter->Address[2],
					pAdapter->Address[3],
					pAdapter->Address[4],
					pAdapter->Address[5]);
				strMac.MakeUpper();
				m_nNICs[i]->mac = strMac;
				m_nNICs[i]->iphlp_desc = pAdapter->Description;
				m_nNICs[i]->dhcp = pAdapter->DhcpEnabled;
				if (m_nNICs[i]->iphlp_desc.Find(_T("PCI")) != -1 || m_nNICs[i]->iphlp_desc.Find(_T("PRO/1000")) != -1 ||
					 m_nNICs[i]->iphlp_desc.Find(_T("AMD")) != -1 || m_nNICs[i]->iphlp_desc.Find(_T("PCNet")) != -1 ||
					 m_nNICs[i]->iphlp_desc.Find(_T("Broadcom")) != -1)
				{
					m_nNICs[i]->physical = TRUE;
				}
				m_nNICs[i]->gate = pAdapter->GatewayList.IpAddress.String;
			}
		}
		
        pAdapter = pAdapter->Next;
    }
	
    free(pMemory);
	return TRUE;
}

BOOL VNFilter::listFriendlyNameInfos() //get friendly name, enabled
{
	PIP_ADAPTER_ADDRESSES pAddresses;
	ULONG iOutBufLen = 0;
	DWORD dwRetVal = 0;
	//char pBuff[50];
	//DWORD iBuff = 50;
	int addrLen = sizeof(SOCKADDR_STORAGE);
	pAddresses = NULL;//(IP_ADAPTER_ADDRESSES*) malloc(sizeof(IP_ADAPTER_ADDRESSES));
	// 第一次使用 GetAdaptersAddresses 从 outBufLen 中获取需要的缓冲区大小
	if (GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &iOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		//free(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES*) malloc(iOutBufLen);
	}
	else
	{
		MyMessageBox_Error(_T("listFriendlyNameInfos"));
		return FALSE;
	}
	
	// 第二次调用GetAdaptersAddresses 获取实际想获取的信息.
	if ((dwRetVal = GetAdaptersAddresses(AF_INET, 0 , NULL, pAddresses, &iOutBufLen)) == NO_ERROR) 
	{
		PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
		while (pCurrAddresses) 
		{
			CString strFriendlyName = pCurrAddresses->FriendlyName;
			CString strAdapterName = pCurrAddresses->AdapterName;
			for (int i = 0; i < m_nNICs.size(); i ++)
			{
				if (isIdenticalNICName(strAdapterName, m_nNICs[i]->iphlp_name))
				{
					m_nNICs[i]->friendly_name = strFriendlyName;
					if (pCurrAddresses->OperStatus == IfOperStatusUp)
					{
						m_nNICs[i]->enabled = 1;
					}
					else if (pCurrAddresses->OperStatus == IfOperStatusDown)
					{
						m_nNICs[i]->enabled = 0;
					}
					else
					{
						m_nNICs[i]->enabled = -1;
					}
				}
			}
			/*
			if (pCurrAddresses->FirstUnicastAddress !=NULL)
			{
				WSAAddressToString(pCurrAddresses->FirstUnicastAddress->Address.lpSockaddr,
					pCurrAddresses->FirstUnicastAddress->Address.iSockaddrLength, NULL, pBuff, &iBuff);
				
				if (strncmp(pBuff,"fe80",4)==0||strcmp(pBuff,"::1") ==0)
				{
				}
				else
				{
					//memcpy(IPAddr,buff,bufflen);
					return;
				}
				
			}
			else
			{
				MyMessageBox_Error(_T("listFriendlyNameInfos"));
				return FALSE;
			}
			*/
			pCurrAddresses = pCurrAddresses->Next;
		}
	}
	return TRUE;
}

BOOL VNFilter::listFriendlyNameInfos2() //get friendly name
{
	CString strCommand = _T("netsh interface ip show address");
	CString strOutput;
	executeCommand(strCommand, strOutput);

	int iPos = 0;
	int iPre = 0;

	while ((iPos = strOutput.Find(_T("\""), iPos + 1)) != -1)
	{
		iPre = iPos;
		iPos = strOutput.Find(_T("\""), iPos + 1);
		if (iPos == -1)
		{
			MyMessageBox_Error(_T("listDisplayNameInfos"));
			return FALSE;
		}

		CString strDisplayName = strOutput.Mid(iPre + 1, iPos - iPre - 1);

		iPos = strOutput.Find(_T("IP"), iPos + 1);
		if (iPos == -1)
		{
			MyMessageBox_Error(_T("listDisplayNameInfos"));
			return FALSE;
		}

		iPos = strOutput.Find(_T("."), iPos + 1);
		if (iPos == -1)
		{
			MyMessageBox_Error(_T("listDisplayNameInfos"));
			return FALSE;
		}

		int iIPStart = iPos;
		int iIPEnd = iPos;

		TCHAR c;
		while ((c = strOutput.GetAt(iIPStart)) == _T('.') || (_T('0') <= c) && c <= _T('9'))
		{
			iIPStart --;
			if (iIPStart < 0)
			{
				MyMessageBox_Error(_T("listDisplayNameInfos"));
				return FALSE;
			}
		}
		iIPStart ++;

		while ((c = strOutput.GetAt(iIPEnd)) == _T('.') || (_T('0') <= c) && c <= _T('9'))
		{
			iIPEnd ++;
			if (iIPStart >= strOutput.GetLength())
			{
				MyMessageBox_Error(_T("listDisplayNameInfos"));
				return FALSE;
			}
		}
		iIPEnd --;

		CString strIP = strOutput.Mid(iIPStart, iIPEnd - iIPStart + 1);

		for (int i = 0; i < m_nNICs.size(); i ++)
		{
			if (strIP == m_nNICs[i]->ip)
			{
				m_nNICs[i]->friendly_name = strDisplayName;
			}
		}
	}

	return TRUE;
}

BOOL VNFilter::listNICs(BOOL bDisplay)
{
	m_nPhysicalNICs.clear();
	m_nNICs.clear();
	char strTmp[1024];

	listNameInfos();
	listIPInfos();
	listGateInfos();
	listFriendlyNameInfos();
	//listFriendlyNameInfos2(); //this method is a little slow

	if (bDisplay)
	{
		for (int i = 0; i < m_nNICs.size(); i ++)
		{
			sprintf(strTmp, "[NIC IPHelper Index]: %d   [NIC IPHelper Name]: %s", m_nNICs[i]->iphlp_index,
				m_nNICs[i]->iphlp_name);
			Simple_Output_Packets_List_Without_Increment(strTmp);
			sprintf(strTmp, "[NIC IPHelper Description]: %s", m_nNICs[i]->iphlp_desc);
			Simple_Output_Packets_List_Without_Increment(strTmp);

			sprintf(strTmp, "[NIC Friendly Name]: %s", m_nNICs[i]->friendly_name);
			Simple_Output_Packets_List_Without_Increment(strTmp);

			sprintf(strTmp, "[NIC Mac Address]: %s", m_nNICs[i]->mac);
			Simple_Output_Packets_List_Without_Increment(strTmp);

			sprintf(strTmp, "[NIC IP Address]: %s  [NIC IP Mask]: %s", m_nNICs[i]->ip, m_nNICs[i]->mask);
			Simple_Output_Packets_List_Without_Increment(strTmp);
			sprintf(strTmp, "[NIC IP Gateway]: %s  [NIC DHCP]: %s", m_nNICs[i]->gate,
				m_nNICs[i]->dhcp? _T("On") : _T("Off"));

			Simple_Output_Packets_List_Without_Increment(strTmp);
			sprintf(strTmp, "[NIC Type]: %s  [NIC State]: %s", m_nNICs[i]->physical? _T("Physical") : _T("Virtual"),
				m_nNICs[i]->enabled? _T("On") : _T("Off"));

			Simple_Output_Packets_List(strTmp);
			Simple_Output_Packets_List_Without_Increment(_T(""));
		}
	}
	return TRUE;
}

void VNFilter::getOldIPConfig(CString &strOldIP, CString &strOldMask, CString &strOldGate)
{
	m_pPhysicalNIC = NULL;
	m_nPhysicalNICs.clear();
	for (int i = 0; i < m_nNICs.size(); i ++)
	{
		if (m_nNICs[i]->physical)
		{
			m_pPhysicalNIC = m_nNICs[i];
			m_nPhysicalNICs.push_back(m_nNICs[i]);
			//break;
		}
	}
	if (m_pPhysicalNIC == NULL)
	{
		MyMessageBox_Error(_T("changeIPConfig"));
		return;
	}

	strOldIP = m_pPhysicalNIC->ip;
	strOldMask = m_pPhysicalNIC->mask;
	strOldGate = m_pPhysicalNIC->gate;
}

void VNFilter::changeIPConfig(CString strNewIP, CString strNewMask, CString strNewGate)
{
	m_pPhysicalNIC = NULL;
	m_nPhysicalNICs.clear();
	if (!checkIP(strNewIP) || !checkMask(strNewMask) || !checkIP(strNewGate))
	{
		MyMessageBox_Error(_T("Input format error!"), _T("Run Info"));
		return;
	}

	for (int i = 0; i < m_nNICs.size(); i ++)
	{
		if (m_nNICs[i]->physical)
		{
			m_pPhysicalNIC = m_nNICs[i];
			m_nPhysicalNICs.push_back(m_nNICs[i]);
			//break;
		}
	}
	if (m_pPhysicalNIC == NULL)
	{
		MyMessageBox_Error(_T("changeIPConfig"));
		return;
	}

	CString strCommand;
	strCommand.Format(_T("netsh interface ip set address \"%s\" static %s %s %s 1"), m_pPhysicalNIC->friendly_name, strNewIP, strNewMask, 
		strNewGate);
	//ShellExecute(NULL, _T("Open"), strCommand, _T(""), _T(""), SW_HIDE);
	executeCommand(strCommand);
}

void VNFilter::setMTU(VNNIC *pNIC, int iMTU)
{
	BOOL bRet;
	if (iMTU <= 0 || iMTU > 1500)
	{
		MyMessageBox_Error(_T("changeMTU"));
		return;
	}

	CRegistry registry(HKEY_LOCAL_MACHINE);
	CString strSubKey = _T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\") + pNIC->kernel_name;
	bRet = registry.Open(strSubKey);
	if (!bRet)
	{
		MyMessageBox_Error(_T("setMTU"));
		return;
	}

	bRet = registry.Write(_T("MTU"), (DWORD) iMTU);
	if (!bRet)
	{
		MyMessageBox_Error(_T("setMTU"));
		return;
	}

	bRet = registry.Flush();
	if (!bRet)
	{
		MyMessageBox_Error(_T("setMTU"));
		return;
	}
}

BOOL VNFilter::restoreMTU()
{
	if (!m_pPhysicalNIC)
	{
		initPhysicalNIC();
	}

	BOOL bRet;
	CRegistry registry(HKEY_LOCAL_MACHINE);
	CString strSubKey = _T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\") +
		m_pPhysicalNIC->kernel_name;
	bRet = registry.Open(strSubKey);
	if (!bRet)
	{
		MyMessageBox_Error(_T("deleteMTU"));
		return FALSE;
	}
	
	bRet = registry.DeleteValue(_T("MTU"));
	if (!bRet)
	{
		MyMessageBox_Error(_T("deleteMTU"));
		return FALSE;
	}

	bRet = registry.Flush();
	if (!bRet)
	{
		MyMessageBox_Error(_T("deleteMTU"));
		return FALSE;
	}

	return TRUE;
}

int VNFilter::checkMTU()
{
	if (!m_pPhysicalNIC)
	{
		initPhysicalNIC();
	}

	int iRet;
	CRegistry registry(HKEY_LOCAL_MACHINE);
	CString strSubKey = _T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\") +
		m_pPhysicalNIC->kernel_name;
	iRet = registry.Open(strSubKey);
	if (!iRet)
	{
		MyMessageBox_Error(_T("checkMTU"));
		return -1;
	}
	
	DWORD dwMTU;
	iRet = registry.Read(_T("MTU"), &dwMTU);
	if (iRet == 1)
	{
		return ((int) dwMTU);
	}
	else if (iRet == -1)
	{
		return DEFAULT_MTU;
	}
	else
	{
		MyMessageBox_Error(_T("checkMTU"));
		return -1;
	}
}

BOOL VNFilter::changeMTUTo(int iMTU)
{
	if (!m_pPhysicalNIC)
	{
		initPhysicalNIC();
	}

	if (iMTU <= 0 || iMTU > 1500)
	{
		MyMessageBox_Error(_T("MTU不符合要求！"), _T("Run Info"));
		return FALSE;
	}

	//ping -f -l 1464 www.baidu.com
	//可知MTU为1464 + 28 = 1492
	//这是由于PPPoE占用8Bytes，因此MTU = 1500 - 8 = 1492
	//我们封装IP的时候至少占8Bytes，所以把MTU改成1492 - 10 = 1482
		
	int iNowMTU = checkMTU();
	if (iNowMTU != iMTU)
	{
		setMTU(m_pPhysicalNIC, iMTU);
		return TRUE;
	}
	else
	{
		MyMessageBox_Error(_T("MTU已经为设置为此值，不用重复设置！"), _T("Run Info"));
		return FALSE;
	}
}

void VNFilter::enumDevices(HDEVINFO hDevInfo, BOOL bEnable)
{
	SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)};
	LPOLESTR guid;
	char devName[128];
	char instanceId[128];
	for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i ++)
	{
		StringFromCLSID(DeviceInfoData.ClassGuid, &guid);
		SetupDiClassNameFromGuid(&DeviceInfoData.ClassGuid, devName, 128, NULL);
		if (!strcmp(devName, "Net"))
		{
			SetupDiGetDeviceInstanceId(hDevInfo, &DeviceInfoData, instanceId, 128, NULL);
			//if (!strncmp(instanceId, "PCI", 3))
			//{
				// wprintf(L"%s\n",guid);
				// printf("%s\n",devName);
				//  printf("%s\n",instanceId);
				SP_PROPCHANGE_PARAMS params = {sizeof(SP_CLASSINSTALL_HEADER)};
				params.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
				params.Scope = DICS_FLAG_CONFIGSPECIFIC;
				//params.StateChange = DICS_ENABLE; //禁用：DICS_DISABLE，DICS_ENABLE启用
				if (bEnable)
				{
					params.StateChange = DICS_ENABLE;
				}
				else
				{
					params.StateChange = DICS_DISABLE;
				}
				params.HwProfile = 0;
				SetupDiSetClassInstallParams(hDevInfo, &DeviceInfoData, (SP_CLASSINSTALL_HEADER*) &params, sizeof(SP_PROPCHANGE_PARAMS));
				SetupDiChangeState(hDevInfo, &DeviceInfoData);
			//}
		}
		CoTaskMemFree(guid);
	}
}

void VNFilter::rebootNICs()
{
	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	hDevInfo = SetupDiGetClassDevs(NULL,NULL,NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
	if(INVALID_HANDLE_VALUE == hDevInfo)
	{
		MyMessageBox_Error(_T("rebootNICs"));
	}
	enumDevices(hDevInfo, FALSE);
	//enumDevices(hDevInfo, TRUE);
	SetupDiDestroyDeviceInfoList(hDevInfo);
}

BOOL VNFilter::checkIP(CString strIP)
{
	char *str = strIP.GetBuffer(0);
	
    int segs = 0;   /* Segment count. */
    int chcnt = 0;  /* Character count within segment. */
    int accum = 0;  /* Accumulator for segment. */
	
    /* Catch NULL pointer. */
	
    if (str == NULL)
        return 0;
	
    /* Process every character in string. */
	
    while (*str != '\0') {
        /* Segment changeover. */
		
        if (*str == '.') {
            /* Must have some digits in segment. */
			
            if (chcnt == 0)
                return 0;
			
            /* Limit number of segments. */
			
            if (++segs == 4)
                return 0;
			
            /* Reset segment values and restart loop. */
			
            chcnt = accum = 0;
            str++;
            continue;
        }
		
		
        /* Check numeric. */
		
        if ((*str < '0') || (*str > '9'))
            return 0;
		
        /* Accumulate and check segment. */
		
        if ((accum = accum * 10 + *str - '0') > 255)
            return 0;
		
        /* Advance other segment specific stuff and continue loop. */
		
        chcnt++;
        str++;
    }
	
    /* Check enough segments and enough characters in last segment. */
	
    if (segs != 3)
        return 0;
	
    if (chcnt == 0)
        return 0;
	
    /* Address okay. */
	
    return 1;
}

BOOL VNFilter::checkMask(CString strMask)
{
	if (checkIP(strMask) == FALSE)
	{
		return FALSE;
	}
	
	CString strPart1, strPart2, strPart3, strPart4;
	int iPos = 0;
	int iPre = 0;
	
	iPos = strMask.Find(_T("."), iPos + 1);
	strPart1 = strMask.Mid(iPre, iPos - iPre);
	iPre = iPos + 1;
	
	iPos = strMask.Find(_T("."), iPos + 1);
	strPart2 = strMask.Mid(iPre, iPos - iPre);
	iPre = iPos + 1;
	
	iPos = strMask.Find(_T("."), iPos + 1);
	strPart3 = strMask.Mid(iPre, iPos - iPre);
	iPre = iPos + 1;
	
	if (strMask.Find(_T("."), iPos + 1) != -1)
	{
		return FALSE;
	}
	
	strPart4 = strMask.Mid(iPre, strMask.GetLength() - iPre);
	
	if (!(strPart1 == _T("255") || strPart1 == _T("254") || strPart1 == _T("252") || strPart1 == _T("248") || strPart1 == _T("240") ||
		strPart1 == _T("224") || strPart1 == _T("192") || strPart1 == _T("128") || strPart1 == _T("0")))
	{
		return FALSE;
	}
	if (!(strPart2 == _T("255") || strPart2 == _T("254") || strPart2 == _T("252") || strPart2 == _T("248") || strPart2 == _T("240") ||
		strPart2 == _T("224") || strPart2 == _T("192") || strPart2 == _T("128") || strPart2 == _T("0")))
	{
		return FALSE;
	}
	if (!(strPart3 == _T("255") || strPart3 == _T("254") || strPart3 == _T("252") || strPart3 == _T("248") || strPart3 == _T("240") ||
		strPart3 == _T("224") || strPart3 == _T("192") || strPart3 == _T("128") || strPart3 == _T("0")))
	{
		return FALSE;
	}
	if (!(strPart4 == _T("255") || strPart4 == _T("254") || strPart4 == _T("252") || strPart4 == _T("248") || strPart4 == _T("240") ||
		strPart4 == _T("224") || strPart4 == _T("192") || strPart4 == _T("128") || strPart4 == _T("0")))
	{
		return FALSE;
	}
	
	if (strPart1 != _T("255") && (strPart2 != _T("0") || strPart3 != _T("0") || strPart4 != _T("0")))
	{
		return FALSE;
	}
	if (strPart2 != _T("255") && (strPart3 != _T("0") || strPart4 != _T("0")))
	{
		return FALSE;
	}
	if (strPart3 != _T("255") && strPart4 != _T("0"))
	{
		return FALSE;
	}
	
	return TRUE;
}

CString VNFilter::getMacFromIP(CString strIP)
{
	if (!checkIP(strIP))
	{
		MyMessageBox_Error(_T("getMacFromIP"));
		return _T("");
	}
	CString strMac;

	unsigned long ulAddr = inet_addr(strIP);
	unsigned long ulMAC[2];
	unsigned long ulLen = 6;
	DWORD dwRetVal  = SendARP(ulAddr, 0, ulMAC, &ulLen); 
	if (dwRetVal == NO_ERROR)
	{
		size_t i, j;
		char *szMac = new char[ulLen*3];
		PBYTE pbHexMac = (PBYTE) ulMAC;
		for (i = 0, j = 0; i < ulLen - 1; ++ i) 
		{
			j += sprintf(szMac + j, "%02X-", pbHexMac[i]);
		}
		sprintf(szMac + j, "%02X", pbHexMac[i]);
		strMac = szMac;
		delete []szMac;
		return strMac;
	}
	else
	{
		//目标IP地址不在本局域网内
		return _T("");
	}
}

CString VNFilter::getTargetMacOrGateMac(CString strTargetIP)
{
	CString strMac = getMacFromIP(strTargetIP);
	if (strMac == _T(""))
	{
		initPhysicalNIC();
		strMac = getMacFromIP(m_pPhysicalNIC->gate);
		if (strMac != _T(""))
		{
			return strMac; //目标IP不在本网段，只能得到网关MAC地址
		}
		else
		{
			MyMessageBox_Error(_T("getTargetMacOrGateMac"));
			return _T("");
		}
	}
	else
	{
		return strMac; //直接得到目标MAC地址
	}
}

UINT VNFilter::IP_Str2Int_Host(CString strIP)
{
	char *szIP = new char[(strIP.GetLength() + 1) * sizeof(CHAR)];
	strcpy(szIP, strIP.GetBuffer(0));
	char *p;
	int sections[4] = {0};
	int i = 0;
	int j = 0;

	p = strtok(szIP,"."); 
	while (p)
	{
		sections[i] = atoi(p);
		p = strtok(NULL, ".");
		//cout << sections[i] << endl;
		i ++;
		if (p != NULL && i == 4)
		{
			MyMessageBox_Error(_T("IPStr2Num_Host"));
			return 0;
		}
	}  
	
	unsigned int num =0;
	for (j = 3, i = 0; j >= 0; j --,i ++)
	{
		num += (sections[i] << (8 * j));
	}
	
	delete szIP;
	return num;
}

UINT VNFilter::IP_Str2Int_Net(CString strIP)
{
	char *szIP = new char[(strIP.GetLength() + 1) * sizeof(CHAR)];
	strcpy(szIP, strIP.GetBuffer(0));
	char *p;
	int sections[4] = {0};
	int i = 0;
	int j = 0;
	
	p = strtok(szIP,"."); 
	while (p)
	{
		sections[i] = atoi(p);
		p = strtok(NULL, ".");
		//cout << sections[i] << endl;
		i ++;
		if (p != NULL && i == 4)
		{
			MyMessageBox_Error(_T("IPStr2Num_Net"));
			return 0;
		}
	}  
	
	unsigned int num =0;
	for (j = 3, i = 3; j >= 0; j --,i --)
	{
		num += (sections[i] << (8 * j));
	}
	
	delete szIP;
	return num;
}

CString VNFilter::IP_Int2Str_Host(UINT iIP) //host sequence
{  
    char *buf = new char[sizeof("aaa.bbb.ccc.ddd")];
    unsigned char *p = (unsigned char *) &iIP;
    sprintf(buf, "%d.%d.%d.%d", p[3] & 0xff, p[2] & 0xff, p[1] & 0xff, p[0] & 0xff);
	CString strIP = buf;
	delete buf;
    return strIP;
}

CString VNFilter::IP_Int2Str_Net(UINT iIP) //network sequence
{  
    char *buf = new char[sizeof("aaa.bbb.ccc.ddd")];
    unsigned char *p = (unsigned char *) &iIP;  
    sprintf(buf, "%d.%d.%d.%d", p[0] & 0xff, p[1] & 0xff, p[2] & 0xff, p[3] & 0xff);
	CString strIP = buf;
	delete buf;
    return strIP;
}

BOOL VNFilter::MAC_Str2Bytes(CString strMAC, PUCHAR pMac)
{
	if (pMac == NULL)
	{
		MyMessageBox_Error(_T("MAC_Str2Bytes"));
		return FALSE; 
	}

	CString strMACTokenWiped;
	int iTokenCnt = 0;
	for (int k = 0; k < strMAC.GetLength(); k ++)
	{
		if (strMAC[k] != _T('-') && strMAC[k] != _T(':'))
		{
			strMACTokenWiped += strMAC[k];
		}
		else
		{
			iTokenCnt ++;
		}
	}
	if (iTokenCnt != 5)
	{
		MyMessageBox_Error(_T("MAC_Str2Bytes"));
		return FALSE;
	}

	int i = 0, j = 0, value;
	unsigned char str_tmp[6] = "\0", tmp = '\0';
	UCHAR *macstr = new UCHAR[50 * sizeof(UCHAR)];
	strcpy((CHAR *) macstr, strMACTokenWiped.GetBuffer(0));
	
	for (i = 0; i < 6; i ++)
	{
		memset(str_tmp, 0, sizeof(str_tmp));
		memcpy(str_tmp, macstr, 2 * sizeof(char));
		macstr += 2;
		pMac[i] = '\0';
		for (j = strlen((char *) str_tmp) - 1; j >= 0; j --)
		{
			switch (tolower(str_tmp[j]))
			{
			case '1' : value = 0x1; break;
			case '2' : value = 0x2; break;
			case '3' : value = 0x3; break;
			case '4' : value = 0x4; break;
			case '5' : value = 0x5; break; 
			case '6' : value = 0x6; break;
			case '7' : value = 0x7; break;
			case '8' : value = 0x8; break;
			case '9' : value = 0x9; break; 
			case 'a' : value = 0xa; break;
			case 'b' : value = 0xb; break;
			case 'c' : value = 0xc; break;
			case 'd' : value = 0xd; break;
			case 'e' : value = 0xe; break;
			case 'f' : value = 0xf; break;
			case '0' : value = 0x0; break;
			default :
				{
					//printf("error str_tmp[j]=%c \n!",str_tmp[j]);
					MyMessageBox_Error(_T("MAC_Str2Bytes"));
					delete macstr;
					return FALSE;
				}
				
			}
			pMac[i] |= (UCHAR) (value << ((strlen((char *) str_tmp) - 1 - j) * 4));
		}
	}  
	return TRUE;
}

CString VNFilter::MAC_Bytes2Str(PUCHAR pMac)
{
	if (pMac == NULL)
	{
		MyMessageBox_Error(_T("MAC_Bytes2Str"));
		return _T("");
	}
	CString strMac;
	strMac.Format("%2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X",
		(unsigned char) pMac[0], (unsigned char) pMac[1], (unsigned char) pMac[2],
		(unsigned char) pMac[3], (unsigned char) pMac[4], (unsigned char) pMac[5]);
	return strMac;
}

void VNFilter::makeWrapper(CString strSrcMac, CString strDstMac, CString strSrcIP, CString strDstIP)
{
	initPhysicalNIC();
	Eth_Header *ethHeader = new Eth_Header();
	MAC_Str2Bytes(strSrcMac, ethHeader->eth_SrcAddr);
	MAC_Str2Bytes(strDstMac, ethHeader->eth_DstAddr);
	//ethHeader->eth_Type = 
}

USHORT VNFilter::checkSum(USHORT *pBuf, UINT nBufLen)
{
	ULONG sum;
	for (sum = 0; nBufLen > 0; nBufLen --)
		sum += *pBuf++;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ~sum;
}