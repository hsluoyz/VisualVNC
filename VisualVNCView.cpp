// VisualVNCView.cpp : implementation of the CVisualVNCView class
//

#include "stdafx.h"
#include "VisualVNC.h"

#include "VisualVNCDoc.h"
#include "VisualVNCView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVisualVNCView

IMPLEMENT_DYNCREATE(CVisualVNCView, CFormView)

BEGIN_MESSAGE_MAP(CVisualVNCView, CFormView)
	//{{AFX_MSG_MAP(CVisualVNCView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)

	ON_MESSAGE(ID_GUI_MESSAGE + 11, On_Output_EditBox)
	ON_MESSAGE(ID_GUI_MESSAGE + 12, On_Clear_And_Output_EditBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisualVNCView construction/destruction

CVisualVNCView::CVisualVNCView()
	: CFormView(CVisualVNCView::IDD)
{
	//{{AFX_DATA_INIT(CVisualVNCView)
	//}}AFX_DATA_INIT

	EnableVisualManagerStyle();

	// TODO: add construction code here

}

CVisualVNCView::~CVisualVNCView()
{
}

void CVisualVNCView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVisualVNCView)
	DDX_Control(pDX, IDC_EDIT_POLICY, m_editPolicy);
	DDX_Control(pDX, IDC_EDIT_CONSOLE, m_editConsole);
	//}}AFX_DATA_MAP
}

BOOL CVisualVNCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CVisualVNCView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	EnableScrollBarCtrl(SB_BOTH, FALSE);
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	ResizeEdits();

	m_editPolicy.LimitText(2147483647);
	m_editConsole.LimitText(2147483647);

	
	CMainFrame *pMainFrm = (CMainFrame*) GetParentFrame();
	//pMainFrm->startServer();
}

/////////////////////////////////////////////////////////////////////////////
// CVisualVNCView printing

void CVisualVNCView::OnFilePrintPreview() 
{
	BCGPPrintPreview (this);
}

BOOL CVisualVNCView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVisualVNCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CVisualVNCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CVisualVNCView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CVisualVNCView diagnostics

#ifdef _DEBUG
void CVisualVNCView::AssertValid() const
{
	CFormView::AssertValid();
}

void CVisualVNCView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CVisualVNCDoc* CVisualVNCView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVisualVNCDoc)));
	return (CVisualVNCDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVisualVNCView message handlers

void CVisualVNCView::OnContextMenu(CWnd*, CPoint point)
{
	theApp.ShowPopupMenu (IDR_CONTEXT_MENU, point, this);
}

void CVisualVNCView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	if (m_editPolicy && m_editConsole)
	{
		ResizeEdits();
	}
}

void CVisualVNCView::ResizeEdits()
{
	CRect rect;
	this->GetClientRect(rect);

	CRect rect2 = rect;
	rect2.DeflateRect(0, 0, rect.Width() / 2, 0);
	CRect rect3 = rect2;
	rect3.OffsetRect(rect.Width() / 2, 0);

	rect2.DeflateRect(10, 10, 5, 10);
	rect3.DeflateRect(5, 10, 10, 10);

	/*
	CRect rect4 = rect;
	rect4.DeflateRect(0, rect.Height() * 3 / 4, rect.Width() / 2, 0);
	rect4.DeflateRect(10, 0, 5, 10);

	CRect rect5 = rect;
	rect5.DeflateRect(rect.Width() / 2, rect.Height() * 3 / 4, 0, 0);
	rect5.DeflateRect(5, 0, 10, 10);
	*/

	m_editPolicy.MoveWindow(rect2);
	m_editConsole.MoveWindow(rect3);

	CSize size;
	size.cx = rect.right;
	size.cy = rect.bottom;
	SetScrollSizes(MM_TEXT, size);
}

LRESULT CVisualVNCView::On_Clear_And_Output_EditBox(WPARAM wParam, LPARAM lParam)
{
	PMParam *pParam = (PMParam *) wParam;
	CMyRichEditCtrl *pEdit = (CMyRichEditCtrl*) pParam->pObject1;
	if (pEdit == NULL)
	{
		MyMessageBox_Error(_T("On_Output_EditBox"));
		return 0;
	}
	CString strContent = pParam->strParam1;
	
	pEdit->SetWindowText(_T(""));
	int nLen = pEdit->GetWindowTextLength();
	pEdit->SetSel(nLen, nLen);
	pEdit->ReplaceSel(strContent);
	return 0;
}

LRESULT CVisualVNCView::On_Output_EditBox(WPARAM wParam, LPARAM lParam)
{
	PMParam *pParam = (PMParam *) wParam;
	CMyRichEditCtrl *pEdit = (CMyRichEditCtrl*) pParam->pObject1;
	if (pEdit == NULL)
	{
		MyMessageBox_Error(_T("On_Output_EditBox"));
		return 0;
	}
	CString strContent = pParam->strParam1;

	int nLen = pEdit->GetWindowTextLength();
	pEdit->SetSel(nLen, nLen);
	pEdit->ReplaceSel(strContent);
	return 0;


	int iLimit = pEdit->GetScrollLimit(SB_VERT);
	int iPos = pEdit->GetScrollPos(SB_VERT);
	
	
	if (pEdit->GetWindowTextLength() == 0)
	{
		pEdit->iEditBoxFlag = 1;
	}
	else
	{
		strContent = _T("\r\n") + strContent;
	}
	
	int iLineOld, iLineNew;
	iLineOld = pEdit->GetFirstVisibleLine();
	
	int iTotalTextLength = pEdit->GetWindowTextLength();
	pEdit->SetSel(iTotalTextLength, iTotalTextLength);
	pEdit->ReplaceSel(strContent);
	pEdit->HideSelection(TRUE, FALSE);
	
	iLineNew = pEdit->GetFirstVisibleLine();
	
	if (iLimit - iPos <= 30 || pEdit->iEditBoxFlag)
	{
		if (iLimit - iPos <= 30)
		{
			pEdit->iEditBoxFlag = 0;
		}
		int iLimitNew = pEdit->GetScrollLimit(SB_VERT);
		int iPosNew = pEdit->GetScrollPos(SB_VERT);
		pEdit->LineScroll(iLimitNew - iPosNew);
	}
	else
	{
		pEdit->LineScroll(iLineOld - iLineNew);
	}
	
	delete pParam;
	pParam = NULL;
	return 0;
}