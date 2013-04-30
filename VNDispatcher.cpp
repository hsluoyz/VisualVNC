#include "StdAfx.h"

#include "MainFrm.h"
#include "VNDispatcher.h"
#include "VNDriverLoaderEx.h"
#include "VNFilter.h"
//#include "DrvFltIp.h"
#include "CommonIO.h"
#include "MyMessageBox.h"

#include <WinIoCtl.h>

VNDispatcher::VNDispatcher()
{
	m_pLoader = NULL;
	m_pFilter = NULL;
	m_pCurCoreRule = NULL;
	m_pCurMappingRule = NULL;

	m_nModeCenter = -1;
	initDataDir();
	m_strRuleFile = _T("Rules2.txt");
	
	prepareDebugRules();
}

VNDispatcher::~VNDispatcher()
{

}

CString VNDispatcher::itos(int i)
{
	CString strTemp;
	strTemp.Format(_T("%d"), i);
	return strTemp;
}

int VNDispatcher::stoi(CString s)
{
	return atoi(s);
}

BOOL VNDispatcher::FolderExist(CString strPath)
{
	WIN32_FIND_DATA wfd;
	BOOL rValue = FALSE;
	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = TRUE;
	}
	FindClose(hFind);
	return rValue;
}

BOOL VNDispatcher::FileExist(CString strPathName)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(strPathName, &FindFileData);
	
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	else
	{
		FindClose(hFind);
		return true;
	}
	return false;
}

void VNDispatcher::initDataDir()
{
	CString cstrDir;
	TCHAR cBuffer[260];
	GetCurrentDirectory(MAX_PATH, cBuffer);
	CString cstrPath(cBuffer);
	if (FolderExist(cstrPath + _T("\\data")))
	{
		cstrDir = cstrPath;
	}
	else
	{
		int nCount = cstrPath.ReverseFind(_T('\\'));
		cstrPath.Delete(nCount, cstrPath.GetLength() - nCount);
		
		if (FolderExist(cstrPath + _T("\\data")))
		{
			cstrDir = cstrPath;
		}
		else
		{
			MyMessageBox_Error(_T("initDataDir"));
			return;
		}
	}
	cstrDir += _T("\\data\\");
	m_strDataDirectory = cstrDir;
}

void VNDispatcher::clearAllRules()
{
	for (int i = 0; i < m_pArrCoreRules.size(); i ++)
	{
		delete m_pArrCoreRules[i];
	}
	m_pArrCoreRules.clear();
	m_pCurCoreRule = NULL;
	m_pCurMappingRule = NULL;
}

void VNDispatcher::readAndParseRuleFile()
{
	clearAllRules();

	CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CString strRuleFile = pMainWnd->m_pEditRuleFile->GetEditText();

	CString strRulePathName = m_strDataDirectory + strRuleFile; //m_strRuleFile;
	if (!FileExist(strRulePathName))
	{
		MyMessageBox_Error(_T("readAndParseRuleFile"));
		return;
	}
	
	CStdioFile pRuleFile;
	pRuleFile.Open(strRulePathName, CFile::modeRead | CFile::typeText);
	
	CString strLine;
	m_strLines = _T("");
	while (pRuleFile.ReadString(strLine))
	{
		m_strLines += strLine;
		m_strLines += _T("\r\n");
	}
	m_pCurCoreRule = parseCoreRule(m_strLines);
	m_pArrCoreRules.push_back(m_pCurCoreRule);
	
	displayWholeFile();
	pRuleFile.Close();
}

// 	[Core Rule]
// 	{
// 		[CIP]: 192.168.1.232
// 			
// 			[Mapping Rule]
// 		{
// 			[VIP]: 192.168.2.123, 255.255.255.0, 192.168.2.1
// 				[PIP]: 192.168.1.234
// 				[IMAC]: 12-34-56-78-9A-BC
// 		}
// 	}
VNCoreRule* VNDispatcher::parseCoreRule(CString strCoreRule)
{
	int iPos = 0;
	int iStart = 0;
	int iEnd = 0;
	int i;
	VNCoreRule *pCoreRule;

	CString strCoreRuleContent = parseBracketContent(strCoreRule, _T("[Core Rule]"));

	iStart = strCoreRuleContent.Find(_T("[CIP]"));
	if (iStart == -1)
	{
		MyMessageBox_Error(_T("parseCoreRule"));
		return NULL;
	}
	iEnd = strCoreRuleContent.Find(_T('['), iStart + 5);
	if (iEnd == -1)
	{
		MyMessageBox_Error(_T("parseCoreRule"));
		return NULL;
	}
	iEnd --;
	CString strCIP = strCoreRuleContent.Mid(iStart, iEnd - iStart + 1); //[CIP]: 192.168.1.232
	strCIP = parseColonContent(strCIP, _T("[CIP]")); // 192.168.1.232
	refineElement(strCIP); //192.168.1.232
	pCoreRule = new VNCoreRule(strCIP);

	vector<int> iArrPoses;
	iPos = iEnd;
	do 
	{
		iPos = strCoreRuleContent.Find(_T("[Mapping Rule]"), iPos + 1);
		if (iPos != -1)
		{
			iArrPoses.push_back(iPos);
		}
		else
		{
			break;
		}
	}
	while (iPos < strCoreRuleContent.GetLength());

	for (i = 0; i < iArrPoses.size(); i ++)
	{
		CString strMappingRule;
		if (i != iArrPoses.size() - 1)
		{
			strMappingRule = strCoreRuleContent.Mid(iArrPoses[i], iArrPoses[i + 1]);
		}
		else
		{
			strMappingRule = strCoreRuleContent.Mid(iArrPoses[i], strCoreRuleContent.GetLength());
		}
		VNMappingRule *pMappingRule = parseMappingRule(strMappingRule);
		pCoreRule->addMappingRule(pMappingRule);
	}

	return pCoreRule;
}

