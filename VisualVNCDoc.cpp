// VisualVNCDoc.cpp : implementation of the CVisualVNCDoc class
//

#include "stdafx.h"
#include "VisualVNC.h"

#include "VisualVNCDoc.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVisualVNCDoc

IMPLEMENT_DYNCREATE(CVisualVNCDoc, CDocument)

BEGIN_MESSAGE_MAP(CVisualVNCDoc, CDocument)
	//{{AFX_MSG_MAP(CVisualVNCDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisualVNCDoc construction/destruction

CVisualVNCDoc::CVisualVNCDoc()
{
	// TODO: add one-time construction code here

}

CVisualVNCDoc::~CVisualVNCDoc()
{
}

BOOL CVisualVNCDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	//SetTitle(_T("基于GNS3和虚拟云的虚拟网络实验平台系统"));
	SetTitle(_T("Visual Cloud & Network Connection System"));
	return TRUE;
}

void CVisualVNCDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	CDocument::SetPathName(lpszPathName, bAddToMRU);

	((CMainFrame*) AfxGetMainWnd ())->UpdateMRUFilesList ();
}



/////////////////////////////////////////////////////////////////////////////
// CVisualVNCDoc serialization

void CVisualVNCDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVisualVNCDoc diagnostics

#ifdef _DEBUG
void CVisualVNCDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVisualVNCDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVisualVNCDoc commands
