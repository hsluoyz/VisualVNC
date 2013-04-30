#include "MyMessageBox.h"
#include "StdAfx.h"

#include "MainFrm.h"
#include "VisualVNCView.h"

PMParam::PMParam(CString strParam1)
{
	this->strParam1 = strParam1;
}

PMParam::PMParam(CString strParam1, CString strParam2)
{
	this->strParam1 = strParam1;
	this->strParam2 = strParam2;
}

PMParam::PMParam(void *pObject1)
{
	this->pObject1 = pObject1;
}

PMParam::PMParam(void *pObject1, CString strParam1)
{
	this->pObject1 = pObject1;
	this->strParam1 = strParam1;
}

PMParam::PMParam(void *pObject1, UINT nEvent)
{
	this->pObject1 = pObject1;
	this->nEvent = nEvent;
}

/*
void MyMessageBox_Error_Old(CString strText, CString strCaption)
{
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
}

void MyMessageBox_Error_Old(CString strFunction)
{
	CString strText = strFunction + _T(" Error.");
	CString strCaption = _T("Error");
	MyMessageBox_Error(strText, strCaption);
}

void Output_Packets_List_Old(CString strNo, CString strContent)
{
	CMainFrame *pMainWnd = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	COutputBar *outputBar = &(pMainWnd->m_wndOutput);
	outputBar->insert2PacketsList(strNo, strContent);
}
*/
/*
void Output_Console2(CString strContent)
{
	CMainFrame *pMainWnd = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CVisualPSDView *pView = (CVisualPSDView*) pMainWnd->GetActiveView();
	CEdit *pEdit = &(pView->m_editConsole);

	//CString strTmp;
	//pEdit->GetWindowText(strTmp);
	//strTmp += (strContent + "\r\n");
	//pEdit->SetWindowText(strTmp);
	pEdit->SetWindowText(strContent);
}
*/

void MyMessageBox_Error(CString strText, CString strCaption)
{
	PMParam *pParam = new PMParam(strText, strCaption);
	AfxGetApp()->m_pMainWnd->SendMessage(ID_GUI_MESSAGE + 0, (WPARAM) pParam, NULL);
}

void MyMessageBox_Error(CString strFunction)
{
	PMParam *pParam = new PMParam(strFunction);
	AfxGetApp()->m_pMainWnd->SendMessage(ID_GUI_MESSAGE + 1, (WPARAM) pParam, NULL);
}

void Output_Console_List(CString strNo, CString strContent)
{
	PMParam *pParam = new PMParam(strNo, strContent);
	AfxGetApp()->m_pMainWnd->PostMessage(ID_GUI_MESSAGE + 2, (WPARAM) pParam, NULL);
}

void Output_A_List(CString strNo, CString strContent)
{
	PMParam *pParam = new PMParam(strNo, strContent);
	AfxGetApp()->m_pMainWnd->PostMessage(ID_GUI_MESSAGE + 3, (WPARAM) pParam, NULL);
}

void Output_B_List(CString strNo, CString strContent)
{
	PMParam *pParam = new PMParam(strNo, strContent);
	AfxGetApp()->m_pMainWnd->PostMessage(ID_GUI_MESSAGE + 4, (WPARAM) pParam, NULL);
}

void Output_Policy_EditBox(CString strContent)
{
	CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CVisualVNCView *pView = (CVisualVNCView*) pMainWnd->GetActiveView();
	PMParam *pParam = new PMParam(&(pView->m_editPolicy), strContent);
	pView->PostMessage(ID_GUI_MESSAGE + 10, (WPARAM) pParam, NULL);
}

void Output_Console_EditBox(CString strContent)
{
	CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CVisualVNCView *pView = (CVisualVNCView*) pMainWnd->GetActiveView();
	PMParam *pParam = new PMParam(&(pView->m_editConsole), strContent);
	pView->PostMessage(ID_GUI_MESSAGE + 11, (WPARAM) pParam, NULL);
}

void Clear_And_Output_Policy_EditBox(CString strContent)
{
	CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CVisualVNCView *pView = (CVisualVNCView*) pMainWnd->GetActiveView();
	PMParam *pParam = new PMParam(&(pView->m_editPolicy), strContent);
	pView->PostMessage(ID_GUI_MESSAGE + 12, (WPARAM) pParam, NULL);
}

void Output_Update_Connection(UINT nEvent, void *pThis)
{
	CMainFrame *pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	PMParam *pParam = new PMParam(pThis, nEvent);
	pMainWnd->PostMessage(ID_GUI_MESSAGE + 13, (WPARAM) pParam, NULL);
}

