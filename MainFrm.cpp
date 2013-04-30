// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "VisualVNC.h"
#include "VisualVNCView.h"

#include "MainFrm.h"

#include "DoWithAPIHook.h"
#include "VNFilter.h"
#include "VNDispatcher.h"
#include "VNDriverLoader.h"
#include "VNDriverLoaderEx.h"

#include "CommonIO.h"
#include "DrvFltIp.h"
#include <WinIoCtl.h>

#include "MyMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)


BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_TASK1, OnTask)
	ON_COMMAND(ID_TASK2, OnTask)
	ON_COMMAND(ID_TASK3, OnTask)
	ON_COMMAND(ID_TASK4, OnTask)
	ON_COMMAND(ID_TASK5, OnTask)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2012_DARK, OnAppLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2012_DARK, OnUpdateAppLook)
	ON_COMMAND(ID_VIEW_WORKSPACE, OnViewWorkspace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE, OnUpdateViewWorkspace)
	ON_COMMAND(ID_VIEW_WORKSPACE2, OnViewWorkspace2)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE2, OnUpdateViewWorkspace2)
	ON_COMMAND(ID_VIEW_OUTPUT, OnViewOutput)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT, OnUpdateViewOutput)
	ON_COMMAND(ID_VIEW_PROPERTIES, OnViewPropGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, OnUpdateViewPropGrid)
	ON_REGISTERED_MESSAGE(BCGM_ON_RIBBON_CUSTOMIZE, OnRibbonCustomize)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)

	ON_COMMAND(ID_EDIT_OLD_IP, OnEditOldIP)
	ON_COMMAND(ID_EDIT_OLD_MASK, OnEditOldMask)
	ON_COMMAND(ID_EDIT_OLD_GATEWAY, OnEditOldGateway)
	ON_COMMAND(ID_EDIT_NEW_IP, OnEditNewIP)
	ON_COMMAND(ID_EDIT_NEW_MASK, OnEditNewMask)
	ON_COMMAND(ID_EDIT_NEW_GATEWAY, OnEditNewGateway)

	ON_COMMAND(ID_EDIT_VIP_IP, OnEditVIPIP)
	ON_COMMAND(ID_EDIT_VIP_MASK, OnEditVIPMask)
	ON_COMMAND(ID_EDIT_VIP_GATEWAY, OnEditVIPGateway)
	ON_COMMAND(ID_EDIT_PIP_IP, OnEditPIPIP)
	ON_COMMAND(ID_EDIT_PIP_MASK, OnEditPIPMask)
	ON_COMMAND(ID_EDIT_PIP_GATEWAY, OnEditPIPGateway)
	ON_COMMAND(ID_EDIT_NOW_IP, OnEditNowIP)
	ON_COMMAND(ID_EDIT_NOW_MASK, OnEditNowMask)
	ON_COMMAND(ID_EDIT_NOW_GATEWAY, OnEditNowGateway)

	ON_COMMAND(ID_EDIT_E_DST_MAC, OnEditEDstMAC)
	ON_COMMAND(ID_EDIT_E_SRC_MAC, OnEditESrcMAC)
	ON_COMMAND(ID_EDIT_E_DST_IP, OnEditEDstIP)
	ON_COMMAND(ID_EDIT_E_SRC_IP, OnEditESrcIP)
	ON_COMMAND(ID_EDIT_I_DST_MAC, OnEditIDstMAC)
	ON_COMMAND(ID_EDIT_I_SRC_MAC, OnEditISrcMAC)
	ON_COMMAND(ID_EDIT_I_DST_IP, OnEditIDstIP)
	ON_COMMAND(ID_EDIT_I_SRC_IP, OnEditISrcIP)
	ON_COMMAND(ID_PLT_DISPLAY_DEBUG, OnEditDisplayDebug)
	ON_UPDATE_COMMAND_UI(ID_PLT_DISPLAY_DEBUG, OnUpdateDisplayDebug)

	ON_COMMAND(ID_BTN_CHANGE_TO_VIP, OnBtnChangeToVIP)
	ON_COMMAND(ID_BTN_CHANGE_TO_PIP, OnBtnChangeToPIP)
	ON_COMMAND(ID_COMBO_E_TYPE, OnBtnSelectEType)
	ON_COMMAND(ID_COMBO_I_TYPE, OnBtnSelectIType)

	ON_COMMAND(ID_EDIT_MTU, OnEditMTU)
	ON_COMMAND(ID_BTN_CHECK_MTU, OnBtnCheckMTU)
	ON_COMMAND(ID_BTN_CHANGE_MTU, OnBtnChangeMTU)
	ON_COMMAND(ID_BTN_RESTORE_MTU, OnBtnRestoreMTU)

	ON_COMMAND(ID_EDIT_IP_TO_QUERY, OnEditIPToQuery)
	ON_COMMAND(ID_EDIT_MAC_TO_QUERY, OnEditMacToQuery)
	ON_COMMAND(ID_EDIT_DRIVER_NAME, OnEditDriverName)
	ON_COMMAND(ID_EDIT_DRIVER_LINK_NAME, OnEditDriverLinkName)
	ON_COMMAND(ID_EDIT_RULE_FILE, OnEditRuleFile)
	ON_COMMAND(ID_EDIT_STATUS, OnEditStatus)
	ON_COMMAND(ID_EDIT_SERVER_IP, OnEditServerIP)
	
	ON_COMMAND(ID_BTN_LIST_NICS, OnBtnListNICs)
	ON_COMMAND(ID_BTN_CONFIG_IP, OnBtnConfigIP)
	ON_COMMAND(ID_BTN_REBOOT_NICS, OnBtnRebootNICs)
	ON_COMMAND(ID_BTN_CLEAR_LOGS, OnBtnClearLogs)

	ON_COMMAND(ID_BTN_READ_RULES, OnBtnReadRules)

	ON_COMMAND(ID_BTN_START_SERVER, OnBtnStartServer)
	ON_COMMAND(ID_BTN_SEND_RULES, OnBtnSendRules)
	ON_COMMAND(ID_BTN_END_SERVER, OnBtnEndServer)

	ON_COMMAND(ID_BTN_START_CLIENT, OnBtnStartClient)
	ON_COMMAND(ID_BTN_END_CLIENT, OnBtnEndClient)

	ON_COMMAND(ID_BTN_LOAD_DRIVER, OnBtnLoadDriver)
	ON_COMMAND(ID_BTN_UNLOAD_DRIVER, OnBtnUnloadDriver)
	ON_COMMAND(ID_BTN_CHECK_DRIVER, OnBtnCheckDriver)
	ON_COMMAND(ID_BTN_CONFIRM_STATUS, OnBtnConfirmStatus)
	ON_COMMAND(ID_BTN_MODE_TO_SERVER, OnBtnModeToServer)
	ON_COMMAND(ID_BTN_MODE_TO_CLIENT, OnBtnModeToClient)
	ON_COMMAND(ID_BTN_MODE_TO_STOP, OnBtnModeToStop)
	ON_COMMAND(ID_BTN_WRITE_RULES_AND_START, OnBtnWriteRulesAndStart)
	ON_COMMAND(ID_BTN_WRITE_DEBUG, OnBtnWriteDebug)
	ON_COMMAND(ID_BTN_CLEAR_RULES_AND_STOP, OnBtnClearRulesAndStop)
	ON_COMMAND(ID_BTN_IP_TO_MAC, OnBtnIPToMac)

	ON_MESSAGE(ID_GUI_MESSAGE + 0, On_MyMessageBox_Error)
	ON_MESSAGE(ID_GUI_MESSAGE + 1, On_MyMessageBox_Error)
	ON_MESSAGE(ID_GUI_MESSAGE + 2, On_Output_Console_List)
	ON_MESSAGE(ID_GUI_MESSAGE + 3, On_Output_A_List)
	ON_MESSAGE(ID_GUI_MESSAGE + 4, On_Output_B_List)

	ON_MESSAGE(ID_GUI_MESSAGE + 13, On_Output_Update_Connection)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_nAppLook = theApp.GetInt (_T("ApplicationLook"), ID_VIEW_APPLOOK_2010_2);

	// TODO: add member initialization code here
	doWithTheAPIHookStuff();

	m_pFilter = NULL;
	m_pDispatcher = new VNDispatcher();
	m_iDebugRuleCnt = m_pDispatcher->getDebugRuleCount();
	delete m_pDispatcher;
	m_pDispatcher = NULL;
	m_pLoader = NULL;
	m_pLoaderEx = NULL;

	m_bServer = FALSE;
	m_pCurServer = NULL;
	m_strPort = _T("2000");
	m_nSockType = SOCK_TCP;	//default TCP

	m_pSMClient = NULL;

	m_initNewIP = _T("");
	m_initNewMask = _T("");
	m_initNewGateway = _T("");

	m_initIPToQuery = _T("192.168.1.1");
	m_initMacToQuery = _T("");