// 	[Mapping Rule]
// 	{
// 		[VIP]: 192.168.2.123, 255.255.255.0, 192.168.2.1
// 		[PIP]: 192.168.1.234
// 		[IMAC]: 12-34-56-78-9A-BC
// 	}
VNMappingRule* VNDispatcher::parseMappingRule(CString strMappingRule)
{
	int iPos = 0;
	int iStart = 0;
	int iEnd = 0;
	VNMappingRule *pMappingRule;
	
//		[VIP]: 192.168.2.123, 255.255.255.0, 192.168.2.1
// 		[PIP]: 192.168.1.234
// 		[IMAC]: 12-34-56-78-9A-BC
	CString strMappingRuleContent = parseBracketContent(strMappingRule, _T("[Mapping Rule]"));

	iStart = strMappingRuleContent.Find(_T("[VIP]"));
	if (iStart == -1)
	{
		MyMessageBox_Error(_T("parseMappingRule"));
		return NULL;
	}
	iEnd = strMappingRuleContent.Find(_T('['), iStart + 5);
	if (iEnd == -1)
	{
		MyMessageBox_Error(_T("parseCoreRule"));
		return NULL;
	}
	iEnd --;
	//[VIP]: 192.168.2.123, 255.255.255.0, 192.168.2.1
	CString strVIP = strMappingRuleContent.Mid(iStart, iEnd - iStart + 1);
	// 192.168.2.123, 255.255.255.0, 192.168.2.1
	strVIP = parseColonContent(strVIP, _T("[VIP]")); 
	//192.168.2.123, 255.255.255.0, 192.168.2.1
	refineElement(strVIP);
	//192.168.2.123
	CString strVIP_IP;
	//255.255.255.0
	CString strVIP_Mask;
	//192.168.2.1
	CString strVIP_Gate;
	parseCommaContent(strVIP, strVIP_IP, strVIP_Mask, strVIP_Gate);

	///////////////////////////////////////////////////////////////////////////////////
	iStart = strMappingRuleContent.Find(_T("[PIP]"), iEnd + 1);
	if (iStart == -1)
	{
		MyMessageBox_Error(_T("parseMappingRule"));
		return NULL;
	}
	iEnd = strMappingRuleContent.Find(_T('['), iStart + 5);
	if (iEnd == -1)
	{
		MyMessageBox_Error(_T("parseCoreRule"));
		return NULL;
	}
	iEnd --;
	//[PIP]: 192.168.1.234
	CString strPIP = strMappingRuleContent.Mid(iStart, iEnd - iStart + 1);
	// 192.168.1.234
	strPIP = parseColonContent(strPIP, _T("[PIP]"));
	//192.168.1.234
	refineElement(strPIP);

	///////////////////////////////////////////////////////////////////////////////////
	iStart = strMappingRuleContent.Find(_T("[IMAC]"), iEnd + 1);
	if (iStart == -1)
	{
		MyMessageBox_Error(_T("parseMappingRule"));
		return NULL;
	}
	iEnd = strMappingRuleContent.GetLength() - 1;

	//[IMAC]: 12-34-56-78-9A-BC
	CString strIMAC = strMappingRuleContent.Mid(iStart, iEnd - iStart + 1);
	// 12-34-56-78-9A-BC
	strIMAC = parseColonContent(strIMAC, _T("[IMAC]"));
	//12-34-56-78-9A-BC
	refineElement(strIMAC);

	pMappingRule = new VNMappingRule(strVIP_IP, strVIP_Mask, strVIP_Gate, strPIP, strIMAC);
	return pMappingRule;
}

CString VNDispatcher::parseBracketContent(CString strText, CString strTag)
{
	int iPos = 0;
	int iStart = 0;
	int iEnd = 0;
	int iBracketCnt = 0;
	CString strResult;

	iPos = strText.Find(strTag); //strTag = "[Core Rule]"
	if (iPos == -1) goto Label1;

	iStart = strText.Find(_T("{"), iPos + 1);
	if (iStart == -1) goto Label1;
	iStart ++;
	iEnd = iStart;
	iBracketCnt ++;

	while (iEnd < strText.GetLength())
	{
		TCHAR ch = strText.GetAt(iEnd);
		if (ch == _T('{'))
		{
			iBracketCnt ++;
		}
		else if (ch == _T('}'))
		{
			iBracketCnt --;
		}
		
		if (iBracketCnt == 0)
		{
			iEnd --;
			break;
		}
		iEnd ++;
	}
	if (iEnd == iStart)
	{
		goto Label1;
	}

	strResult = strText.Mid(iStart, iEnd - iStart + 1);
	return strResult;

Label1:
	MyMessageBox_Error(_T("parseBracketContent"));
	return _T("");
}

CString VNDispatcher::parseColonContent(CString strText, CString strTag) //[CIP]: 192.168.1.232
{
	int iPos = 0;
	int iStart = 0;
	int iEnd = 0;
	CString strResult;
	
	iPos = strText.Find(strTag); //strTag = "[CIP]"
	if (iPos == -1) goto Label2;
	
	iStart = strText.Find(_T(":"), iPos + 1);
	if (iStart == -1) goto Label2;
	iStart ++;
	iEnd = strText.GetLength() - 1;
	while (strText.GetAt(iStart) == _T(' ') || strText.GetAt(iStart) == _T('\t') || strText.GetAt(iStart) == _T('\n') || strText.GetAt(iStart) == _T('\r'))
	{
		iStart ++;
	}
	while (strText.GetAt(iEnd) == _T(' ') || strText.GetAt(iEnd) == _T('\t') || strText.GetAt(iEnd) == _T('\n') || strText.GetAt(iEnd) == _T('\r'))
	{
		iEnd --;
	}

	strResult = strText.Mid(iStart, iEnd - iStart + 1);
	return strResult;

Label2:
	MyMessageBox_Error(_T("parseColonContent"));
	return _T("");
}

