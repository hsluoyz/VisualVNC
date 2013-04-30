// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__96ACB862_EA3D_4B04_BB55_97210A71FCCB__INCLUDED_)
#define AFX_MAINFRM_H__96ACB862_EA3D_4B04_BB55_97210A71FCCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "WorkspaceBar.h"
#include "WorkspaceBar2.h"
#include "OutputBar.h"
#include "TaskPane.h"
#include "PropertiesViewBar.h"

#include "SocketManager.h"

#include <vector>
using namespace std;

const int SOCK_TCP	= 0;
const int SOCK_UDP  = 1;

#define CFrameWnd CBCGPFrameWnd

class VNFilter;
class VNDispatcher;
class VNDriverLoader;
class VNDriverLoaderEx;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CBCGPRibbonEdit* m_pEditOldIP;
	CBCGPRibbonEdit* m_pEditOldMask;
	CBCGPRibbonEdit* m_pEditOldGateway;
	CBCGPRibbonEdit* m_pEditNewIP;
	CBCGPRibbonEdit* m_pEditNewMask;
	CBCGPRibbonEdit* m_pEditNewGateway;
	CBCGPRibbonEdit* m_pEditIPToQuery;
	CBCGPRibbonEdit* m_pEditMacToQuery;
	CBCGPRibbonEdit* m_pEditDriverName;
	CBCGPRibbonEdit* m_pEditDriverLinkName;
	CBCGPRibbonEdit* m_pEditRuleFile;
	CBCGPRibbonEdit* m_pEditStatus;
	CBCGPRibbonEdit* m_pEditServerIP;

	CBCGPToolBarImages m_imagesDDGallery;

	CBCGPRibbonEdit* m_pEditVIP_IP;
	CBCGPRibbonEdit* m_pEditVIP_Mask;
	CBCGPRibbonEdit* m_pEditVIP_Gate;
	CBCGPRibbonEdit* m_pEditPIP_IP;
	CBCGPRibbonEdit* m_pEditPIP_Mask;
	CBCGPRibbonEdit* m_pEditPIP_Gate;
	CBCGPRibbonEdit* m_pEditNow_IP;
	CBCGPRibbonEdit* m_pEditNow_Mask;
	CBCGPRibbonEdit* m_pEditNow_Gate;
	CBCGPRibbonPaletteButton* m_pPltDisplayDebug;

	CBCGPRibbonEdit* m_pEditMTU;

	CBCGPRibbonComboBox* m_pComboEType;
	CBCGPRibbonComboBox* m_pComboIType;
	CBCGPRibbonEdit* m_pEditEDstMAC;
	CBCGPRibbonEdit* m_pEditESrcMAC;
	CBCGPRibbonEdit* m_pEditEDstIP;
	CBCGPRibbonEdit* m_pEditESrcIP;
	CBCGPRibbonEdit* m_pEditIDstMAC;
	CBCGPRibbonEdit* m_pEditISrcMAC;
	CBCGPRibbonEdit* m_pEditIDstIP;
	CBCGPRibbonEdit* m_pEditISrcIP;

	CString m_initOldIP;
	CString m_initOldMask;
	CString m_initOldGateway;
	CString m_initNewIP;
	CString m_initNewMask;
	CString m_initNewGateway;
	CString m_initIPToQuery;
	CString m_initMacToQuery;
	CString m_initDriverName;
	CString m_initDriverLinkName;
	CString m_initRuleFile;
	CString m_initStatus;
	CString m_initServerIP;

	VNFilter *m_pFilter;
	VNDispatcher *m_pDispatcher;
	VNDriverLoader *m_pLoader;
	VNDriverLoaderEx *m_pLoaderEx;
	int m_iDebugRuleCnt;

	BOOL m_bServer;
	vector<CSocketManager*> m_npSMServers;
	CSocketManager *m_pCurServer;
	CString m_strPort;
	int m_nSockType;

	CSocketManager *m_pSMClient;
	SockAddrIn m_SockPeer;

// Operations
public:
	void UpdateMRUFilesList ()
	{
		m_wndTaskPane.UpdateMRUFilesList ();
	}

	CString itos(int i);
	int stoi(CString s);

	void refreshVIP();
	void refreshPIP();
	void refreshNowIP();

	BOOL PickNextAvailable();
	BOOL startServer();
	void stopServer(int iIndex, BOOL bEraseList = TRUE);
	void stopServers();
	void sendAsServer(CString strText);

	void startClient();
	void connectToServer(CString strServer, CString strPort);
	void disconnectWithServer();
	void stopClient();
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CBCGPRibbonBar			m_wndRibbonBar;
	CBCGPRibbonMainButton	m_MainButton;
	CBCGPToolBarImages		m_PanelImages;
	CBCGPRibbonStatusBar	m_wndStatusBar;
	CWorkspaceBar		m_wndWorkSpace;
	CWorkspaceBar2		m_wndWorkSpace2;
	COutputBar		m_wndOutput;
	CTaskPane		m_wndTaskPane;
	CPropertiesBar		m_wndPropGrid;