// 	m_initDriverName = _T("DrvFltIp.sys");
// 	m_initDriverLinkName = _T("DrvFltIp");
	m_initDriverName = _T("passthru.sys");
	m_initDriverLinkName = _T("VNCPassthru");
	m_initRuleFile = _T("Rules.txt");
	m_initStatus = _T("N/A");

	m_initServerIP = _T("192.168.1.232");

	/*
	PUCHAR pMac = new UCHAR(6);
	VNFilter::MAC_Str2Bytes("78-45-C4-1E-A3-A4", pMac);
	CString a = VNFilter::MAC_Bytes2Str(pMac);
	delete pMac;
	*/
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!CreateRibbonBar ())
	{
		TRACE0("Failed to create ribbon bar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: add your status bar panes here:
	m_wndStatusBar.AddElement (new CBCGPRibbonStatusBarPane (
		ID_STATUSBAR_PANE1, _T("北航·网络技术北京市重点实验室"), TRUE), _T("北航·网络技术北京市重点实验室"));
	//m_wndStatusBar.AddExtendedElement (new CBCGPRibbonStatusBarPane (
	//	ID_STATUSBAR_PANE2, _T("Pane 2"), TRUE), _T("Pane 2"));


	// Load control bar icons:
	CBCGPToolBarImages imagesWorkspace;
	imagesWorkspace.SetImageSize (CSize (16, 16));
	imagesWorkspace.SetTransparentColor (RGB (255, 0, 255));
	imagesWorkspace.Load (IDB_WORKSPACE);
	imagesWorkspace.SmoothResize(globalData.GetRibbonImageScale());

	if (!m_wndWorkSpace.Create (_T("View  1"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_WORKSPACE,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Workspace bar\n");
		return -1;      // fail to create
	}

	m_wndWorkSpace.SetIcon (imagesWorkspace.ExtractIcon (0), FALSE);

	if (!m_wndWorkSpace2.Create (_T("View 2"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_WORKSPACE2,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Workspace bar 2\n");
		return -1;      // fail to create
	}

	m_wndWorkSpace2.SetIcon (imagesWorkspace.ExtractIcon (1), FALSE);

	if (!m_wndOutput.Create (_T("Output"), this, CRect (0, 0, 150, 150),
		TRUE /* Has gripper */, ID_VIEW_OUTPUT,
		WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create output bar\n");
		return -1;      // fail to create
	}

	m_wndOutput.SetIcon (imagesWorkspace.ExtractIcon (2), FALSE);

	if (!CreateTaskPane ())
	{
		TRACE0("Failed to create tasks pane\n");
		return -1;      // fail to create
	}

	m_wndTaskPane.SetIcon (imagesWorkspace.ExtractIcon (4), FALSE);

	if (!m_wndPropGrid.Create (_T("Properties"), this, CRect (0, 0, 200, 200),
		TRUE, 
		ID_VIEW_PROPERTIES,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties Bar\n");
		return FALSE;		// fail to create
	}

	m_wndPropGrid.SetIcon (imagesWorkspace.ExtractIcon (3), FALSE);
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndWorkSpace.EnableDocking(CBRS_ALIGN_ANY);
	m_wndWorkSpace2.EnableDocking(CBRS_ALIGN_ANY);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	m_wndTaskPane.EnableDocking(CBRS_ALIGN_RIGHT | CBRS_ALIGN_LEFT);
	m_wndPropGrid.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHideBars(CBRS_ALIGN_ANY);
	DockControlBar (&m_wndWorkSpace);
	m_wndWorkSpace2.AttachToTabWnd (&m_wndWorkSpace, BCGP_DM_STANDARD, FALSE, NULL);
	DockControlBar(&m_wndTaskPane);
	m_wndPropGrid.DockToWindow (&m_wndTaskPane, CBRS_ALIGN_BOTTOM);
	DockControlBar(&m_wndOutput);


	OnAppLook (m_nAppLook);

	if (m_pFilter)
	{
		delete m_pFilter;
		m_pFilter = NULL;
	}
	m_pFilter = new VNFilter();
	m_pFilter->listNICs(FALSE);
	
	if (m_pFilter)
	{
		CString strOldIP;
		CString strOldMask;
		CString strOldGateway;
		m_pFilter->getOldIPConfig(strOldIP, strOldMask, strOldGateway);
		m_pEditOldIP->SetEditText(strOldIP);
		m_pEditOldMask->SetEditText(strOldMask);
		m_pEditOldGateway->SetEditText(strOldGateway);
	}
	refreshNowIP();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

CString CMainFrame::itos(int i)
{
	CString strTemp;
	strTemp.Format(_T("%d"), i);
	return strTemp;
}

int CMainFrame::stoi(CString s)
{
	return atoi(s);
}

BOOL CMainFrame::CreateTaskPane ()
{
	CRect rectDummy(0, 0, 200, 400);
	if (!m_wndTaskPane.Create(_T("Tasks Pane"), this, rectDummy, TRUE, ID_VIEW_TASKPANE,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_BCGP_CLOSE | CBRS_BCGP_FLOAT))
	{
		TRACE0("Failed to create task pane\n");
		return FALSE;      // fail to create
	}

	return TRUE;
}

void CMainFrame::OnTask()
{
	MessageBox (_T("TODO: Task"));
}

void CMainFrame::OnAppLook(UINT id)
{
	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);

	m_nAppLook = id;

	CBCGPTabbedControlBar::m_StyleTabWnd = CBCGPTabWnd::STYLE_3D;

	m_wndRibbonBar.SetScenicLook(FALSE);
	m_wndRibbonBar.EnableMinimizeButton(FALSE);

	switch (m_nAppLook)
	{
	case ID_VIEW_APPLOOK_2007:
	case ID_VIEW_APPLOOK_2007_1:
	case ID_VIEW_APPLOOK_2007_2:
	case ID_VIEW_APPLOOK_2007_3:
		// enable Office 2007 look:

		switch (m_nAppLook)
		{
		case ID_VIEW_APPLOOK_2007:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_2007_1:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_2007_2:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Silver);
			break;

		case ID_VIEW_APPLOOK_2007_3:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Aqua);
			break;
		}

		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2010_1:
	case ID_VIEW_APPLOOK_2010_2:
	case ID_VIEW_APPLOOK_2010_3:
		// enable Office 2010 look:
		switch (m_nAppLook)
		{
		case ID_VIEW_APPLOOK_2010_1:
			CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Blue);
			break;

		case ID_VIEW_APPLOOK_2010_2:
			CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Black);
			break;

		case ID_VIEW_APPLOOK_2010_3:
			CBCGPVisualManager2010::SetStyle (CBCGPVisualManager2010::VS2010_Silver);
			break;
		}

		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2010));

		m_wndRibbonBar.SetScenicLook();
		m_wndRibbonBar.EnableMinimizeButton();

		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WIN7:
		// enable Windows 7 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerScenic));

		m_wndRibbonBar.SetScenicLook();

		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	default:
		ASSERT (FALSE);
	}

	m_wndTaskPane.RecalcLayout ();

	CBCGPDockManager* pDockManager = GetDockManager ();
	if (pDockManager != NULL)
	{
		ASSERT_VALID (pDockManager);
		pDockManager->AdjustBarFrames ();
	}

	CBCGPTabbedControlBar::ResetTabs ();

	RecalcLayout ();
	RedrawWindow (NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt (_T("ApplicationLook"), m_nAppLook);
}

void CMainFrame::OnUpdateAppLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio (m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::CreateRibbonBar()
{
	m_wndRibbonBar.EnableCustomization();

	if (!m_wndRibbonBar.Create(this))
	{
		return FALSE;
	}

	// Load panel images:
	m_PanelImages.SetImageSize (CSize (16, 16));
	m_PanelImages.Load (IDB_RIBBON_ICONS);

	// Init main button:
	m_MainButton.SetImage (IDB_RIBBON_MAIN);
	m_MainButton.SetToolTipText (_T("File"));
	m_MainButton.SetText (_T("\nf"));

	m_wndRibbonBar.SetMainButton (&m_MainButton, CSize (45, 45));

	CBCGPRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory (
		_T("File"), IDB_RIBBON_FILESMALL, IDB_RIBBON_FILELARGE);

	pMainPanel->Add (new CBCGPRibbonButton (ID_FILE_NEW, _T("&New"), 0, 0));
	pMainPanel->Add (new CBCGPRibbonButton (ID_FILE_OPEN, _T("&Open..."), 1, 1));

	pMainPanel->Add (new CBCGPRibbonButton (ID_FILE_SAVE, _T("&Save"), 2, 2));

	pMainPanel->Add (new CBCGPRibbonButton (ID_FILE_SAVE_AS, _T("Save &As..."), 3, 3));

	CBCGPRibbonButton* pBtnPrint = new CBCGPRibbonButton (ID_FILE_PRINT, _T("&Print"), 4, 4);

	pBtnPrint->AddSubItem (new CBCGPRibbonLabel (_T("Preview and print the document")));
	pBtnPrint->AddSubItem (new CBCGPRibbonButton (ID_FILE_PRINT, _T("&Print"), 4, 4, TRUE));
	pBtnPrint->AddSubItem (new CBCGPRibbonButton (ID_FILE_PRINT_DIRECT, _T("&Quick Print"), 7, 7, TRUE));
	pBtnPrint->AddSubItem (new CBCGPRibbonButton (ID_FILE_PRINT_PREVIEW, _T("Print Pre&view"), 6, 6, TRUE));

	pBtnPrint->SetKeys (_T("p"), _T("w"));

	pMainPanel->Add (pBtnPrint);

	pMainPanel->AddRecentFilesList (_T("Recent Documents"), 300, TRUE /* Show Pins */);

	pMainPanel->AddToBottom (new CBCGPRibbonMainPanelButton (ID_TOOLS_OPTIONS, _T("Opt&ions"), 9));
	pMainPanel->AddToBottom (new CBCGPRibbonMainPanelButton (ID_APP_EXIT, _T("E&xit"), 8));

	// Add "Home" category with "Clipboard" panel:
	CBCGPRibbonCategory* pBasicCategory = m_wndRibbonBar.AddCategory (
		_T("&Basic"),
		IDB_RIBBON_HOMESMALL,
		IDB_RIBBON_HOMELARGE);

	CBCGPRibbonCategory* pMarginCategory = m_wndRibbonBar.AddCategory (
		_T("&Margin"),
		IDB_RIBBON_HOMESMALL,
		IDB_RIBBON_HOMELARGE);

	CBCGPRibbonCategory* pNetworkCategory = m_wndRibbonBar.AddCategory (
		_T("&Network"),
		IDB_RIBBON_HOMESMALL,
		IDB_RIBBON_HOMELARGE);

	CBCGPRibbonCategory* pConnectionCategory = m_wndRibbonBar.AddCategory (
		_T("&Connection"),
		IDB_RIBBON_HOMESMALL,
		IDB_RIBBON_HOMELARGE);

	CBCGPRibbonCategory* pDebugCategory = m_wndRibbonBar.AddCategory (
		_T("&Debug"),
		IDB_RIBBON_HOMESMALL,
		IDB_RIBBON_HOMELARGE);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "Clipboard" panel:
	CBCGPRibbonPanel* pPanelClipboard = pBasicCategory->AddPanel(_T("Clipboard"), 
		m_PanelImages.ExtractIcon (1));

	CBCGPRibbonButton* pBtnPaste = new CBCGPRibbonButton (ID_EDIT_PASTE, _T("Paste\nv"), 0, 0);
	pBtnPaste->SetMenu (IDR_PASTE_MENU, TRUE);
	pPanelClipboard->Add (pBtnPaste);

	pPanelClipboard->Add (new CBCGPRibbonButton (ID_EDIT_CUT, _T("Cut\nx"), 1));
	pPanelClipboard->Add (new CBCGPRibbonButton (ID_EDIT_COPY, _T("Copy\nc"), 2));
	pPanelClipboard->Add (new CBCGPRibbonButton (ID_EDIT_CLEAR, _T("Erase\nr"), 3));

	pPanelClipboard->SetKeys (_T("zc"));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "Window" panel:
	CBCGPRibbonPanel* pPanelWindow = pBasicCategory->AddPanel(_T("Window"), 
		m_PanelImages.ExtractIcon (2));

	pPanelWindow->Add (new CBCGPRibbonCheckBox (ID_VIEW_WORKSPACE, _T("View 1\n1")));
	pPanelWindow->Add (new CBCGPRibbonCheckBox (ID_VIEW_WORKSPACE2, _T("View 2\n2")));
	pPanelWindow->Add (new CBCGPRibbonCheckBox (ID_VIEW_OUTPUT, _T("Output\no")));
	pPanelWindow->Add (new CBCGPRibbonCheckBox (ID_VIEW_PROPERTIES, _T("Property Grid\ng")));
	pPanelWindow->Add (new CBCGPRibbonCheckBox (ID_VIEW_STATUS_BAR, _T("Status Bar\ns")));

	pPanelWindow->SetKeys (_T("zw"));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "Role Change" panel:
	CBCGPRibbonPanel* pPanelRoleChange = pMarginCategory->AddPanel(_T("Role Change"), 
		m_PanelImages.ExtractIcon (2));
	
	m_pEditVIP_IP = new CBCGPRibbonEdit(ID_EDIT_VIP_IP, 100, _T("VIP  IP :"), 2);
	m_pEditVIP_IP->SetTextAlign(ES_LEFT);
	m_pEditVIP_IP->SetEditText(_T("N/A"));
	m_pEditVIP_Mask = new CBCGPRibbonEdit(ID_EDIT_VIP_MASK, 100, _T("VIP Mask:"), 2);
	m_pEditVIP_Mask->SetTextAlign(ES_LEFT);
	m_pEditVIP_Mask->SetEditText(_T("N/A"));
	m_pEditVIP_Gate = new CBCGPRibbonEdit(ID_EDIT_VIP_GATEWAY, 100, _T("VIP Gate:"), 2);
	m_pEditVIP_Gate->SetTextAlign(ES_LEFT);
	m_pEditVIP_Gate->SetEditText(_T("N/A"));
	
	m_pEditPIP_IP = new CBCGPRibbonEdit(ID_EDIT_PIP_IP, 100, _T("PIP  IP :"), 2);
	m_pEditPIP_IP->SetTextAlign(ES_LEFT);
	m_pEditPIP_IP->SetEditText(_T("N/A"));
	m_pEditPIP_Mask = new CBCGPRibbonEdit(ID_EDIT_PIP_MASK, 100, _T("PIP Mask:"), 2);
	m_pEditPIP_Mask->SetTextAlign(ES_LEFT);
	m_pEditPIP_Mask->SetEditText(_T("N/A"));
	m_pEditPIP_Gate = new CBCGPRibbonEdit(ID_EDIT_PIP_GATEWAY, 100, _T("PIP Gate:"), 2);
	m_pEditPIP_Gate->SetTextAlign(ES_LEFT);
	m_pEditPIP_Gate->SetEditText(_T("N/A"));

	m_pEditNow_IP = new CBCGPRibbonEdit(ID_EDIT_NOW_IP, 100, _T("Now  IP :"), 2);
	m_pEditNow_IP->SetTextAlign(ES_LEFT);
	m_pEditNow_IP->SetEditText(_T(""));
	m_pEditNow_Mask = new CBCGPRibbonEdit(ID_EDIT_NOW_MASK, 100, _T("Now Mask:"), 2);
	m_pEditNow_Mask->SetTextAlign(ES_LEFT);
	m_pEditNow_Mask->SetEditText(_T(""));
	m_pEditNow_Gate = new CBCGPRibbonEdit(ID_EDIT_NOW_GATEWAY, 100, _T("Now Gate:"), 2);
	m_pEditNow_Gate->SetTextAlign(ES_LEFT);
	m_pEditNow_Gate->SetEditText(_T(""));

	CBCGPRibbonButton* pBtnChangeToVIP = new CBCGPRibbonButton(ID_BTN_CHANGE_TO_VIP, _T("Change to VIP\nv"), 0, 3);
	CBCGPRibbonButton* pBtnChangeToPIP = new CBCGPRibbonButton(ID_BTN_CHANGE_TO_PIP, _T("Change to PIP\np"), 0, 3);
	

	pPanelRoleChange->Add (m_pEditVIP_IP);
	pPanelRoleChange->Add (m_pEditVIP_Mask);
	pPanelRoleChange->Add (m_pEditVIP_Gate);
	pPanelRoleChange->Add (m_pEditPIP_IP);
	pPanelRoleChange->Add (m_pEditPIP_Mask);
	pPanelRoleChange->Add (m_pEditPIP_Gate);
	pPanelRoleChange->Add (m_pEditNow_IP);
	pPanelRoleChange->Add (m_pEditNow_Mask);
	pPanelRoleChange->Add (m_pEditNow_Gate);

	pPanelRoleChange->Add (pBtnChangeToVIP);
	pPanelRoleChange->Add (pBtnChangeToPIP);
	

	pPanelRoleChange->SetKeys (_T("zl"));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "MTU Config" panel:
	CBCGPRibbonPanel* pPanelMTUConfig = pMarginCategory->AddPanel(_T("MTU Config"), 
		m_PanelImages.ExtractIcon (2));

	m_pEditMTU = new CBCGPRibbonEdit(ID_EDIT_MTU, 100, _T("Now MTU:"), 2);
	m_pEditMTU->SetTextAlign(ES_LEFT);
	m_pEditMTU->SetEditText(_T(""));

	CBCGPRibbonButton* pBtnCheckMTU = new CBCGPRibbonButton(ID_BTN_CHECK_MTU, _T("Check MTU\nc"), 0, 3);
	CBCGPRibbonButton* pBtnChangeMTU = new CBCGPRibbonButton(ID_BTN_CHANGE_MTU, _T("Change MTU\nh"), 0, 3);
	CBCGPRibbonButton* pBtnRestoreMTU = new CBCGPRibbonButton(ID_BTN_RESTORE_MTU, _T("Restore MTU\ns"), 0, 3);

	pPanelMTUConfig->Add (m_pEditMTU);
	pPanelMTUConfig->Add (pBtnCheckMTU);
	pPanelMTUConfig->Add (pBtnChangeMTU);
	pPanelMTUConfig->Add (pBtnRestoreMTU);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "Adapters" panel:
	CBCGPRibbonPanel* pPanelAdapters = pMarginCategory->AddPanel(_T("Adapters"), 
		m_PanelImages.ExtractIcon (2));
	
	CBCGPRibbonButton* pBtnListNICs = new CBCGPRibbonButton(ID_BTN_LIST_NICS, _T("List NICs\nl"), 0, 3);
	CBCGPRibbonButton* pBtnClearLogs = new CBCGPRibbonButton(ID_BTN_CLEAR_LOGS, _T("Clear Logs\ne"), 0, 3);
	CBCGPRibbonButton* pBtnRebootNICs = new CBCGPRibbonButton(ID_BTN_REBOOT_NICS, _T("Reboot NICs\nr"), 0, 3);
	
	pPanelAdapters->Add (pBtnListNICs);
	pPanelAdapters->Add (pBtnClearLogs);
	pPanelAdapters->Add (pBtnRebootNICs);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "IP Config" panel:
	CBCGPRibbonPanel* pPanelIPConfig = pNetworkCategory->AddPanel(_T("IP Config"), 
		m_PanelImages.ExtractIcon (2));
	
	m_pEditOldIP = new CBCGPRibbonEdit(ID_EDIT_OLD_IP, 100, _T("Old  IP :"), 2);
	m_pEditOldIP->SetTextAlign(ES_LEFT);
	m_pEditOldIP->SetEditText(m_initOldIP);
	m_pEditOldMask = new CBCGPRibbonEdit(ID_EDIT_OLD_MASK, 100, _T("Old Mask:"), 2);
	m_pEditOldMask->SetTextAlign(ES_LEFT);
	m_pEditOldMask->SetEditText(m_initOldMask);
	m_pEditOldGateway = new CBCGPRibbonEdit(ID_EDIT_OLD_GATEWAY, 100, _T("Old Gate:"), 2);
	m_pEditOldGateway->SetTextAlign(ES_LEFT);
	m_pEditOldGateway->SetEditText(m_initOldGateway);

	m_pEditNewIP = new CBCGPRibbonEdit(ID_EDIT_NEW_IP, 100, _T("New  IP :"), 2);
	m_pEditNewIP->SetTextAlign(ES_LEFT);
	m_pEditNewIP->SetEditText(m_initNewIP);
	m_pEditNewMask = new CBCGPRibbonEdit(ID_EDIT_NEW_MASK, 100, _T("New Mask:"), 2);
	m_pEditNewMask->SetTextAlign(ES_LEFT);
	m_pEditNewMask->SetEditText(m_initNewMask);
	m_pEditNewGateway = new CBCGPRibbonEdit(ID_EDIT_NEW_GATEWAY, 100, _T("New Gate:"), 2);
	m_pEditNewGateway->SetTextAlign(ES_LEFT);
	m_pEditNewGateway->SetEditText(m_initNewGateway);
	
	CBCGPRibbonButton* pBtnConfigIP = new CBCGPRibbonButton(ID_BTN_CONFIG_IP, _T("Config New IP\nc"), 0, 3);

	m_pEditIPToQuery = new CBCGPRibbonEdit(ID_EDIT_IP_TO_QUERY, 150, _T("IP  to query:"), 2);
	m_pEditIPToQuery->SetTextAlign(ES_LEFT);
	m_pEditIPToQuery->SetEditText(m_initIPToQuery);

	m_pEditMacToQuery = new CBCGPRibbonEdit(ID_EDIT_MAC_TO_QUERY, 150, _T("Mac to query:"), 2);
	m_pEditMacToQuery->SetTextAlign(ES_LEFT);
	m_pEditMacToQuery->SetEditText(m_initMacToQuery);

	CBCGPRibbonButton* pBtnIPToMac = new CBCGPRibbonButton(ID_BTN_IP_TO_MAC, _T("IP to Mac\ni"), 0, 3);

	pPanelIPConfig->Add (m_pEditOldIP);
	pPanelIPConfig->Add (m_pEditOldMask);
	pPanelIPConfig->Add (m_pEditOldGateway);
	pPanelIPConfig->Add (m_pEditNewIP);
	pPanelIPConfig->Add (m_pEditNewMask);
	pPanelIPConfig->Add (m_pEditNewGateway);
	pPanelIPConfig->Add (pBtnConfigIP);
	pPanelIPConfig->Add (m_pEditIPToQuery);
	pPanelIPConfig->Add (m_pEditMacToQuery);
	pPanelIPConfig->Add (pBtnIPToMac);
	
	pPanelIPConfig->SetKeys (_T("zl"));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "Rules Config" panel:
	CBCGPRibbonPanel* pPanelRulesConfig = pConnectionCategory->AddPanel(_T("Rules Config"), 
		m_PanelImages.ExtractIcon (2));

	CBCGPRibbonButton* pBtnReadRules2 = new CBCGPRibbonButton(ID_BTN_READ_RULES, _T("Read Rules\nr"), 0, 3);
	
	pPanelRulesConfig->Add (pBtnReadRules2);
	
	pPanelRulesConfig->SetKeys (_T("zl"));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "Server Config" panel:
	CBCGPRibbonPanel* pPanelServerConfig = pConnectionCategory->AddPanel(_T("Server Config"), 
		m_PanelImages.ExtractIcon (2));
	
	CBCGPRibbonButton* pBtnStartServer = new CBCGPRibbonButton(ID_BTN_START_SERVER, _T("Start Server\ns"), 0, 3);
	CBCGPRibbonButton* pBtnSendPolicies = new CBCGPRibbonButton(ID_BTN_SEND_RULES, _T("Send\ns"), 0, 3);
	CBCGPRibbonButton* pBtnEndServer = new CBCGPRibbonButton(ID_BTN_END_SERVER, _T("End Server\nn"), 0, 3);
	
	pPanelServerConfig->Add (pBtnStartServer);
	pPanelServerConfig->Add (pBtnSendPolicies);
	pPanelServerConfig->Add (pBtnEndServer);
	
	pPanelServerConfig->SetKeys (_T("zl"));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "Client Config" panel:
	CBCGPRibbonPanel* pPanelClientConfig = pConnectionCategory->AddPanel(_T("Client Config"), 
		m_PanelImages.ExtractIcon (2));
	
	m_pEditServerIP = new CBCGPRibbonEdit(ID_EDIT_SERVER_IP, 100, _T("Server IP :"), 2);
	m_pEditServerIP->SetTextAlign(ES_LEFT);
	m_pEditServerIP->SetEditText(m_initServerIP);

	CBCGPRibbonButton* pBtnStartClient = new CBCGPRibbonButton(ID_BTN_START_CLIENT, _T("Connect As Client\nt"), 0, 3);
	CBCGPRibbonButton* pBtnEndClient = new CBCGPRibbonButton(ID_BTN_END_CLIENT, _T("End Client\nn"), 0, 3);
	
	pPanelClientConfig->Add (m_pEditServerIP);
	pPanelClientConfig->Add (pBtnStartClient);
	pPanelClientConfig->Add (pBtnEndClient);
	
	pPanelClientConfig->SetKeys (_T("zl"));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "Driver Config" panel:
	CBCGPRibbonPanel* pPanelDriverConfig = pBasicCategory->AddPanel(_T("Driver Config"), 
		m_PanelImages.ExtractIcon (2));

	m_pEditDriverName = new CBCGPRibbonEdit(ID_EDIT_DRIVER_NAME, 100, _T("Driver Name:"), 2);
	m_pEditDriverName->SetTextAlign(ES_LEFT);
	m_pEditDriverName->SetEditText(m_initDriverName);

	m_pEditDriverLinkName = new CBCGPRibbonEdit(ID_EDIT_DRIVER_LINK_NAME, 100, _T("Link   Name:"), 2);
	m_pEditDriverLinkName->SetTextAlign(ES_LEFT);
	m_pEditDriverLinkName->SetEditText(m_initDriverLinkName);

	m_pEditRuleFile = new CBCGPRibbonEdit(ID_EDIT_RULE_FILE, 100, _T("Rule   File:"), 2);
	m_pEditRuleFile->SetTextAlign(ES_LEFT);
	m_pEditRuleFile->SetEditText(m_initRuleFile);

	m_pEditStatus = new CBCGPRibbonEdit(ID_EDIT_STATUS, 100, _T("Run Status:"), 2);
	m_pEditStatus->SetTextAlign(ES_LEFT);
	m_pEditStatus->SetEditText(m_initStatus);

	CBCGPRibbonButton* pBtnLoadDriver = new CBCGPRibbonButton(ID_BTN_LOAD_DRIVER, _T("Load Driver\ni"), 0, 3);
	CBCGPRibbonButton* pBtnCheckDriver = new CBCGPRibbonButton(ID_BTN_CHECK_DRIVER, _T("Check Driver\ni"), 0, 3);
	CBCGPRibbonButton* pBtnReadRules = new CBCGPRibbonButton(ID_BTN_READ_RULES, _T("Read Rules\nr"), 0, 3);
	CBCGPRibbonButton* pBtnConfirmStatus = new CBCGPRibbonButton(ID_BTN_CONFIRM_STATUS, _T("Confirm Status\ni"), 0, 3);
	CBCGPRibbonButton* pBtnModeToSever = new CBCGPRibbonButton(ID_BTN_MODE_TO_SERVER, _T("Mode to Server\ni"), 0, 3);
	CBCGPRibbonButton* pBtnModeToClient = new CBCGPRibbonButton(ID_BTN_MODE_TO_CLIENT, _T("Mode to Client\ni"), 0, 3);
	CBCGPRibbonButton* pBtnModeToStop = new CBCGPRibbonButton(ID_BTN_MODE_TO_STOP, _T("Mode to Stop\ni"), 0, 3);
	CBCGPRibbonButton* pBtnWriteRules = new CBCGPRibbonButton(ID_BTN_WRITE_RULES_AND_START, _T("Write Rules and Start Driver\ni"), 0, 3);
	CBCGPRibbonButton* pBtnClearRules = new CBCGPRibbonButton(ID_BTN_CLEAR_RULES_AND_STOP, _T("Stop Driver and Clear Rules\ni"), 0, 3);

	CBCGPRibbonButton* pBtnUnloadDriver = new CBCGPRibbonButton(ID_BTN_UNLOAD_DRIVER, _T("Unload Driver\ni"), 0, 3);

	pPanelDriverConfig->Add (m_pEditDriverName);
	pPanelDriverConfig->Add (m_pEditDriverLinkName);
	pPanelDriverConfig->Add (m_pEditRuleFile);
	pPanelDriverConfig->Add (m_pEditStatus);
	pPanelDriverConfig->Add (pBtnLoadDriver);
	pPanelDriverConfig->Add (pBtnCheckDriver);
	pPanelDriverConfig->Add (pBtnReadRules);
	pPanelDriverConfig->Add (pBtnConfirmStatus);
	//pPanelDriverConfig->Add (pBtnModeToSever);
	//pPanelDriverConfig->Add (pBtnModeToClient);
	//pPanelDriverConfig->Add (pBtnModeToStop);
	pPanelDriverConfig->Add (pBtnWriteRules);
	pPanelDriverConfig->Add (pBtnClearRules);
	pPanelDriverConfig->Add (pBtnUnloadDriver);

	pPanelDriverConfig->SetKeys (_T("zl"));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "Debug Rule Import" panel:
	CBCGPRibbonPanel* pPanelDebugRuleImport = pDebugCategory->AddPanel(_T("Debug Rule Import"), 
		m_PanelImages.ExtractIcon (2));

	// Load control bar icons:
	CBCGPToolBarImages m_imagesDDGallery;
	m_imagesDDGallery.Load (IDB_RIBBON_HOMELARGE);
	m_imagesDDGallery.SetImageSize (CSize (32, 32));

	CBCGPToolBarImages imagesDisplayDebug;
	imagesDisplayDebug.SetImageSize (CSize (32, 32));

	for (int i = 0; i < m_iDebugRuleCnt; i ++)
	{
		imagesDisplayDebug.AddImage(m_imagesDDGallery, i % 7);
	}

// 	CBCGPToolBarImages imagesDisplayDebug2;
// 	imagesDisplayDebug2.Load (IDB_RIBBON_FILELARGE); //IDB_WORKSPACE);
// 	imagesDisplayDebug2.SetImageSize (CSize (32, 32));

	//imagesDisplayDebug.AddImage(imagesDisplayDebug2, 0);
	
	//imagesDisplayDebug.SetTransparentColor (RGB (255, 0, 255));
	//BOOL a = imagesDisplayDebug.Is32BitTransparencySupported();
	//imagesDisplayDebug.SmoothResize(globalData.GetRibbonImageScale());
	
	m_pPltDisplayDebug = new CBCGPRibbonPaletteButton(ID_PLT_DISPLAY_DEBUG, _T("Debug Rule"), 0, 0, imagesDisplayDebug);
	//m_pPltDisplayDebug = new CBCGPRibbonPaletteButton(ID_PLT_DISPLAY_DEBUG, _T("Debug Rule"), 0, 0,
	//	IDB_RIBBON_HOMELARGE, 32);
	//pPltDisplayDebug->SetPalette(imagesDisplayDebug);
	m_pPltDisplayDebug->SetIconsInRow(7);
	m_pPltDisplayDebug->EnableMenuResize(FALSE, TRUE);
	
	//pPltDisplayDebug->AddGroup(_T("ss"), IDB_RIBBON_HOMELARGE, 2);
	//m_pPltDisplayDebug->AddSubItem(new CBCGPRibbonButton(ID_BTN_WRITE_DEBUG, _T("asdasasa"), 0, 3));
	//m_pPltDisplayDebug->AddSubItem(new CBCGPRibbonButton(ID_BTN_WRITE_DEBUG, _T("Write Debug Rule\ni"), 0, 3));
	
	pPanelDebugRuleImport->Add(m_pPltDisplayDebug);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "Debug Packet" panel:
	CBCGPRibbonPanel* pPanelDebugPacket = pDebugCategory->AddPanel(_T("Debug Packet"), 
		m_PanelImages.ExtractIcon (2));

	m_pComboEType = new CBCGPRibbonComboBox(ID_COMBO_E_TYPE, FALSE, 80, _T("Ex Type:"), 2);
	m_pComboIType = new CBCGPRibbonComboBox(ID_COMBO_I_TYPE, FALSE, 80, _T("In Type:"), 2);

	m_pComboEType->AddItem(_T("IP"), TYPE_IP);
	m_pComboEType->AddItem(_T("ARP Request"), TYPE_ARP_REQUEST);
	m_pComboEType->AddItem(_T("ARP Reply"), TYPE_ARP_REPLY);
	m_pComboEType->SelectItem(_T("IP"));

	m_pComboIType->AddItem(_T("IP"), TYPE_IP);
	m_pComboIType->AddItem(_T("ARP Request"), TYPE_ARP_REQUEST);
	m_pComboIType->AddItem(_T("ARP Reply"), TYPE_ARP_REPLY);
	m_pComboIType->AddItem(_T("None"), TYPE_NONE);
	m_pComboIType->SelectItem(_T("IP"));

	m_pEditEDstMAC = new CBCGPRibbonEdit(ID_EDIT_E_DST_MAC, 120, _T("EDstMAC:"), 2);
	m_pEditEDstMAC->SetTextAlign(ES_LEFT);
	m_pEditEDstMAC->SetEditText(_T(""));
	
	m_pEditESrcMAC = new CBCGPRibbonEdit(ID_EDIT_E_SRC_MAC, 120, _T("ESrcMAC:"), 2);
	m_pEditESrcMAC->SetTextAlign(ES_LEFT);
	m_pEditESrcMAC->SetEditText(_T(""));
	
	m_pEditEDstIP = new CBCGPRibbonEdit(ID_EDIT_E_DST_IP, 120, _T("EDstIP :"), 2);
	m_pEditEDstIP->SetTextAlign(ES_LEFT);
	m_pEditEDstIP->SetEditText(_T(""));
	
	m_pEditESrcIP = new CBCGPRibbonEdit(ID_EDIT_E_SRC_IP, 120, _T("ESrcIP :"), 2);
	m_pEditESrcIP->SetTextAlign(ES_LEFT);
	m_pEditESrcIP->SetEditText(_T(""));

	m_pEditIDstMAC = new CBCGPRibbonEdit(ID_EDIT_I_DST_MAC, 120, _T("IDstMAC:"), 2);
	m_pEditIDstMAC->SetTextAlign(ES_LEFT);
	m_pEditIDstMAC->SetEditText(_T(""));
	
	m_pEditISrcMAC = new CBCGPRibbonEdit(ID_EDIT_I_SRC_MAC, 120, _T("ISrcMAC:"), 2);
	m_pEditISrcMAC->SetTextAlign(ES_LEFT);
	m_pEditISrcMAC->SetEditText(_T(""));
	
	m_pEditIDstIP = new CBCGPRibbonEdit(ID_EDIT_I_DST_IP, 120, _T("IDstIP :"), 2);
	m_pEditIDstIP->SetTextAlign(ES_LEFT);
	m_pEditIDstIP->SetEditText(_T(""));
	
	m_pEditISrcIP = new CBCGPRibbonEdit(ID_EDIT_I_SRC_IP, 120, _T("ISrcIP :"), 2);
	m_pEditISrcIP->SetTextAlign(ES_LEFT);
	m_pEditISrcIP->SetEditText(_T(""));

	CBCGPRibbonButton *pBtnWriteDebug = new CBCGPRibbonButton(ID_BTN_WRITE_DEBUG, _T("Write Debug Rule\ni"), 0, 3);

	pPanelDebugPacket->Add(m_pComboEType);
	pPanelDebugPacket->Add(m_pComboIType);
	pPanelDebugPacket->Add(m_pEditEDstMAC);
	pPanelDebugPacket->Add(m_pEditESrcMAC);
	pPanelDebugPacket->Add(m_pEditEDstIP);
	pPanelDebugPacket->Add(m_pEditESrcIP);
	pPanelDebugPacket->Add(m_pEditIDstMAC);
	pPanelDebugPacket->Add(m_pEditISrcMAC);
	pPanelDebugPacket->Add(m_pEditIDstIP);
	pPanelDebugPacket->Add(m_pEditISrcIP);
	
	pPanelDebugPacket->Add(pBtnWriteDebug);

	pPanelDebugPacket->SetKeys(_T("zl"));

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create "<TODO>" panel:
	/*
	CBCGPRibbonPanel* pPanelTODO = pBasicCategory->AddPanel (
		_T("<TODO>"));
	pPanelTODO->Add (new CBCGPRibbonLabel (_T("TODO: add elements here")));
	pPanelTODO->SetKeys (_T("zt"));
	*/

	// Add some hidden (non-ribbon) elements:
	CBCGPRibbonUndoButton* pUndo = new CBCGPRibbonUndoButton (ID_EDIT_UNDO, _T("Undo"), 4);

	pUndo->AddUndoAction (_T("Undo Item 1"));
	pUndo->AddUndoAction (_T("Undo Item 2"));
	pUndo->AddUndoAction (_T("Undo Item 3"));
	pUndo->AddUndoAction (_T("Undo Item 4"));
	pUndo->AddUndoAction (_T("Undo Item 5"));

	pBasicCategory->AddHidden (pUndo);

	// Add "<TODO>" category with "Clipboard" panel:
	CBCGPRibbonCategory* pTODOCategory = m_wndRibbonBar.AddCategory (
		_T("&TODO"), 
		0 /* TODO: ID-category small images */, 
		0 /* TODO: ID-category large images */);

	// Add quick access toolbar commands:
	CBCGPRibbonQATDefaultState qatState;

	qatState.AddCommand (ID_FILE_NEW, FALSE);
	qatState.AddCommand (ID_FILE_OPEN, FALSE);
	qatState.AddCommand (ID_FILE_SAVE);
	qatState.AddCommand (ID_FILE_PRINT_DIRECT);
	qatState.AddCommand (ID_FILE_PRINT_PREVIEW, FALSE);
	qatState.AddCommand (ID_EDIT_UNDO);

	m_wndRibbonBar.SetQuickAccessDefaultState (qatState);

	// Add "Style" button to the right of tabs:
	CBCGPRibbonButton* pStyleButton = new CBCGPRibbonButton (-1, _T("Style\ns"), -1, -1);
	pStyleButton->SetMenu (IDR_THEME_MENU, TRUE /* Right align */);

	m_wndRibbonBar.AddToTabs (pStyleButton);

	// Add "About" button to the right of tabs:
	m_wndRibbonBar.AddToTabs (new CBCGPRibbonButton (ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon (0)));

	return TRUE;
}

LRESULT CMainFrame::OnRibbonCustomize (WPARAM wp, LPARAM /*lp*/)
{
	ShowOptions (wp == 0 ? 1 : 0);
	return 1;
}

void CMainFrame::OnToolsOptions()
{
	ShowOptions (0);
}

void CMainFrame::ShowOptions (int nPage)
{
	// Create custom categories:
	
	// "Popular" items:
	CList<UINT, UINT> lstPopular;
	
	lstPopular.AddTail (ID_FILE_NEW);
	lstPopular.AddTail (ID_FILE_OPEN);
	lstPopular.AddTail (ID_FILE_SAVE);
	lstPopular.AddTail (ID_FILE_PRINT_PREVIEW);
	lstPopular.AddTail (ID_FILE_PRINT_DIRECT);
	lstPopular.AddTail (ID_EDIT_UNDO);
	
	// Hidden commands:
	CList<UINT,UINT> lstHidden;
	m_wndRibbonBar.GetItemIDsList (lstHidden, TRUE);
	
	// All commands:
	CList<UINT,UINT> lstAll;
	m_wndRibbonBar.GetItemIDsList (lstAll);
	
	// Create "Customize QAT" page:
	CBCGPRibbonCustomizeQATPage pageCustomizeQAT(&m_wndRibbonBar);

	pageCustomizeQAT.AddCustomCategory (_T("Popular Commands"), lstPopular);
	pageCustomizeQAT.AddCustomCategory (_T("Commands not in the Ribbon"), lstHidden);
	pageCustomizeQAT.AddCustomCategory (_T("All Commands"), lstAll);

	// Create Customize Ribbon" page:
	CBCGPRibbonCustomizeRibbonPage pageCustomizeRibbon(&m_wndRibbonBar);

	pageCustomizeRibbon.AddCustomCategory (_T("Popular Commands"), lstPopular);
	pageCustomizeRibbon.AddCustomCategory (_T("Commands not in the Ribbon"), lstHidden);
	pageCustomizeRibbon.AddCustomCategory (_T("All Commands"), lstAll);

	// Create property sheet:
	CBCGPPropertySheet propSheet (_T("Options"), this, nPage);
	propSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	propSheet.SetLook(CBCGPPropertySheet::PropSheetLook_List, 150 /* List width */);

	propSheet.AddPage (&pageCustomizeRibbon);

	propSheet.AddPage (&pageCustomizeQAT);

	// TODO: add your option pages here:
	// COptionsPage1 pageOptions1;
	// propSheet.AddPage (&pageOptions1);
	//
	// COptionsPage1 pageOptions2;
	// propSheet.AddPage (&pageOptions2);

	if (propSheet.DoModal() != IDOK)
	{
		return;
	}
}


void CMainFrame::OnViewWorkspace() 
{
	ShowControlBar (&m_wndWorkSpace,
					!(m_wndWorkSpace.IsVisible ()),
					FALSE, TRUE);
	RecalcLayout ();
}

void CMainFrame::OnUpdateViewWorkspace(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndWorkSpace.IsVisible ());
	pCmdUI->Enable (!GetDockManager ()->IsPrintPreviewValid ());
}

void CMainFrame::OnViewWorkspace2() 
{
	ShowControlBar (&m_wndWorkSpace2,
					!(m_wndWorkSpace2.IsVisible ()),
					FALSE, TRUE);
	RecalcLayout ();
}

void CMainFrame::OnUpdateViewWorkspace2(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndWorkSpace2.IsVisible ());
	pCmdUI->Enable (!GetDockManager ()->IsPrintPreviewValid ());
}

void CMainFrame::OnViewOutput() 
{
	ShowControlBar (&m_wndOutput,
					!(m_wndOutput.IsVisible ()),
					FALSE, TRUE);
	RecalcLayout ();
}

void CMainFrame::OnUpdateViewOutput(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndOutput.IsVisible ());
	pCmdUI->Enable (!GetDockManager ()->IsPrintPreviewValid ());
}

