// VisualVNC.h : main header file for the VISUALVNC application
//

#if !defined(AFX_VISUALVNC_H__D72A5A9B_989A_4D6E_92E2_E1448D64D516__INCLUDED_)
#define AFX_VISUALVNC_H__D72A5A9B_989A_4D6E_92E2_E1448D64D516__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CVisualVNCApp:
// See VisualVNC.cpp for the implementation of this class
//

class CVisualVNCApp : public CWinApp,
					  public CBCGPWorkspace
{
public:
	CVisualVNCApp();

	// Override from CBCGPWorkspace
	virtual void PreLoadState ();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualVNCApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CVisualVNCApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


extern CVisualVNCApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISUALVNC_H__D72A5A9B_989A_4D6E_92E2_E1448D64D516__INCLUDED_)