// Generated message map functions
public:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg void OnTask();
	afx_msg void OnAppLook(UINT id);
	afx_msg void OnUpdateAppLook(CCmdUI* pCmdUI);
	afx_msg LRESULT OnRibbonCustomize (WPARAM wp, LPARAM lp);
	afx_msg void OnToolsOptions();
	afx_msg void OnViewWorkspace();
	afx_msg void OnUpdateViewWorkspace(CCmdUI* pCmdUI);
	afx_msg void OnViewWorkspace2();
	afx_msg void OnUpdateViewWorkspace2(CCmdUI* pCmdUI);
	afx_msg void OnViewOutput();
	afx_msg void OnUpdateViewOutput(CCmdUI* pCmdUI);
	afx_msg void OnViewPropGrid();
	afx_msg void OnUpdateViewPropGrid(CCmdUI* pCmdUI);

	afx_msg LRESULT On_MyMessageBox_Error(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On_Output_Update_Connection(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On_Output_Console_List(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On_Output_A_List(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On_Output_B_List(WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnEditOldIP(){};
	afx_msg void OnEditOldMask(){};
	afx_msg void OnEditOldGateway(){};
	afx_msg void OnEditNewIP(){};
	afx_msg void OnEditNewMask(){};
	afx_msg void OnEditNewGateway(){};

	afx_msg void OnEditVIPIP(){};
	afx_msg void OnEditVIPMask(){};
	afx_msg void OnEditVIPGateway(){};
	afx_msg void OnEditPIPIP(){};
	afx_msg void OnEditPIPMask(){};
	afx_msg void OnEditPIPGateway(){};
	afx_msg void OnEditNowIP(){};
	afx_msg void OnEditNowMask(){};
	afx_msg void OnEditNowGateway(){};
	afx_msg void OnEditEDstMAC(){};
	afx_msg void OnEditESrcMAC(){};
	afx_msg void OnEditEDstIP(){};
	afx_msg void OnEditESrcIP(){};
	afx_msg void OnEditIDstMAC(){};
	afx_msg void OnEditISrcMAC(){};
	afx_msg void OnEditIDstIP(){};
	afx_msg void OnEditISrcIP(){};
	afx_msg void OnEditDisplayDebug();
	afx_msg void OnUpdateDisplayDebug(){};
	afx_msg void OnEditMTU(){};

	afx_msg void OnBtnChangeToVIP();
	afx_msg void OnBtnChangeToPIP();
	afx_msg void OnBtnSelectEType();
	afx_msg void OnBtnSelectIType(){};

	afx_msg void OnEditIPToQuery(){};
	afx_msg void OnEditMacToQuery(){};
	afx_msg void OnEditDriverName(){};
	afx_msg void OnEditDriverLinkName(){};
	afx_msg void OnEditRuleFile(){};
	afx_msg void OnEditStatus(){};
	afx_msg void OnEditServerIP(){};

	afx_msg void OnBtnListNICs();
	afx_msg void OnBtnConfigIP();
	afx_msg void OnBtnCheckMTU();
	afx_msg void OnBtnChangeMTU();
	afx_msg void OnBtnRestoreMTU();
	afx_msg void OnBtnRebootNICs();
	afx_msg void OnBtnClearLogs();

	afx_msg void OnBtnReadRules();

	afx_msg void OnBtnStartServer();
	afx_msg void OnBtnSendRules();
	afx_msg void OnBtnEndServer();

	afx_msg void OnBtnStartClient();
	afx_msg void OnBtnEndClient();

	afx_msg void OnBtnLoadDriver();
	afx_msg void OnBtnUnloadDriver();
	afx_msg void OnBtnCheckDriver();
	afx_msg void OnBtnConfirmStatus();
	afx_msg void OnBtnModeToServer();
	afx_msg void OnBtnModeToClient();
	afx_msg void OnBtnModeToStop();
	afx_msg void OnBtnWriteRulesAndStart();
	afx_msg void OnBtnWriteDebug();
	afx_msg void OnBtnClearRulesAndStop();
	afx_msg void OnBtnIPToMac();
	DECLARE_MESSAGE_MAP()

	BOOL CreateTaskPane ();
	BOOL CreateRibbonBar ();
	void ShowOptions (int nPage);

	UINT	m_nAppLook;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__96ACB862_EA3D_4B04_BB55_97210A71FCCB__INCLUDED_)