BOOL VNDispatcher::parseCommaContent(CString strText, CString &strContent1, CString &strContent2, CString &strContent3)
{
	//strText = "192.168.2.123, 255.255.255.0, 192.168.2.1"
	int iPos = 0;
	int iStart = 0;
	int iEnd = 0;
	vector<int> iArrPoses;
	for (int i = 0; i < strText.GetLength(); i ++)
	{
		if (strText.GetAt(i) == _T(','))
			iArrPoses.push_back(i);
	}
	if (iArrPoses.size() != 2)
	{
		goto Label3;
	}

	iStart = 0;
	iEnd = iArrPoses[0] - 1;
	strContent1 = strText.Mid(iStart, iEnd - iStart + 1);
	refineElement(strContent1);

	iStart = iArrPoses[0] + 1;
	iEnd = iArrPoses[1] - 1;
	strContent2 = strText.Mid(iStart, iEnd - iStart + 1);
	refineElement(strContent2);

	iStart = iArrPoses[1] + 1;
	iEnd = strText.GetLength() - 1;
	strContent3 = strText.Mid(iStart, iEnd - iStart + 1);
	refineElement(strContent3);

	return TRUE;

Label3:
	MyMessageBox_Error(_T("parseCommaContent"));
	return FALSE;
}

void VNDispatcher::refineElement(CString &strText)
{
	int iStart = 0;
	int iEnd = strText.GetLength() - 1;

	while (strText.GetAt(iStart) == _T(' ') || strText.GetAt(iStart) == _T('\t') || strText.GetAt(iStart) == _T('\n') || strText.GetAt(iStart) == _T('\r'))
	{
		iStart ++;
	}
	while (strText.GetAt(iEnd) == _T(' ') || strText.GetAt(iEnd) == _T('\t') || strText.GetAt(iEnd) == _T('\n') || strText.GetAt(iEnd) == _T('\r'))
	{
		iEnd --;
	}

	strText = strText.Mid(iStart, iEnd - iStart + 1);
}

void VNDispatcher::displayWholeFile()
{
	Clear_And_Output_Policy_EditBox(m_strLines);
}

CString VNDispatcher::getWholeFile()
{
	return m_strLines;
}

void VNDispatcher::displayRules()
{
	for (int i = 0; i < m_pArrCoreRules.size(); i ++)
	{
		CString strTemp;
		strTemp.Format(_T("Core Rule: CIP=%s"), m_pArrCoreRules[i]->cIP);
		Output_Console_List(itos(i), strTemp);

		vector<VNMappingRule*> pArrMappingRules = m_pArrCoreRules[i]->pArrMappingRules;
		for (int j = 0; j < pArrMappingRules.size(); j ++)
		{
			strTemp.Format(_T("\tMapping Rule: VIP=%s/%s/%s, PIP=%s, IMAC=%s"), pArrMappingRules[j]->vIP, pArrMappingRules[j]->vIP_Mask,
				pArrMappingRules[j]->vIP_Gate, pArrMappingRules[j]->pIP, pArrMappingRules[j]->iMAC);
			Output_Console_List(itos(i) + _T("-") + itos(j), strTemp);
		}
	}
}