void CMainFrame::OnViewPropGrid() 
{
	ShowControlBar (&m_wndPropGrid,
					!(m_wndPropGrid.IsVisible ()),
					FALSE, TRUE);
	RecalcLayout ();
}

void CMainFrame::OnUpdateViewPropGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndPropGrid.IsVisible ());
	pCmdUI->Enable (!GetDockManager ()->IsPrintPreviewValid ());
}

LRESULT CMainFrame::On_MyMessageBox_Error(WPARAM wParam, LPARAM lParam)
{
	PMParam *pParam = (PMParam *) wParam;
	CString strText = pParam->strParam1;
	CString strCaption = pParam->strParam2;
	delete pParam;
	pParam = NULL;
	
	if (strCaption == _T(""))
	{
		strText = strText + _T(" Error.");
		strCaption = _T("Error");
	}
	
	BCGP_MSGBOXPARAMS params;
	
	params.lpszCaption = strCaption;
	params.lpszText = strText;
	params.bDrawButtonsBanner = TRUE;
	params.bUseNativeControls = FALSE;
	
	params.dwStyle = MB_OK;
	params.bIgnoreStandardButtons = FALSE;
	params.bShowCheckBox = FALSE;
	
	params.dwStyle |= MB_ICONEXCLAMATION;
	
	int result = BCGPMessageBoxIndirect(&params);
	
	//On_ProgressBar_Stop(NULL, NULL);
	
	return 0;
}

