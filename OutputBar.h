// OutputBar.h : interface of the COutputBar class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTBAR_H__2E7E4242_211F_498F_B9CA_8FF8E33E4C01__INCLUDED_)
#define AFX_OUTPUTBAR_H__2E7E4242_211F_498F_B9CA_8FF8E33E4C01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COutputBar : public CBCGPDockingControlBar
{
public:
	COutputBar();

// Attributes
protected:
	CBCGPTabWnd	m_wndTabs;

	CBCGPListCtrl	m_listConsole;
	CBCGPListCtrl	m_listA;
	CBCGPListCtrl	m_listB;

// Operations
public:
	void initConsoleList();
	void insert2ConsoleList(CString strNo, CString strContent);
	void clearConsoleList();
	void initAList();
	void insert2AList(CString strNo, CString strContent);
	void clearAList();
	void initBList();
	void insert2BList(CString strNo, CString strContent);
	void clearBList();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputBar();

// Generated message map functions
protected:
	//{{AFX_MSG(COutputBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTBAR_H__2E7E4242_211F_498F_B9CA_8FF8E33E4C01__INCLUDED_)