void VNDispatcher::prepareDebugRules()
{
	VNDebugRule *pDebugRule;
	//测试生成
	//测试发送功能时由192.168.1.234发送即可
	//测试接收功能时由192.168.1.235发送，192.168.1.234接收。


	//////////////////////////////////////测试Margin主机功能//////////////////////////////////////
	//Margin角色:
	//Win2003ForDDK
	//192.168.1.234 （主网卡）
	//00-0C-29-3F-C4-77
	//192.168.1.236 （Loopback网卡1）
	//00-0C-29-3F-C4-81
	//192.168.1.237 （Loopback网卡2）
	//00-0C-29-3F-C4-8B

	//与Margin相连的角色:
	//Win2003ForDDK2
	//192.168.1.235 （主网卡）
	//00-0C-29-03-C5-E7

	//Center角色:
	//Akisn0w-PC
	//192.168.1.232
	//78-45-C4-1E-A3-A4

	//PIP = 192.168.1.234
	//VIP = 192.168.2.128
	//CIP = 192.168.1.232
	//EMAC = 00-0C-29-3F-C4-77
	//CMAC = 78-45-C4-1E-A3-A4
	//IMAC = N/A

	//(0)
	//192.168.1.234来触发，Margin主机发送数据包，数据包无限制条件
	//此数据包应该进行以下VEP封装后发送：
	//目的MAC: 78-45-C4-1E-A3-A4 (CMAC)
	//源MAC: 00-0C-29-3F-C4-77 (EMAC)
	//目的IP: 192.168.1.232 (CIP)
	//源IP: 192.168.1.234 (PIP)
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_NONE;
	pDebugRule->eDstMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eSrcMAC = _T("00-0C-29-3F-C4-77");
	pDebugRule->eDstIP = _T("192.168.1.235");
	pDebugRule->eSrcIP = _T("192.168.1.234");
	m_pArrDebugRules.push_back(pDebugRule);

	//(1)
	//192.168.1.235来触发，Margin主机接收数据包，A情况：ARP Request报文
	//仅可能是ARP请求，因为Margin是不会主动进行ARP请求的，所以也不会收到ARP应答
	//此数据包应该由驱动直接进行应答，然后原数据包丢弃
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_ARP_REQUEST;
	pDebugRule->iType = TYPE_NONE;
	//pDebugRule->eDstMAC = _T("12-34-56-78-9A-BC");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.234");
	pDebugRule->eSrcIP = _T("192.168.1.235");
	m_pArrDebugRules.push_back(pDebugRule);

	//(2)
	//192.168.1.235来触发，Margin主机接收数据包，B情况：VEP报文，且目的IP为192.168.1.234 (PIP)，内部为IP报文
	//此数据包应该解封装后，修改目的MAC为EMAC，接收数据包
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_IP;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-77");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.234");
	pDebugRule->eSrcIP = _T("192.168.1.235");
	pDebugRule->iDstMAC = _T("12-34-56-78-9A-BC"); //此处被修改为00-0C-29-3F-C4-77 (EMAC)
	pDebugRule->iSrcMAC = _T("78-9A-BC-DE-F0-12");
	pDebugRule->iDstIP = _T("192.168.2.128");
	pDebugRule->iSrcIP = _T("4.4.4.4");
	m_pArrDebugRules.push_back(pDebugRule);

	//(3)
	//192.168.1.235来触发，Margin主机接收数据包，B情况：VEP报文，且目的IP为192.168.1.234 (PIP)，内部为ARP Request报文
	//此数据包应该解封装后，接收数据包
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_ARP_REQUEST;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-77");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.234");
	pDebugRule->eSrcIP = _T("192.168.1.235");
	//pDebugRule->iDstMAC = _T("12-34-56-78-9A-BC");
	pDebugRule->iSrcMAC = _T("78-9A-BC-DE-F0-12");
	pDebugRule->iDstIP = _T("192.168.2.128");
	pDebugRule->iSrcIP = _T("4.4.4.4");
	m_pArrDebugRules.push_back(pDebugRule);

	//(4)
	//192.168.1.235来触发，Margin主机接收数据包，B情况：VEP报文，且目的IP为192.168.1.234 (PIP)，内部为ARP Reply报文
	//此数据包应该解封装后，修改两处目的MAC为EMAC，接收数据包
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_ARP_REPLY;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-77");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.234");
	pDebugRule->eSrcIP = _T("192.168.1.235");
	pDebugRule->iDstMAC = _T("12-34-56-78-9A-BC"); //此处（实际上是两处）被修改为00-0C-29-3F-C4-77 (EMAC)
	pDebugRule->iSrcMAC = _T("78-9A-BC-DE-F0-12");
	pDebugRule->iDstIP = _T("192.168.2.128");
	pDebugRule->iSrcIP = _T("4.4.4.4");
	m_pArrDebugRules.push_back(pDebugRule);

	//(5)
	//192.168.1.235来触发，Margin主机接收数据包，B情况：VEP报文，且目的IP不为192.168.1.234 (PIP)
	//一般收不到这种包，但是可以用Loopback实现
	//此数据包应该丢弃
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_IP;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-81");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.236"); //假设不是发给Margin主机的包，只是随便地通过Margin主机的Loopback网卡收到而已
	pDebugRule->eSrcIP = _T("192.168.1.235");
	pDebugRule->iDstMAC = _T("12-34-56-78-9A-BC");
	pDebugRule->iSrcMAC = _T("78-9A-BC-DE-F0-12");
	pDebugRule->iDstIP = _T("192.168.2.128");
	pDebugRule->iSrcIP = _T("4.4.4.4");
	m_pArrDebugRules.push_back(pDebugRule);

	//(6)
	//192.168.1.235来触发，Margin主机接收数据包，C情况：普通的IP报文
	//此数据包应该丢弃
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_NONE;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-77");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.234");
	pDebugRule->eSrcIP = _T("192.168.1.235");
	m_pArrDebugRules.push_back(pDebugRule);

	//////////////////////////////////////测试Center主机功能//////////////////////////////////////
	//Center角色:
	//Win2003ForDDK
	//192.168.1.234 （主网卡）
	//00-0C-29-3F-C4-77
	//192.168.1.236 （Loopback网卡1）
	//00-0C-29-3F-C4-81
	//192.168.1.237 （Loopback网卡2）
	//00-0C-29-3F-C4-8B
	
	//Margin角色:
	//Win2003ForDDK2
	//192.168.1.235 （主网卡）
	//00-0C-29-03-C5-E7

	//没角色:
	//Akisn0w-PC
	//192.168.1.232
	//78-45-C4-1E-A3-A4
	
	//PIP = 192.168.1.235
	//VIP = 192.168.1.236
	//CIP = 192.168.1.234
	//EMAC = 00-0C-29-03-C5-E7
	//CMAC = 00-0C-29-3F-C4-77
	//IMAC = 00-0C-29-3F-C4-81

	//(7)
	//192.168.1.234来触发，Center主机发送数据包，数据包无限制条件
	//此数据包应该允许通过
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_NONE;
	pDebugRule->eDstMAC = _T("78-45-C4-1E-A3-A4");
	pDebugRule->eSrcMAC = _T("00-0C-29-3F-C4-77");
	pDebugRule->eDstIP = _T("192.168.1.232");
	pDebugRule->eSrcIP = _T("192.168.1.234");
	m_pArrDebugRules.push_back(pDebugRule);

	//(8)
	//192.168.1.235来触发，Center主机接收数据包，A情况：接收网卡为00-0C-29-3F-C4-81 (IMAC)网卡
	//此数据包应该进行以下VEP封装后发送：
	//目的MAC: 00-0C-29-03-C5-E7 (EMAC)
	//源MAC: 00-0C-29-3F-C4-77 (CMAC)
	//目的IP: 192.168.1.235 (PIP)
	//源IP: 192.168.1.234 (CIP)
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_NONE;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-81"); //IMAC
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.236"); //发给接收网卡就是发给对应IP
	pDebugRule->eSrcIP = _T("192.168.1.235"); //按说不应该由Margin主机发这个包，不过测试就无所谓了
	m_pArrDebugRules.push_back(pDebugRule);

	//(9)
	//192.168.1.235来触发，Center主机接收数据包，B情况：接收网卡为00-0C-29-3F-C4-77 (CMAC)网卡，且IP报文，为VEP封装，
	//外层源IP对应到某一PIP，如192.168.1.235，解封装后，内部为IP报文
	//此数据包应该修改源MAC为00-0C-29-3F-C4-81 (IMAC)，从对应的IMAC网卡转发数据包
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_IP;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-77");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.234");
	pDebugRule->eSrcIP = _T("192.168.1.235");
	pDebugRule->iDstMAC = _T("56-78-9A-BC-DE-F0");
	pDebugRule->iSrcMAC = _T("78-9A-BC-DE-F0-12"); //此处被修改为00-0C-29-3F-C4-81 (IMAC)
	pDebugRule->iDstIP = _T("3.3.3.3");
	pDebugRule->iSrcIP = _T("4.4.4.4");
	m_pArrDebugRules.push_back(pDebugRule);

	//(10)
	//192.168.1.235来触发，Center主机接收数据包，B情况：接收网卡为00-0C-29-3F-C4-77 (CMAC)网卡，且IP报文，为VEP封装，
	//外层源IP对应到某一PIP，如192.168.1.235，解封装后，内部为ARP Request报文
	//此数据包应该修改两处源MAC为00-0C-29-3F-C4-81 (IMAC)，从对应的IMAC网卡转发数据包
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_ARP_REQUEST;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-77");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.234");
	pDebugRule->eSrcIP = _T("192.168.1.235");
	//pDebugRule->iDstMAC = _T("56-78-9A-BC-DE-F0");
	pDebugRule->iSrcMAC = _T("78-9A-BC-DE-F0-12"); //此处（实际上是两处）被修改为00-0C-29-3F-C4-81 (IMAC)
	pDebugRule->iDstIP = _T("3.3.3.3");
	pDebugRule->iSrcIP = _T("4.4.4.4");
	m_pArrDebugRules.push_back(pDebugRule);

	//(11)
	//192.168.1.235来触发，Center主机接收数据包，B情况：接收网卡为00-0C-29-3F-C4-77 (CMAC)网卡，且IP报文，为VEP封装，
	//外层源IP对应到某一PIP，如192.168.1.235，解封装后，内部为ARP Reply报文
	//此数据包应该修改两处源MAC为00-0C-29-3F-C4-81 (IMAC)，从对应的IMAC网卡转发数据包
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_ARP_REPLY;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-77");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.234");
	pDebugRule->eSrcIP = _T("192.168.1.235");
	pDebugRule->iDstMAC = _T("56-78-9A-BC-DE-F0");
	pDebugRule->iSrcMAC = _T("78-9A-BC-DE-F0-12"); //此处（实际上是两处）被修改为00-0C-29-3F-C4-81 (IMAC)
	pDebugRule->iDstIP = _T("3.3.3.3");
	pDebugRule->iSrcIP = _T("4.4.4.4");
	m_pArrDebugRules.push_back(pDebugRule);

	//(12)
	//192.168.1.235来触发，Center主机接收数据包，B情况：接收网卡为00-0C-29-3F-C4-77 (CMAC)网卡，且IP报文，为VEP封装，
	//外层源IP不对应对应到任何PIP，如192.168.1.235
	//此数据包应该丢弃
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_IP;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-77");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.234");
	pDebugRule->eSrcIP = _T("192.168.1.238"); //不对应192.168.1.235 (PIP)
	pDebugRule->iDstMAC = _T("56-78-9A-BC-DE-F0");
	pDebugRule->iSrcMAC = _T("78-9A-BC-DE-F0-12");
	pDebugRule->iDstIP = _T("3.3.3.3");
	pDebugRule->iSrcIP = _T("4.4.4.4");
	m_pArrDebugRules.push_back(pDebugRule);

	//(13)
	//192.168.1.235来触发，Center主机接收数据包，B情况：接收网卡为00-0C-29-3F-C4-77 (CMAC)网卡，且IP报文，但不是VEP封装
	//此数据包应该允许通过
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_NONE;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-77");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7");
	pDebugRule->eDstIP = _T("192.168.1.234");
	pDebugRule->eSrcIP = _T("192.168.1.235");
	m_pArrDebugRules.push_back(pDebugRule);

	//(14)
	//192.168.1.235来触发，Center主机接收数据包，C情况：接收网卡为00-0C-29-3F-C4-8B，既不属于00-0C-29-3F-C4-81 (IMAC)网卡，
	//也不属于00-0C-29-3F-C4-77 (CMAC)网卡
	//此数据包应该允许通过
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_NONE;
	pDebugRule->eDstMAC = _T("00-0C-29-3F-C4-8B");
	pDebugRule->eSrcMAC = _T("00-0C-29-03-C5-E7"); //写个EMAC无所谓
	pDebugRule->eDstIP = _T("192.168.1.237"); //Lookback2
	pDebugRule->eSrcIP = _T("192.168.1.235");
	m_pArrDebugRules.push_back(pDebugRule);

	/*
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = TYPE_IP;
	pDebugRule->iType = TYPE_IP;
	pDebugRule->eDstMAC = _T("12-34-56-78-9A-BC");
	pDebugRule->eSrcMAC = _T("34-56-78-9A-BC-DE");
	pDebugRule->eDstIP = _T("1.1.1.1");
	pDebugRule->eSrcIP = _T("2.2.2.2");
	pDebugRule->iDstMAC = _T("56-78-9A-BC-DE-F0");
	pDebugRule->iSrcMAC = _T("78-9A-BC-DE-F0-12");
	pDebugRule->iDstIP = _T("3.3.3.3");
	pDebugRule->iSrcIP = _T("4.4.4.4");
	m_pArrDebugRules.push_back(pDebugRule);
	*/
}