LRESULT CMainFrame::On_Output_Update_Connection(WPARAM wParam, LPARAM lParam)
{
	PMParam *pParam = (PMParam *) wParam;
	int nEvent = pParam->nEvent;
	CSocketManager *pSocketManager = (CSocketManager*) pParam->pObject1;
	delete pParam;
	pParam = NULL;
	
	switch (nEvent)
	{
	case EVT_CONSUCCESS:
		//AppendMessage( _T("Connection Established\r\n") );
		startServer();
		break;
	case EVT_CONFAILURE:
	case EVT_CONDROP:
	case EVT_ZEROLENGTH:
		if (m_bServer)
		{
			stopServer(pSocketManager->m_iIndex, TRUE);
		}
		else
		{
			stopClient();
		}
		return 0;
	}
	
	return 0;
}

LRESULT CMainFrame::On_Output_Console_List(WPARAM wParam, LPARAM lParam)
{
	PMParam *pParam = (PMParam *) wParam;
	CString strNo = pParam->strParam1;
	CString strContent = pParam->strParam2;
	delete pParam;
	pParam = NULL;
	
	COutputBar *outputBar = &m_wndOutput;
	outputBar->insert2ConsoleList(strNo, strContent);
	
	return 0;
}

LRESULT CMainFrame::On_Output_A_List(WPARAM wParam, LPARAM lParam)
{
	PMParam *pParam = (PMParam *) wParam;
	CString strNo = pParam->strParam1;
	CString strContent = pParam->strParam2;
	delete pParam;
	pParam = NULL;
	
	COutputBar *outputBar = &m_wndOutput;
	outputBar->insert2AList(strNo, strContent);
	
	return 0;
}

