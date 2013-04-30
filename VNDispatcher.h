#include <vector>
using namespace std;

#define POLICY_TYPE_SERVER 0
#define POLICY_TYPE_CLIENT 1

class VNPolicy
{
public:
	int no;
	int type;
	CString cIP;
	CString cMask;
	CString cGate;

	CString vIP;
	CString vMask;
	CString vGate;

	CString pIP;
	CString pMask;
	CString pGate;

	VNPolicy(int no, CString sIP, CString sMask, CString sGate)
	{
		this->no = no;
		this->type = POLICY_TYPE_SERVER;
		this->cIP = sIP;
		this->cMask = sMask;
		this->cGate = sGate;
	}
	VNPolicy(int no, CString vIP, CString vMask, CString vGate, CString pIP, CString pMask, CString pGate)
	{
		this->no = no;
		this->type = POLICY_TYPE_CLIENT;
		this->vIP = vIP;
		this->vMask = vMask;
		this->vGate = vGate;
		this->pIP = pIP;
		this->pMask = pMask;
		this->pGate = pGate;
	}
};

class VNMappingRule
{
public:
	CString vIP;
	CString vIP_Mask;
	CString vIP_Gate;
	CString pIP;
	CString pIP_Mask; //后来生成的，用来保存原有值
	CString pIP_Gate; //后来生成的，用来保存原有值
	CString iMAC;
	VNMappingRule(CString strVIP, CString strPIP, CString strIMAC)
	{
		this->vIP = strVIP;
		this->pIP = strPIP;
		this->iMAC = strIMAC;
	}
	VNMappingRule(CString strVIP, CString strVIP_Mask, CString strVIP_Gate, CString strPIP, CString strIMAC)
	{
		this->vIP = strVIP;
		this->vIP_Mask = strVIP_Mask;
		this->vIP_Gate = strVIP_Gate;
		this->pIP = strPIP;
		this->iMAC = strIMAC;
	}
	~VNMappingRule()
	{
	}
};

class VNCoreRule
{
public:
	CString cIP;
	vector<VNMappingRule*> pArrMappingRules;
	VNCoreRule(CString strCIP)
	{
		this->cIP = strCIP;
	}
	~VNCoreRule()
	{
		for (int i = 0; i < pArrMappingRules.size(); i ++)
		{
			delete pArrMappingRules[i];
		}
		pArrMappingRules.clear();
	}
	void addMappingRule(VNMappingRule *pMappingRule)
	{
		pArrMappingRules.push_back(pMappingRule);
	}
};

//////////////////////////Debug_Rule//////////////////////////
// ULONG		eType; //TYPE_IP | TYPE_ARP_REQUEST | TYPE_ARP_REPLY
// ULONG		iType; //TYPE_IP | TYPE_ARP_REQUEST | TYPE_ARP_REPLY | TYPE_NONE
// UCHAR		eDstMAC[6];
// UCHAR		eSrcMAC[6];
// ULONG		eDstIP;
// ULONG		eSrcIP;
// UCHAR		iDstMAC[6];
// UCHAR		iSrcMAC[6];
// ULONG		iDstIP;
// ULONG		iSrcIP;
// UCHAR		content[20];

#define TYPE_NONE				3

class VNDebugRule
{
public:
	int			eType;
	int			iType;
	CString		eDstMAC;
	CString		eSrcMAC;
	CString		eDstIP;
	CString		eSrcIP;
	CString		iDstMAC;
	CString		iSrcMAC;
	CString		iDstIP;
	CString		iSrcIP;
	BYTE		content[30];
	VNDebugRule()
	{
		eType = TYPE_NONE;
		iType = TYPE_NONE;
		eDstMAC = eSrcMAC = iDstMAC = iSrcMAC = _T("00-00-00-00-00-00");
		eDstIP = eSrcIP = iDstIP = iSrcIP = _T("0.0.0.0");
		for (int i = 0; i < 30; i ++)
		{
			content[i] = i;
		}
	}
};

class VNDriverLoaderEx;
class VNFilter;

class VNDispatcher
{
public:
	vector<VNCoreRule*> m_pArrCoreRules;
	vector<VNDebugRule*> m_pArrDebugRules;
	VNCoreRule *m_pCurCoreRule; //在Center和Margin时都有意义
	VNMappingRule *m_pCurMappingRule; //在Margin时有意义

	CString m_strDataDirectory;
	CString m_strRuleFile;
	int m_nModeCenter; //1-Center 0-Margin -1-Unknown

	VNDriverLoaderEx *m_pLoader;
	VNFilter *m_pFilter;

	CString m_strLines;

public:
	VNDispatcher();
	~VNDispatcher();

	CString itos(int i);
	int stoi(CString s);
	BOOL FolderExist(CString strPath);
	BOOL FileExist(CString strPathName);
	void initDataDir();

	void clearAllRules();
	void readAndParseRuleFile();
	VNCoreRule* parseCoreRule(CString strCoreRule);
	VNMappingRule* parseMappingRule(CString strMappingRule);
	CString parseBracketContent(CString strText, CString strTag);
	CString parseColonContent(CString strText, CString strTag);
	BOOL parseCommaContent(CString strText, CString &strContent1, CString &strContent2, CString &strContent3);
	void refineElement(CString &strText);

	void displayWholeFile();
	CString getWholeFile();
	void displayRules();
	void prepareDebugRules();
	int getDebugRuleCount();
	void updateDebugButtons();
	void displayDebugRule(int nIndex);
	void displayDebugRule(VNDebugRule *pDebugRule);
	void writeDebugRule(int eType, int iType, CString eDstMAC, CString eSrcMAC, CString eDstIP, CString eSrcIP,
		CString iDstMAC, CString iSrcMAC, CString iDstIP, CString iSrcIP);

	void setLoader(VNDriverLoaderEx *pLoader);
	void setFilter(VNFilter *pFilter);

	BOOL autoSelectModeAndRule(int &nMappingRuleIndex);
	UCHAR* genCoreRule(VNCoreRule *pCoreRule, int &nRetDataLen);
	UCHAR* genMappingRule(VNMappingRule *pMappingRule, int &nRetDataLen);
	UCHAR* genDebugRule(VNDebugRule *pDebugRule, int &nRetDataLen);
	void freeCoreRule(UCHAR *pData);
	void freeMappingRule(UCHAR *pData);
	void freeDebugRule(UCHAR *pData);

	BOOL writeMode();
	void changeToVIP();
	void changeToPIP();
	void writeBothRules();
	void writeDebugRule(VNDebugRule *pDebugRule);
	void startDriver();
	void stopDriver();

	BOOL Driver_WriteMode(BOOL bCenterOrMargin);
	BOOL Driver_StartStop(BOOL bStartOrStop);
	BOOL Driver_WriteCoreRule(VNCoreRule *pCoreRule);
	BOOL Driver_WriteMappingRule(VNMappingRule *pMappingRule);
	BOOL Driver_WriteDebugRule(VNDebugRule *pDebugRule);
	BOOL Driver_ClearRules();
};