int VNDispatcher::getDebugRuleCount()
{
	return m_pArrDebugRules.size();
}

void VNDispatcher::updateDebugButtons()
{
	CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	//pMainWnd->m_pPltDisplayDebug->Remove
}

void VNDispatcher::displayDebugRule(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_pArrDebugRules.size())
	{
		MyMessageBox_Error(_T("displayDebugRule"));
		return;
	}
	else
	{
		displayDebugRule(m_pArrDebugRules[nIndex]);
	}
}

void VNDispatcher::displayDebugRule(VNDebugRule *pDebugRule)
{
	CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CString strEType;
	CString strIType;

	switch (pDebugRule->eType)
	{
	case TYPE_IP:
		strEType = _T("IP");
		break;
	case TYPE_ARP_REQUEST:
		strEType = _T("ARP Request");
		break;
	case TYPE_ARP_REPLY:
		strEType = _T("ARP Reply");
		break;
	default:
		MyMessageBox_Error(_T("displayDebugRule"));
		return;
	}

	switch (pDebugRule->iType)
	{
	case TYPE_IP:
		strIType = _T("IP");
		break;
	case TYPE_ARP_REQUEST:
		strIType = _T("ARP Request");
		break;
	case TYPE_ARP_REPLY:
		strIType = _T("ARP Reply");
		break;
	case TYPE_NONE:
		strIType = _T("None");
		break;
	default:
		MyMessageBox_Error(_T("displayDebugRule"));
		return;
	}

	pMainWnd->m_pComboEType->SelectItem(strEType);
	pMainWnd->m_pComboIType->SelectItem(strIType);
	pMainWnd->m_pEditEDstMAC->SetEditText(pDebugRule->eDstMAC);
	pMainWnd->m_pEditESrcMAC->SetEditText(pDebugRule->eSrcMAC);
	pMainWnd->m_pEditEDstIP->SetEditText(pDebugRule->eDstIP);
	pMainWnd->m_pEditESrcIP->SetEditText(pDebugRule->eSrcIP);
	pMainWnd->m_pEditIDstMAC->SetEditText(pDebugRule->iDstMAC);
	pMainWnd->m_pEditISrcMAC->SetEditText(pDebugRule->iSrcMAC);
	pMainWnd->m_pEditIDstIP->SetEditText(pDebugRule->iDstIP);
	pMainWnd->m_pEditISrcIP->SetEditText(pDebugRule->iSrcIP);
}