LRESULT CMainFrame::On_Output_B_List(WPARAM wParam, LPARAM lParam)
{
	PMParam *pParam = (PMParam *) wParam;
	CString strNo = pParam->strParam1;
	CString strContent = pParam->strParam2;
	delete pParam;
	pParam = NULL;
	
	COutputBar *outputBar = &m_wndOutput;
	outputBar->insert2BList(strNo, strContent);
	
	return 0;
}

void CMainFrame::OnBtnListNICs()
{
	if (m_pFilter)
	{
		delete m_pFilter;
		m_pFilter = NULL;
	}
	m_pFilter = new VNFilter();
	m_pFilter->listNICs();

	if (m_pFilter)
	{
		CString strOldIP;
		CString strOldMask;
		CString strOldGateway;
		m_pFilter->getOldIPConfig(strOldIP, strOldMask, strOldGateway);
		m_pEditOldIP->SetEditText(strOldIP);
		m_pEditOldMask->SetEditText(strOldMask);
		m_pEditOldGateway->SetEditText(strOldGateway);
	}
}

void CMainFrame::OnBtnConfigIP()
{
	if (m_pFilter)
	{
		CString strOldIP;
		CString strOldMask;
		CString strOldGateway;
		m_pFilter->getOldIPConfig(strOldIP, strOldMask, strOldGateway);
		m_pEditOldIP->SetEditText(strOldIP);
		m_pEditOldMask->SetEditText(strOldMask);
		m_pEditOldGateway->SetEditText(strOldGateway);

		CString strNewIP = m_pEditNewIP->GetEditText();
		CString strNewMask = m_pEditNewMask->GetEditText();
		CString strNewGateway = m_pEditNewGateway->GetEditText();

		m_pFilter->changeIPConfig(strNewIP, strNewMask, strNewGateway);
	}
	else
	{
		MyMessageBox_Error(_T("Please list NICs first!"), _T("Run Info"));
	}
}

