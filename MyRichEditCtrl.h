#if !defined(AFX_MYRICHEDITCTRL_H__3FE96240_D7C7_4933_99C5_638BBEFD799C__INCLUDED_)
#define AFX_MYRICHEDITCTRL_H__3FE96240_D7C7_4933_99C5_638BBEFD799C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyRichEditCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyRichEditCtrl window

class CMyRichEditCtrl : public CRichEditCtrl
{
// Construction
public:
	CMyRichEditCtrl();

// Attributes
public:
	int iEditBoxFlag;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyRichEditCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyRichEditCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyRichEditCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYRICHEDITCTRL_H__3FE96240_D7C7_4933_99C5_638BBEFD799C__INCLUDED_)