void VNDispatcher::writeDebugRule(int eType, int iType, CString eDstMAC, CString eSrcMAC, CString eDstIP, CString eSrcIP,
								   CString iDstMAC, CString iSrcMAC, CString iDstIP, CString iSrcIP)
{
	VNDebugRule *pDebugRule;
	
	pDebugRule = new VNDebugRule();
	pDebugRule->eType = eType;
	pDebugRule->iType = iType;
	pDebugRule->eDstMAC = eDstMAC;
	pDebugRule->eSrcMAC = eSrcMAC;
	pDebugRule->eDstIP = eDstIP;
	pDebugRule->eSrcIP = eSrcIP;
	pDebugRule->iDstMAC = iDstMAC;
	pDebugRule->iSrcMAC = iSrcMAC;
	pDebugRule->iDstIP = iDstIP;
	pDebugRule->iSrcIP = iSrcIP;
	writeDebugRule(pDebugRule);
}

void VNDispatcher::setLoader(VNDriverLoaderEx *pLoader)
{
	m_pLoader = pLoader;
}

void VNDispatcher::setFilter(VNFilter *pFilter)
{
	m_pFilter = pFilter;
}

BOOL VNDispatcher::autoSelectModeAndRule(int &nMappingRuleIndex) //首先必须确定m_pCurCoreRule
{
	if (!m_pFilter)
	{
		MyMessageBox_Error(_T("autoSelectModeAndRule"));
		return FALSE;
	}
	m_pFilter->initPhysicalNIC();

	if (!m_pCurCoreRule)
	{
		MyMessageBox_Error(_T("autoSelectModeAndRule"));
		return FALSE;
	}

	for (int i = 0;  i< m_pFilter->m_nPhysicalNICs.size(); i ++)
	{
		if (m_pCurCoreRule->cIP == m_pFilter->m_nPhysicalNICs[i]->ip)
		{
			m_nModeCenter = 1; ////////////////////////
			nMappingRuleIndex = -1;
			m_pCurMappingRule = NULL;

			m_pFilter->m_pPhysicalNIC = m_pFilter->m_nPhysicalNICs[i];

			return TRUE;
		}
	}

	vector<VNMappingRule*> pMappingRules = m_pCurCoreRule->pArrMappingRules;
	for (i = 0; i < pMappingRules.size(); i ++)
	{
		if (pMappingRules[i]->pIP == m_pFilter->m_pPhysicalNIC->ip)
		{
			m_nModeCenter = 0; ////////////////////////
			nMappingRuleIndex = i;
			m_pCurMappingRule = pMappingRules[i];
			m_pCurMappingRule->pIP_Mask = m_pFilter->m_pPhysicalNIC->mask;
			m_pCurMappingRule->pIP_Gate = m_pFilter->m_pPhysicalNIC->gate;
			return TRUE;
		}
	}

	m_nModeCenter = -1; ////////////////////////
	nMappingRuleIndex = -1;
	m_pCurMappingRule = NULL;
	//MyMessageBox_Error(_T("无法确定本主机的角色！"), _T("Run Info"));
	return FALSE;
}

UCHAR* VNDispatcher::genCoreRule(VNCoreRule *pCoreRule, int &nRetDataLen)
{
	nRetDataLen = 0;
	if (!m_pFilter)
	{
		MyMessageBox_Error(_T("genCoreRule"));
		return NULL;
	}

	if (pCoreRule == NULL)
	{
		MyMessageBox_Error(_T("genCoreRule"));
		return NULL;
	}

	UCHAR *pData = (UCHAR*) malloc(CORE_RULE_LEN * sizeof(UCHAR));
	memset(pData, 0, CORE_RULE_LEN * sizeof(UCHAR));
	////////////////////////////////////////////////////////////////
	CString strCIP = pCoreRule->cIP;
	CString strCMAC = m_pFilter->getTargetMacOrGateMac(strCIP);

	//UCHAR		cMAC[6]; //Center MAC Address
	//UCHAR										reserved1[2]; //Reserved for alignment
	//ULONG		cIP; //Center IP Address

	VNFilter::MAC_Str2Bytes(strCMAC, pData); //Center MAC Address
	*((UINT*) (pData + 8)) = VNFilter::IP_Str2Int_Net(strCIP); //Center IP Address

	nRetDataLen = CORE_RULE_LEN;
	return pData;
}