void CMainFrame::OnBtnChangeToVIP()
{
	if (!m_pDispatcher)
	{
		MyMessageBox_Error(_T("Please confirm your status first."), _T("Run Info"));
		return;
	}

	if (!m_pFilter)
	{
		m_pFilter = new VNFilter();
	}
	m_pFilter->listNICs(FALSE);
	
	CString strNow_IP;
	CString strNow_Mask;
	CString strNow_Gate;
	m_pFilter->getOldIPConfig(strNow_IP, strNow_Mask, strNow_Gate);

	CString strVIP_IP = m_pDispatcher->m_pCurMappingRule->vIP;
	CString strVIP_Mask = m_pDispatcher->m_pCurMappingRule->vIP_Mask;
	CString strVIP_Gate = m_pDispatcher->m_pCurMappingRule->vIP_Gate;

	if (strNow_IP == strVIP_IP && strNow_Mask == strVIP_Mask && strNow_Gate == strVIP_Gate)
	{
		MyMessageBox_Error(_T("Now IP is equal to VIP, no need to change."), _T("Run Info"));
	}
	else
	{
		m_pFilter->changeIPConfig(strVIP_IP, strVIP_Mask, strVIP_Gate);
		m_pEditNow_IP->SetEditText(strVIP_IP);
		m_pEditNow_Mask->SetEditText(strVIP_Mask);
		m_pEditNow_Gate->SetEditText(strVIP_Gate);
	}
}

void CMainFrame::OnBtnChangeToPIP()
{
	if (!m_pDispatcher)
	{
		MyMessageBox_Error(_T("Please confirm your status first."), _T("Run Info"));
		return;
	}

	if (!m_pFilter)
	{
		m_pFilter = new VNFilter();
	}
	m_pFilter->listNICs(FALSE);
	
	CString strNow_IP;
	CString strNow_Mask;
	CString strNow_Gate;
	m_pFilter->getOldIPConfig(strNow_IP, strNow_Mask, strNow_Gate);
	
	if (!m_pDispatcher->m_pCurMappingRule)
	{
		MyMessageBox_Error(_T("OnBtnChangeToPIP"));
		return;
	}
	CString strPIP_IP = m_pDispatcher->m_pCurMappingRule->pIP;
	CString strPIP_Mask = m_pDispatcher->m_pCurMappingRule->pIP_Mask;
	CString strPIP_Gate = m_pDispatcher->m_pCurMappingRule->pIP_Gate;
	
	if (strNow_IP == strPIP_IP && strNow_Mask == strPIP_Mask && strNow_Gate == strPIP_Gate)
	{
		MyMessageBox_Error(_T("Now IP is equal to PIP, no need to change."), _T("Run Info"));
	}
	else
	{
		m_pFilter->changeIPConfig(strPIP_IP, strPIP_Mask, strPIP_Gate);
		m_pEditNow_IP->SetEditText(strPIP_IP);
		m_pEditNow_Mask->SetEditText(strPIP_Mask);
		m_pEditNow_Gate->SetEditText(strPIP_Gate);
	}
}

void CMainFrame::OnBtnSelectEType()
{
	int iIndex = m_pComboEType->GetCurSel();
	if (iIndex == TYPE_ARP_REQUEST || iIndex == TYPE_ARP_REPLY)
	{
		m_pComboIType->RemoveAllItems();
		m_pComboIType->AddItem(_T("None"), TYPE_NONE);
		m_pComboIType->SelectItem(_T("None"));
	}
	else //if (iIndex == TYPE_IP)
	{
		if (m_pComboIType->GetCount() == 1)
		{
			m_pComboIType->RemoveAllItems();
			m_pComboIType->AddItem(_T("IP"), TYPE_IP);
			m_pComboIType->AddItem(_T("ARP Request"), TYPE_ARP_REQUEST);
			m_pComboIType->AddItem(_T("ARP Reply"), TYPE_ARP_REPLY);
			m_pComboIType->AddItem(_T("None"), TYPE_NONE);
			m_pComboIType->SelectItem(_T("IP"));
		}
	}
}

	//DebugRule2Edits

void CMainFrame::refreshVIP()
{
	if (!m_pDispatcher || m_pDispatcher->m_nModeCenter != 0)
	{
		m_pEditVIP_IP->SetEditText(_T("N/A"));
		m_pEditVIP_Mask->SetEditText(_T("N/A"));
		m_pEditVIP_Gate->SetEditText(_T("N/A"));
		MyMessageBox_Error(_T("This is not a margin machine!"), _T("Run Info"));
		return;
	}
	else
	{
		m_pEditVIP_IP->SetEditText(m_pDispatcher->m_pCurMappingRule->vIP);
		m_pEditVIP_Mask->SetEditText(m_pDispatcher->m_pCurMappingRule->vIP_Mask);
		m_pEditVIP_Gate->SetEditText(m_pDispatcher->m_pCurMappingRule->vIP_Gate);
	}
}

void CMainFrame::refreshPIP()
{
	if (!m_pDispatcher || m_pDispatcher->m_nModeCenter != 0)
	{
		m_pEditPIP_IP->SetEditText(_T("N/A"));
		m_pEditPIP_Mask->SetEditText(_T("N/A"));
		m_pEditPIP_Gate->SetEditText(_T("N/A"));
		MyMessageBox_Error(_T("This is not a margin machine!"), _T("Run Info"));
		return;
	}
	else
	{
		m_pEditPIP_IP->SetEditText(m_pDispatcher->m_pCurMappingRule->pIP);
		m_pEditPIP_Mask->SetEditText(m_pDispatcher->m_pCurMappingRule->pIP_Mask);
		m_pEditPIP_Gate->SetEditText(m_pDispatcher->m_pCurMappingRule->pIP_Gate);
	}
}

void CMainFrame::refreshNowIP()
{
	if (!m_pFilter)
	{
		m_pFilter = new VNFilter();
	}
	m_pFilter->listNICs(FALSE);
	
	CString strNow_IP;
	CString strNow_Mask;
	CString strNow_Gate;
	m_pFilter->getOldIPConfig(strNow_IP, strNow_Mask, strNow_Gate);
	m_pEditNow_IP->SetEditText(strNow_IP);
	m_pEditNow_Mask->SetEditText(strNow_Mask);
	m_pEditNow_Gate->SetEditText(strNow_Gate);
}

void CMainFrame::OnBtnCheckMTU()
{
	if (!m_pFilter)
	{
		m_pFilter = new VNFilter();
	}

	int iMTU = m_pFilter->checkMTU();
	if (iMTU == DEFAULT_MTU)
	{
		m_pEditMTU->SetEditText(itos(DEFAULT_MTU) + _T(" (default)"));
	}
	else
	{
		m_pEditMTU->SetEditText(itos(iMTU));
	}
}

void CMainFrame::OnBtnChangeMTU()
{
	CString strMTU = m_pEditMTU->GetEditText();
	int iPos = 0;
	while (iPos < strMTU.GetLength())
	{
		TCHAR c = strMTU.GetAt(iPos);
		if (c < _T('0') || c > _T('9'))
		{
			break;
		}
		else
		{
			iPos ++;
		}
	}
	if (iPos == 0)
	{
		MyMessageBox_Error(_T("MTU不符合要求！"), _T("Run Info"));
		return;
	}
	else
	{
		strMTU = strMTU.Left(iPos);
	}

	int iMTU = stoi(strMTU);

	if (!m_pFilter)
	{
		m_pFilter = new VNFilter();
	}

	m_pFilter->changeMTUTo(iMTU);
}

void CMainFrame::OnBtnRestoreMTU()
{
	if (!m_pFilter)
	{
		m_pFilter = new VNFilter();
	}

	if (m_pFilter->restoreMTU())
	{
		m_pEditMTU->SetEditText(itos(DEFAULT_MTU) + _T(" (default)"));
	}
}

void CMainFrame::OnBtnRebootNICs()
{
	VNFilter::rebootNICs();
}

void CMainFrame::OnBtnClearLogs()
{
	m_wndOutput.clearConsoleList();
	m_wndOutput.clearAList();
	m_wndOutput.clearBList();
}

void CMainFrame::OnBtnReadRules()
{
	if (m_pDispatcher)
	{
		delete m_pDispatcher;
		m_pDispatcher = NULL;
	}
	m_pDispatcher = new VNDispatcher();
	m_pDispatcher->readAndParseRuleFile();
	m_pDispatcher->displayRules();
}

