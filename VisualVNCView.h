// VisualVNCView.h : interface of the CVisualVNCView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALVNCVIEW_H__5639E315_5B3F_4B72_A19E_6DA91D24EA39__INCLUDED_)
#define AFX_VISUALVNCVIEW_H__5639E315_5B3F_4B72_A19E_6DA91D24EA39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
class CVisualVNCDoc;
#include "MyRichEditCtrl.h"

#define CFormView CBCGPFormView

class CVisualVNCView : public CFormView
{
protected: // create from serialization only
	CVisualVNCView();
	DECLARE_DYNCREATE(CVisualVNCView)

public:
	//{{AFX_DATA(CVisualVNCView)
	enum { IDD = IDD_VISUALVNC_FORM };
	CMyRichEditCtrl	m_editPolicy;
	CMyRichEditCtrl	m_editConsole;
	//}}AFX_DATA

// Attributes
public:
	CVisualVNCDoc* GetDocument();

public:

// Operations
public:
	void ResizeEdits();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualVNCView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVisualVNCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVisualVNCView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnFilePrintPreview();

	LRESULT On_Clear_And_Output_EditBox(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On_Output_EditBox(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in VisualVNCView.cpp
inline CVisualVNCDoc* CVisualVNCView::GetDocument()
   { return (CVisualVNCDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISUALVNCVIEW_H__5639E315_5B3F_4B72_A19E_6DA91D24EA39__INCLUDED_)