UCHAR* VNDispatcher::genMappingRule(VNMappingRule *pMappingRule, int &nRetDataLen)
{
	nRetDataLen = 0;
	if (!m_pFilter)
	{
		MyMessageBox_Error(_T("genMappingRule"));
		return NULL;
	}
	
	if (pMappingRule == NULL)
	{
		MyMessageBox_Error(_T("genMappingRule"));
		return NULL;
	}

	UCHAR *pData = (UCHAR*) malloc(MAPPING_RULE_LEN * sizeof(UCHAR));
	memset(pData, 0, MAPPING_RULE_LEN * sizeof(UCHAR));
	////////////////////////////////////////////////////////////////
	CString strVIP = pMappingRule->vIP;
	CString strPIP = pMappingRule->pIP;
	CString strEMAC = m_pFilter->getTargetMacOrGateMac(strPIP);
	CString strIMAC = pMappingRule->iMAC;

	//UCHAR		eMAC[6]; //External MAC Address
	//UCHAR										reserved1[2]; //Reserved for alignment
	//ULONG		vIP; //Virtual IP Address
	//UCHAR		iMAC[6]; //Internal MAC Address
	//UCHAR										reserved2[2]; //Reserved for alignment
	//ULONG		pIP; //Physical IP Address
	
	VNFilter::MAC_Str2Bytes(strEMAC, pData); //External MAC Address
	*((UINT*) (pData + 8)) = VNFilter::IP_Str2Int_Net(strVIP); //Virtual IP Address
	VNFilter::MAC_Str2Bytes(strIMAC, pData + 12); //External MAC Address
	*((UINT*) (pData + 20)) = VNFilter::IP_Str2Int_Net(strPIP); //Physical IP Address
	
	nRetDataLen = MAPPING_RULE_LEN;

	return pData;
}

UCHAR* VNDispatcher::genDebugRule(VNDebugRule *pDebugRule, int &nRetDataLen)
{
	nRetDataLen = 0;
	//if (!m_pFilter)
	//{
	//	MyMessageBox_Error(_T("genDebugRule"));
	//	return NULL;
	//}
	
	if (pDebugRule == NULL)
	{
		MyMessageBox_Error(_T("genDebugRule"));
		return NULL;
	}
	
	UCHAR *pData = (UCHAR*) malloc(DEBUG_RULE_LEN * sizeof(UCHAR));
	memset(pData, 0, DEBUG_RULE_LEN * sizeof(UCHAR));
	////////////////////////////////////////////////////////////////
// 	ULONG		eType; //TYPE_IP | TYPE_ARP_REQUEST | TYPE_ARP_REPLY
// 	ULONG		iType; //TYPE_IP | TYPE_ARP_REQUEST | TYPE_ARP_REPLY | TYPE_NONE
// 	UCHAR		eDstMAC[6];
// 	UCHAR		eSrcMAC[6];
// 	ULONG		eDstIP;
// 	ULONG		eSrcIP;
// 	UCHAR		iDstMAC[6];
// 	UCHAR		iSrcMAC[6];
// 	ULONG		iDstIP;
// 	ULONG		iSrcIP;
// 	UCHAR		content[20];

	*((UINT*) pData) = pDebugRule->eType;
	*((UINT*) (pData + 4)) = pDebugRule->iType;

	VNFilter::MAC_Str2Bytes(pDebugRule->eDstMAC, pData + 8);
	VNFilter::MAC_Str2Bytes(pDebugRule->eSrcMAC, pData + 14);
	*((UINT*) (pData + 20)) = VNFilter::IP_Str2Int_Net(pDebugRule->eDstIP);
	*((UINT*) (pData + 24)) = VNFilter::IP_Str2Int_Net(pDebugRule->eSrcIP);

	VNFilter::MAC_Str2Bytes(pDebugRule->iDstMAC, pData + 28);
	VNFilter::MAC_Str2Bytes(pDebugRule->iSrcMAC, pData + 34);
	*((UINT*) (pData + 40)) = VNFilter::IP_Str2Int_Net(pDebugRule->iDstIP);
	*((UINT*) (pData + 44)) = VNFilter::IP_Str2Int_Net(pDebugRule->iSrcIP);

	memmove(pData + 48, pDebugRule->content, 30);
	
	nRetDataLen = DEBUG_RULE_LEN;
	return pData;
}

void VNDispatcher::freeCoreRule(UCHAR *pData)
{
	free(pData);
}

void VNDispatcher::freeMappingRule(UCHAR *pData)
{
	free(pData);
}

void VNDispatcher::freeDebugRule(UCHAR *pData)
{
	free(pData);
}

BOOL VNDispatcher::writeMode()
{
	if (m_nModeCenter == -1)
	{
		MyMessageBox_Error(_T("changeMode"));
		return FALSE;
	}
	else
	{
		Driver_WriteMode(m_nModeCenter);
		return TRUE;
	}
}

void VNDispatcher::changeToVIP()
{
	if (m_nModeCenter != 0)
	{
		MyMessageBox_Error(_T("changeToVIP"));
	}
	else
	{
		CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		pMainWnd->OnBtnChangeToVIP();
	}
}

void VNDispatcher::changeToPIP()
{
	if (m_nModeCenter != 0)
	{
		MyMessageBox_Error(_T("changeToPIP"));
	}
	else
	{
		CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		pMainWnd->OnBtnChangeToPIP();
	}
}

void VNDispatcher::writeBothRules()
{
	if (!m_pCurCoreRule || m_nModeCenter == -1)
	{
		MyMessageBox_Error(_T("writeBothRules"));
		return;
	}
	else
	{
		Driver_ClearRules();
		Driver_WriteCoreRule(m_pCurCoreRule);
	}
	
	if (m_nModeCenter == 1)
	{
		vector<VNMappingRule*> pMapingRules = m_pCurCoreRule->pArrMappingRules;
		for (int i = 0; i < pMapingRules.size(); i ++)
		{
			Driver_WriteMappingRule(pMapingRules[i]);
		}
	}
	else if (m_nModeCenter == 0)
	{
		Driver_WriteMappingRule(m_pCurMappingRule);
	}
	else
	{
		MyMessageBox_Error(_T("writeBothRules"));
		return;
	}
}

void VNDispatcher::writeDebugRule(VNDebugRule *pDebugRule)
{
	Driver_WriteDebugRule(pDebugRule);
}

