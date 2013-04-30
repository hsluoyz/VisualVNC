// OutputBar.cpp : implementation of the COutputBar class
//

#include "stdafx.h"
#include "VisualVNC.h"
#include "OutputBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nBorderSize = 1;

/////////////////////////////////////////////////////////////////////////////
// COutputBar

BEGIN_MESSAGE_MAP(COutputBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(COutputBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputBar construction/destruction

COutputBar::COutputBar()
{
	// TODO: add one-time construction code here

}

COutputBar::~COutputBar()
{
}

void COutputBar::initConsoleList()
{
	LONG lStyle;
	lStyle = GetWindowLong(m_listConsole.m_hWnd, GWL_STYLE); //获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_listConsole.m_hWnd, GWL_STYLE, lStyle); //设置style
	
	m_listConsole.SetExtendedStyle(m_listConsole.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	
	m_listConsole.InsertColumn(0, _T("Line No."));
	m_listConsole.InsertColumn(1, _T("Content"));
	m_listConsole.SetColumnWidth(0, 70);
}

void COutputBar::insert2ConsoleList(CString strNo, CString strContent)
{
	int iLimit = m_listConsole.GetScrollLimit(SB_VERT);
	int iPos = m_listConsole.GetScrollPos(SB_VERT);

	m_listConsole.InsertItem(m_listConsole.GetItemCount(), strNo);
	m_listConsole.SetItemText(m_listConsole.GetItemCount() - 1, 1, strContent);

	if (iLimit == iPos)
	{
		m_listConsole.EnsureVisible(m_listConsole.GetItemCount() - 1, FALSE);
	}
}

void COutputBar::clearConsoleList()
{
	m_listConsole.DeleteAllItems();
}

void COutputBar::initAList()
{
	LONG lStyle;
	lStyle = GetWindowLong(m_listA.m_hWnd, GWL_STYLE); //获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_listA.m_hWnd, GWL_STYLE, lStyle); //设置style
	
	m_listA.SetExtendedStyle(m_listA.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	
	m_listA.InsertColumn(0, _T("Shellcode No."));
	m_listA.InsertColumn(1, _T("Description"));
	m_listA.SetColumnWidth(0, 90);
}

void COutputBar::insert2AList(CString strNo, CString strContent)
{
	int iLimit = m_listA.GetScrollLimit(SB_VERT);
	int iPos = m_listA.GetScrollPos(SB_VERT);

	m_listA.InsertItem(m_listA.GetItemCount(), strNo);
	m_listA.SetItemText(m_listA.GetItemCount() - 1, 1, strContent);
	
	if (iLimit == iPos)
	{
		m_listA.EnsureVisible(m_listA.GetItemCount() - 1, FALSE);
	}
}

void COutputBar::clearAList()
{
	m_listA.DeleteAllItems();
}

void COutputBar::initBList()
{
	LONG lStyle;
	lStyle = GetWindowLong(m_listB.m_hWnd, GWL_STYLE); //获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_listB.m_hWnd, GWL_STYLE, lStyle); //设置style
	
	m_listB.SetExtendedStyle(m_listB.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	
	m_listB.InsertColumn(0, _T("Shellcode No."));
	m_listB.InsertColumn(1, _T("Description"));
	m_listB.SetColumnWidth(0, 90);
}

void COutputBar::insert2BList(CString strNo, CString strContent)
{
	int iLimit = m_listB.GetScrollLimit(SB_VERT);
	int iPos = m_listB.GetScrollPos(SB_VERT);
	
	m_listB.InsertItem(m_listB.GetItemCount(), strNo);
	m_listB.SetItemText(m_listB.GetItemCount() - 1, 1, strContent);
	
	if (iLimit == iPos)
	{
		m_listB.EnsureVisible(m_listB.GetItemCount() - 1, FALSE);
	}
}

void COutputBar::clearBList()
{
	m_listB.DeleteAllItems();
}


/////////////////////////////////////////////////////////////////////////////
// COutputBar message handlers

int COutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create tabs window:
	if (!m_wndTabs.Create (CBCGPTabWnd::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create list windows.
	// TODO: create your own window here:
	const DWORD dwViewStyle =	
		LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;
	
	m_listConsole.m_bVisualManagerStyle = TRUE;
	m_listA.m_bVisualManagerStyle = TRUE;
	m_listB.m_bVisualManagerStyle = TRUE;
	
	if (!m_listConsole.Create (dwViewStyle, rectDummy, &m_wndTabs, 2) ||
		!m_listA.Create (dwViewStyle, rectDummy, &m_wndTabs, 3) ||
		!m_listB.Create (dwViewStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("Failed to create output view\n");
		return -1;      // fail to create
	}

	// Attach list windows to tab:
	m_wndTabs.AddTab (&m_listConsole, _T("Packets"), -1);
	m_wndTabs.AddTab (&m_listA, _T("GetPC Location"), -1);
	m_wndTabs.AddTab (&m_listB, _T("IA-32 Instruction Recognition"), -1);

	initConsoleList();
	initAList();
	initBList();

	return 0;
}

void COutputBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	// Tab control should cover a whole client area:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	m_listConsole.SetColumnWidth(1, cx - 70 - 6);
	m_listA.SetColumnWidth(1, cx - 90 - 6);
	m_listB.SetColumnWidth(1, cx - 90 - 6);
}