void CMainFrame::OnBtnSendRules()
{
	//if (!m_pDispatcher)
	//{
	//	m_pDispatcher = new VNDispatcher();
	//	m_pDispatcher->readPolicies();
	//}

	CVisualVNCView *pView = (CVisualVNCView*) GetActiveView();
	CString strText;
	pView->m_editPolicy.GetWindowText(strText);
	if (strText == _T(""))
	{
		MyMessageBox_Error(_T("Text to send is blank!"), _T("Run Info"));
		return;
	}
	sendAsServer(strText);
}

void CMainFrame::OnBtnLoadDriver()
{
	/*
	if (m_loader)
	{
		delete m_loader;
		m_loader = NULL;
	}

	CString strDriverName = m_pEditDriverName->GetEditText();
	CString strDriverLinkName = m_pEditDriverLinkName->GetEditText();
	m_loader = new VNDriverLoader(strDriverName, strDriverLinkName);

	m_loader->loadDriver();
	*/

	if (m_pLoaderEx)
	{
		delete m_pLoaderEx;
		m_pLoaderEx = NULL;
	}

	m_pLoaderEx = new VNDriverLoaderEx();
	m_pLoaderEx->installDriver_Passthru();
}

void CMainFrame::OnBtnUnloadDriver()
{
	/*
	if (m_loader)
	{
		delete m_loader;
		m_loader = NULL;
	}
	else
	{
		m_loader = new VNDriverLoader();
		m_loader->setJustUnload();
		delete m_loader;
		m_loader = NULL;
	}
	*/
	if (m_pLoaderEx)
	{
		delete m_pLoaderEx;
		m_pLoaderEx = NULL;
	}
	else
	{
		m_pLoaderEx = new VNDriverLoaderEx();
		m_pLoaderEx->setJustUnload();
		delete m_pLoaderEx;
		m_pLoaderEx = NULL;
	}
}

void CMainFrame::OnBtnCheckDriver()
{
	/*
	if (!m_loader)
	{
		MyMessageBox_Error(_T("Load driver first!"), _T("Error Info"));
		return;
	}

	if (!m_dispatcher)
	{
		m_dispatcher = new VNDispatcher();
		m_dispatcher->readPolicies();
	}
	m_dispatcher->setLoader(m_loader);
	m_dispatcher->switchFunction();
	*/
	if (!m_pLoaderEx)
	{
		m_pLoaderEx = new VNDriverLoaderEx();
	}

	if (m_pLoaderEx->queryAlive())
	{
		MyMessageBox_Error(_T("The driver is alive!"), _T("Run Info"));
	}
	else
	{
		MyMessageBox_Error(_T("The driver is NOT alive!"), _T("Run Info"));
	}
}

void CMainFrame::OnBtnConfirmStatus()
{
	if (!m_pDispatcher)
	{
		m_pDispatcher = new VNDispatcher();
	}

	if (!m_pFilter)
	{
		m_pFilter = new VNFilter();
	}
	m_pFilter->listNICs(FALSE);
	m_pDispatcher->setFilter(m_pFilter);

	int m_nMappingRuleIndex;
	m_pDispatcher->autoSelectModeAndRule(m_nMappingRuleIndex);

	CString strMode;
	switch (m_pDispatcher->m_nModeCenter)
	{
	case 1:
		strMode = _T("Center");
		break;
	case 0:
		strMode.Format(_T("Margin, Map Rule: %d"), m_nMappingRuleIndex);
		refreshVIP();
		refreshPIP();
		break;
	case -1:
		strMode = _T("Unknown");
	}

	CString strText;
	strText.Format(_T("Status: %s"), strMode);
	m_pEditStatus->SetEditText(strText);
}

void CMainFrame::OnBtnModeToServer()
{
	if (!m_pDispatcher)
	{
		m_pDispatcher = new VNDispatcher();
	}

	if (!m_pLoaderEx)
	{
		m_pLoaderEx = new VNDriverLoaderEx();
	}
	m_pDispatcher->setLoader(m_pLoaderEx);
	m_pDispatcher->Driver_WriteMode(MODE_FUNCTION_CENTER);
}

void CMainFrame::OnBtnModeToClient()
{
	if (!m_pDispatcher)
	{
		m_pDispatcher = new VNDispatcher();
	}
	
	if (!m_pLoaderEx)
	{
		m_pLoaderEx = new VNDriverLoaderEx();
	}
	m_pDispatcher->setLoader(m_pLoaderEx);
	m_pDispatcher->Driver_WriteMode(MODE_FUNCTION_MARGIN);
}

void CMainFrame::OnBtnModeToStop()
{
	if (!m_pDispatcher)
	{
		m_pDispatcher = new VNDispatcher();
	}
	
	if (!m_pLoaderEx)
	{
		m_pLoaderEx = new VNDriverLoaderEx();
	}
	m_pDispatcher->setLoader(m_pLoaderEx);
	m_pDispatcher->Driver_StartStop(FALSE);
}

void CMainFrame::OnBtnWriteRulesAndStart()
{
	if (!m_pDispatcher)
	{
		m_pDispatcher = new VNDispatcher();
	}
	if (!m_pDispatcher->m_pCurCoreRule)
	{
		m_pDispatcher->readAndParseRuleFile(); //读取规则文件
		m_pDispatcher->displayRules();
		OnBtnConfirmStatus(); //确定本主机角色
	}
	
	if (!m_pLoaderEx)
	{
		m_pLoaderEx = new VNDriverLoaderEx();
	}
	m_pDispatcher->setLoader(m_pLoaderEx);

	if (!m_pFilter)
	{
		m_pFilter = new VNFilter();
	}
	m_pFilter->listNICs(FALSE);
	m_pDispatcher->setFilter(m_pFilter);

	if (m_pDispatcher->writeMode()) //把角色写入驱动
	{
		m_pDispatcher->writeBothRules(); //把规则写入驱动
		m_pDispatcher->startDriver(); //命令驱动进入工作模式
	}
	else
	{
		MyMessageBox_Error(_T("OnBtnWriteRulesAndStart"));
		return;
	}
	
	if (m_pDispatcher->m_nModeCenter == 0)
	{
		m_pDispatcher->changeToVIP(); //把Margin主机的实际IP改为虚拟IP，即PIP->VIP
	}
}

void CMainFrame::OnBtnClearRulesAndStop()
{
	if (!m_pDispatcher)
	{
		MyMessageBox_Error(_T("OnBtnClearRulesAndStop"));
	}
	else
	{
		m_pDispatcher->stopDriver(); //命令驱动进入停止模式
		m_pDispatcher->Driver_ClearRules(); //清除驱动内的所有规则
		if (m_pDispatcher->m_nModeCenter == 0)
		{
			m_pDispatcher->changeToPIP(); //把Margin主机的虚拟IP改为实际IP，即VIP->PIP
		}
		m_pDispatcher->clearAllRules(); //清除应用层的所有规则
	}
}

void CMainFrame::OnEditDisplayDebug()
{
	int nIndex = m_pPltDisplayDebug->GetSelectedItem();
	if (!m_pDispatcher)
	{
		m_pDispatcher = new VNDispatcher();
	}
	m_pDispatcher->displayDebugRule(nIndex);
}

void CMainFrame::OnBtnWriteDebug()
{
	if (!m_pDispatcher)
	{
		m_pDispatcher = new VNDispatcher();
	}
	if (!m_pLoaderEx)
	{
		m_pLoaderEx = new VNDriverLoaderEx();
	}
	m_pDispatcher->setLoader(m_pLoaderEx);

	int eType;
	CString strEType = m_pComboEType->GetItem(m_pComboEType->GetCurSel());
	if (strEType == _T("IP"))
	{
		eType = TYPE_IP;
	}
	else if (strEType == _T("ARP Request"))
	{
		eType = TYPE_ARP_REQUEST;
	}
	else if (strEType == _T("ARP Reply"))
	{
		eType = TYPE_ARP_REPLY;
	}
	else
	{
		MyMessageBox_Error(_T("OnBtnWriteDebug"));
		return;
	}

	int iType;
	CString strIType = m_pComboIType->GetItem(m_pComboIType->GetCurSel());
	if (strIType == _T("IP"))
	{
		iType = TYPE_IP;
	}
	else if (strIType == _T("ARP Request"))
	{
		iType = TYPE_ARP_REQUEST;
	}
	else if (strIType == _T("ARP Reply"))
	{
		iType = TYPE_ARP_REPLY;
	}
	else if (strIType == _T("None"))
	{
		iType = TYPE_NONE;
	}
	else
	{
		MyMessageBox_Error(_T("OnBtnWriteDebug"));
		return;
	}

	CString strEDstMAC = m_pEditEDstMAC->GetEditText();
	CString strESrcMAC = m_pEditESrcMAC->GetEditText();
	CString strEDstIP = m_pEditEDstIP->GetEditText();
	CString strESrcIP = m_pEditESrcIP->GetEditText();
	CString strIDstMAC = m_pEditIDstMAC->GetEditText();
	CString strISrcMAC = m_pEditISrcMAC->GetEditText();
	CString strIDstIP = m_pEditIDstIP->GetEditText();
	CString strISrcIP = m_pEditISrcIP->GetEditText();

	if (strEDstMAC == _T("") || strESrcMAC == _T("") || strEDstIP == _T("") || strESrcIP == _T("") ||
		strIDstMAC == _T("") || strISrcMAC == _T("") || strIDstIP == _T("") || strISrcIP == _T(""))
	{
		MyMessageBox_Error(_T("OnBtnWriteDebug"));
		return;
	}

	m_pDispatcher->writeDebugRule(eType, iType, strEDstMAC, strESrcMAC, strEDstIP, strESrcIP, strIDstMAC, strISrcMAC, strIDstIP, strISrcIP);
}

void CMainFrame::OnBtnIPToMac()
{
	
// 	int a = 0x123456f8;
// 	int *b = (int*) 0x12345678;
// 	UCHAR *p = (UCHAR*) a;
// 	char buf[100];
// 	sprintf(buf, "%2X", a);
// 	sprintf(buf, "%2X", b);

	CString strIPToQuery = m_pEditIPToQuery->GetEditText();
	if (!m_pFilter)
	{
		m_pFilter = new VNFilter();
	}
	CString strMacToQuery = m_pFilter->getMacFromIP(strIPToQuery);
	m_pEditMacToQuery->SetEditText(strMacToQuery);
}