void VNDispatcher::startDriver()
{
	Driver_StartStop(TRUE);
}

void VNDispatcher::stopDriver()
{
	Driver_StartStop(FALSE);
}

BOOL VNDispatcher::Driver_WriteMode(BOOL bCenterOrMargin)
{
	if (!m_pLoader)
	{
		MyMessageBox_Error(_T("Driver_ChangeMode"));
		return FALSE;
	}
	
	int nDataIn;
	int nDataOut;
	int nRetDataLen;
	if (bCenterOrMargin)
	{
		nDataIn = MODE_CENTER;
	}
	else
	{
		nDataIn = MODE_MARGIN;
	}
	m_pLoader->writeAndReadFromNdis(IOCTL_VNCIO_CHANGE_MODE, (UCHAR*) &nDataIn, 4, (UCHAR*) &nDataOut, 4, nRetDataLen);
	
	if (nRetDataLen != 4)
	{
		MyMessageBox_Error(_T("Driver_ChangeMode"));
		return FALSE;
	}
	else
	{
		if (nDataOut != 1)
		{
			MyMessageBox_Error(_T("Driver_ChangeMode"));
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
}

BOOL VNDispatcher::Driver_StartStop(BOOL bStartOrStop)
{
	if (!m_pLoader)
	{
		MyMessageBox_Error(_T("Driver_StartStop"));
		return FALSE;
	}

	int nDataIn;
	int nDataOut;
	int nRetDataLen;
	if (bStartOrStop)
	{
		nDataIn = STATUS_START;
	}
	else
	{
		nDataIn = STATUS_STOP;
	}
	m_pLoader->writeAndReadFromNdis(IOCTL_VNCIO_START_STOP, (UCHAR*) &nDataIn, 4, (UCHAR*) &nDataOut, 4, nRetDataLen);

	if (nRetDataLen != 4)
	{
		MyMessageBox_Error(_T("Driver_StartStop"));
		return FALSE;
	}
	else
	{
		if (nDataOut != 1)
		{
			MyMessageBox_Error(_T("Driver_StartStop"));
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
}

BOOL VNDispatcher::Driver_WriteCoreRule(VNCoreRule *pCoreRule)
{
	if (!m_pLoader)
	{
		MyMessageBox_Error(_T("Driver_WriteCoreRule"));
		return FALSE;
	}
	
	int nDataInLen;
	UCHAR *pDataIn = genCoreRule(pCoreRule, nDataInLen);
	if (pDataIn == NULL)
	{
		return FALSE;
	}
	int nDataOut;
	int nRetDataLen;
	
	m_pLoader->writeAndReadFromNdis(IOCTL_VNCIO_WRITE_CORE_RULE, pDataIn, nDataInLen, (UCHAR*) &nDataOut, 4, nRetDataLen);

	freeCoreRule(pDataIn);
	if (nRetDataLen != 4)
	{
		MyMessageBox_Error(_T("Driver_WriteCoreRule"));
		return FALSE;
	}
	else
	{
		if (nDataOut != 1)
		{
			MyMessageBox_Error(_T("Driver_WriteCoreRule"));
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
}

BOOL VNDispatcher::Driver_WriteMappingRule(VNMappingRule *pMappingRule)
{
	if (!m_pLoader)
	{
		MyMessageBox_Error(_T("Driver_WriteMappingRule"));
		return FALSE;
	}
	
	int nDataInLen;
	UCHAR *pDataIn = genMappingRule(pMappingRule, nDataInLen);
	if (pDataIn == NULL)
	{
		return FALSE;
	}
	int nDataOut;
	int nRetDataLen;
	
	m_pLoader->writeAndReadFromNdis(IOCTL_VNCIO_WRITE_MAPPING_RULE, pDataIn, nDataInLen, (UCHAR*) &nDataOut, 4, nRetDataLen);
	
	freeMappingRule(pDataIn);
	if (nRetDataLen != 4)
	{
		MyMessageBox_Error(_T("Driver_WriteMappingRule"));
		return FALSE;
	}
	else
	{
		if (nDataOut != 1)
		{
			MyMessageBox_Error(_T("Driver_WriteMappingRule"));
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
}

BOOL VNDispatcher::Driver_WriteDebugRule(VNDebugRule *pDebugRule)
{
	if (!m_pLoader)
	{
		MyMessageBox_Error(_T("Driver_WriteDebugRule"));
		return FALSE;
	}
	
	int nDataInLen;
	UCHAR *pDataIn = genDebugRule(pDebugRule, nDataInLen);
	if (pDataIn == NULL)
	{
		return FALSE;
	}
	int nDataOut;
	int nRetDataLen;
	
	m_pLoader->writeAndReadFromNdis(IOCTL_VNCIO_WRITE_DEBUG_RULE, pDataIn, nDataInLen, (UCHAR*) &nDataOut, 4, nRetDataLen);
	
	freeDebugRule(pDataIn);
	if (nRetDataLen != 4)
	{
		MyMessageBox_Error(_T("Driver_WriteDebugRule"));
		return FALSE;
	}
	else
	{
		if (nDataOut != 1)
		{
			MyMessageBox_Error(_T("Driver_WriteDebugRule"));
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
}

BOOL VNDispatcher::Driver_ClearRules()
{
	if (!m_pLoader)
	{
		MyMessageBox_Error(_T("Driver_ClearRules"));
		return FALSE;
	}
	
	int nDataInLen = 4;
	UCHAR *pDataIn = new UCHAR[4];
	int nDataOut;
	int nRetDataLen;
	
	m_pLoader->writeAndReadFromNdis(IOCTL_VNCIO_CLEAR_RULES, pDataIn, nDataInLen, (UCHAR*) &nDataOut, 4, nRetDataLen);
	
	delete[] pDataIn;
	if (nRetDataLen != 4)
	{
		MyMessageBox_Error(_T("Driver_ClearRules"));
		return FALSE;
	}
	else
	{
		if (nDataOut != 1)
		{
			MyMessageBox_Error(_T("Driver_ClearRules"));
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
}