BOOL CMainFrame::PickNextAvailable()
{
	for (int i = 0; i < m_npSMServers.size(); i ++)
	{
		if (m_npSMServers[i]->IsOpen() && !m_npSMServers[i]->IsConnected())
		{
			m_pCurServer = m_npSMServers[i];
			return TRUE; //已经在等待了
		}
		if (!m_npSMServers[i]->IsOpen())
		{
			m_pCurServer = m_npSMServers[i];
			return FALSE;
		}
	}

	CSocketManager *pSocketManager = new CSocketManager(m_npSMServers.size());
	CVisualVNCView *pView = (CVisualVNCView*) GetActiveView();
	pSocketManager->SetMessageWindow(&(pView->m_editConsole));
	pSocketManager->SetServerState(TRUE);
	m_pCurServer = pSocketManager;
	m_npSMServers.push_back(pSocketManager);
	return FALSE;
	/*
	m_pCurServer = NULL;
	for(int i=0; i<MAX_CONNECTION; i++)
	{
		if (!m_SocketManager[i].IsOpen())
		{
			m_pCurServer = &m_SocketManager[i];
			break;
		}
	}
	*/
}

BOOL CMainFrame::startServer()
{
	m_bServer = TRUE;
	BOOL bSuccess = FALSE;
	if (PickNextAvailable())
	{
		return TRUE;
	}

	if (m_pCurServer != NULL)
	{
		CString strServerIP = m_pEditServerIP->GetEditText();
		if (m_nSockType == SOCK_TCP)
		{
			// no smart addressing - we use connection oriented
			m_pCurServer->SetSmartAddressing(FALSE);
			bSuccess = m_pCurServer->CreateSocket(strServerIP, m_strPort, AF_INET, SOCK_STREAM, 0); // TCP
		}
		else
		{
			//UDP没有用
			m_pCurServer->SetSmartAddressing(TRUE);
			bSuccess = m_pCurServer->CreateSocket(m_strPort, AF_INET, SOCK_DGRAM, SO_BROADCAST); // UDP
		}
		
		if (bSuccess && m_pCurServer->WatchComm())
		{
			/*
			GetDlgItem(IDC_BTN_SEND)->EnableWindow( TRUE );
			GetDlgItem(IDC_BTN_STOP)->EnableWindow( TRUE );
			NextDlgCtrl();
			GetDlgItem(IDC_BTN_START)->EnableWindow( FALSE );
			GetDlgItem(IDC_TCP)->EnableWindow( FALSE );
			GetDlgItem(IDC_UDP)->EnableWindow( FALSE );
			*/
			CString strServer, strAddr;
			m_pCurServer->GetLocalName(strServer.GetBuffer(256), 256);
			strServer.ReleaseBuffer();
			if (m_pCurServer->strSelfIP == _T(""))
			{
				m_pCurServer->GetLocalAddress(strAddr.GetBuffer(256), 256);
				strAddr.ReleaseBuffer();
			}
			else
			{
				strAddr = m_pCurServer->strSelfIP;
			}
			CString strMsg = _T("Server: ") + strServer;
			strMsg += _T(", No. ") + itos(m_pCurServer->m_iIndex);
			strMsg += _T(", @Address: ") + strAddr;
			strMsg += _T(" is running on port ") + m_strPort + CString("\r\n");
			m_pCurServer->AppendMessage(strMsg);
		}
	}
	
	return bSuccess;
}

void CMainFrame::stopServer(int iIndex, BOOL bEraseList)
{
	CSocketManager *pSM;

	if (iIndex < 0 || iIndex >= m_npSMServers.size())
	{
		MyMessageBox_Error(_T("stopServer"));
		return;
	}

	pSM = m_npSMServers[iIndex];
	m_npSMServers[iIndex]->StopComm();
	
	CString strServer, strAddr;
	m_npSMServers[iIndex]->GetLocalName(strServer.GetBuffer(256), 256);
	strServer.ReleaseBuffer();
	if (m_npSMServers[iIndex]->strSelfIP == _T(""))
	{
		m_npSMServers[iIndex]->GetLocalAddress(strAddr.GetBuffer(256), 256);
		strAddr.ReleaseBuffer();
	}
	else
	{
		strAddr = m_npSMServers[iIndex]->strSelfIP;
	}
	CString strMsg = _T("Server: ") + strServer;
	strMsg += _T(", No. ") + itos(m_npSMServers[iIndex]->m_iIndex);
	strMsg += _T(", @Address: ") + strAddr;
	strMsg += _T(" is stopping on port ") + m_strPort + CString("\r\n");
	m_npSMServers[iIndex]->AppendMessage(strMsg);
	
	
	if (bEraseList)
	{
		vector<CSocketManager*>::iterator it;
		for (it = m_npSMServers.begin(); it != m_npSMServers.end(); it ++)
		{
			if (*it == pSM)
			{
				//(*it)->StopComm();
				delete (*it);
				m_npSMServers.erase(it);
				break;
			}
		}
		for (int i = 0; i < m_npSMServers.size(); i ++)
		{
			m_npSMServers[i]->m_iIndex = i;
		}
	}
	else
	{
		//m_npSMServers[iIndex]->StopComm();
		delete m_npSMServers[iIndex];
	}
}

void CMainFrame::stopServers()
{
	// Disconnect all clients
	for (int i = m_npSMServers.size() - 1; i >= 0; i --)
	{
		stopServer(i, TRUE);
	}
	m_npSMServers.clear();

	/*
	while (m_npSMServers.size() != 0)
	{
		stopServer(0);
	}
	*/
	
	/*
	for(int i=0; i<MAX_CONNECTION; i++)
	m_SocketManager[i].StopComm();
	
	  if (!m_pCurServer->IsOpen())
	  {
	  GetDlgItem(IDC_BTN_START)->EnableWindow( TRUE );
	  PrevDlgCtrl();
	  GetDlgItem(IDC_BTN_STOP)->EnableWindow( FALSE );
		GetDlgItem(IDC_TCP)->EnableWindow( TRUE );
		GetDlgItem(IDC_UDP)->EnableWindow( TRUE );
	}
	*/
}

void CMainFrame::sendAsServer(CString strText)
{
	if (!m_bServer)
	{
		MyMessageBox_Error(_T("Please start server first!"), _T("Run Info"));
		return;
	}

	if (!m_pCurServer)
	{
		MyMessageBox_Error(_T("Please start server first!"), _T("Run Info"));
		return;
	}

	if (!m_pCurServer->IsOpen())
	{
		MyMessageBox_Error(_T("Please start server first!"), _T("Run Info"));
		return;
	}

	int nLen = strText.GetLength();
	stMessageProxy msgProxy;
	
	if (nLen > 0)
	{
		USES_CONVERSION;
		strText += _T("\r\n");
		nLen = strText.GetLength();
		if (m_nSockType == SOCK_UDP)
		{
			// send broadcast...
			msgProxy.address.CreateFrom(_T("255.255.255.255"), m_strPort);
			memcpy(msgProxy.byData, T2CA(strText), nLen);
			nLen += msgProxy.address.Size();
		}
		else
		{
			nLen = __min(sizeof(msgProxy.byData)-1, nLen+1);
			memcpy(msgProxy.byData, T2CA(strText), nLen);
		}
		
		// Send data to peer...
		if (m_nSockType == SOCK_UDP)
			m_pCurServer->WriteComm((const LPBYTE)&msgProxy, nLen, INFINITE);
		else
		{
			// Send to all clients
			for(int i = 0; i < m_npSMServers.size(); i ++)
			{
				if (m_npSMServers[i]->IsOpen() && m_npSMServers[i]->IsConnected())// && m_pCurServer != m_npSocketManager[i])
					m_npSMServers[i]->WriteComm(msgProxy.byData, nLen, INFINITE);
			}
		}
	}
}

void CMainFrame::startClient()
{
	m_bServer = FALSE;
	if (m_pSMClient)
	{
		MyMessageBox_Error(_T("Please stop the client first!"), _T("Run Info"));
		return;
		//delete m_pSMClient;
		//m_pSMClient = NULL;
	}
	m_pSMClient = new CSocketManager(0);
	CVisualVNCView *pView = (CVisualVNCView*) GetActiveView();
	m_pSMClient->SetMessageWindow(&(pView->m_editConsole));
	m_pSMClient->SetServerState(FALSE);// run as client
	m_pSMClient->SetSmartAddressing(FALSE); //always send to server
}

void CMainFrame::connectToServer(CString strServer, CString strPort)
{
	if (!m_pSMClient)
	{
		MyMessageBox_Error(_T("connectToServer"));
		return;
	}

	BOOL bSuccess;
	if (m_nSockType == SOCK_TCP)
	{
		bSuccess = m_pSMClient->ConnectTo( strServer, m_strPort, AF_INET, SOCK_STREAM); //TCP
	}
	else
	{
		bSuccess = m_pSMClient->ConnectTo( strServer, m_strPort, AF_INET, SOCK_DGRAM); //UDP
	}
	
	if (bSuccess && m_pSMClient->WatchComm())
	{
		/*
		GetDlgItem(IDC_BTN_SEND)->EnableWindow( TRUE );
		GetDlgItem(IDC_BTN_STOP)->EnableWindow( TRUE );
		NextDlgCtrl();
		GetDlgItem(IDC_BTN_START)->EnableWindow( FALSE );
		GetDlgItem(IDC_TCP)->EnableWindow( FALSE );
		GetDlgItem(IDC_UDP)->EnableWindow( FALSE );
		*/
		CString strMsg;
		
		if (m_nSockType == SOCK_TCP)
		{
			strMsg = _T("Connection established with server: ") + strServer;
			strMsg += _T(" on port ") + m_strPort + CString("\r\n");
			m_pSMClient->GetPeerName(m_SockPeer);
		}
		else
		{
			SockAddrIn addrin;
			m_pSMClient->GetSockName( addrin );	// just to get our current address
			LONG  uAddr = addrin.GetIPAddr();
			BYTE* sAddr = (BYTE*) &uAddr;
			short nPort = ntohs( addrin.GetPort() );
			CString strAddr;
			// Address is stored in network format...
			strAddr.Format(_T("IP: %u.%u.%u.%u, Port: %d"),
				(UINT)(sAddr[0]), (UINT)(sAddr[1]),
				(UINT)(sAddr[2]), (UINT)(sAddr[3]), nPort);
			strMsg = _T("Socket created: ") + strAddr + CString("\r\n");
			m_SockPeer.CreateFrom( strServer, m_strPort, AF_INET);
		}
		
		Output_Console_EditBox(strMsg);
	}
}

void CMainFrame::disconnectWithServer()
{
	if (m_pSMClient)
	{
		m_pSMClient->StopComm();
	}
	else
	{
		MyMessageBox_Error(_T("disconnectWithServer"));
	}
}

void CMainFrame::stopClient()
{
	if (m_pSMClient)
	{
		delete m_pSMClient;
		m_pSMClient = NULL;
	}
}

void CMainFrame::OnBtnStartServer()
{
	startServer();
}

void CMainFrame::OnBtnEndServer()
{
	stopServers();
}

void CMainFrame::OnBtnStartClient()
{
	startClient();

	CString strServerIP = m_pEditServerIP->GetEditText();
	connectToServer(strServerIP, m_strPort);
}

void CMainFrame::OnBtnEndClient()
{
	disconnectWithServer();
	stopClient();
}

